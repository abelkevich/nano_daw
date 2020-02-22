#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "client_api.h"

#pragma comment (lib, "Ws2_32.lib")

void spawnClient(callback_t transmitter)
{
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed\n");
        return;
    }

    // Resolve the server address and port
    struct addrinfo init_addrinfo;
    ZeroMemory(&init_addrinfo, sizeof(init_addrinfo));
    init_addrinfo.ai_family = AF_INET;
    init_addrinfo.ai_socktype = SOCK_STREAM;
    init_addrinfo.ai_protocol = IPPROTO_TCP;
    init_addrinfo.ai_flags = AI_PASSIVE;

    struct addrinfo* result_addrinfo = NULL;
    if (getaddrinfo(NULL, "27015", &init_addrinfo, &result_addrinfo) != 0)
    {
        printf("getaddrinfo failed");
        WSACleanup();
        return;
    }


    // Create a SOCKET for connecting to server
    SOCKET ListenSocket = INVALID_SOCKET;

    ListenSocket = socket(result_addrinfo->ai_family, result_addrinfo->ai_socktype, result_addrinfo->ai_protocol);
    
    if (ListenSocket == INVALID_SOCKET) 
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result_addrinfo);
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    if (bind(ListenSocket, result_addrinfo->ai_addr, (int)result_addrinfo->ai_addrlen) == SOCKET_ERROR) 
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result_addrinfo);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    freeaddrinfo(result_addrinfo);

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) 
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    // Accept a client socket
    SOCKET ClientSocket = INVALID_SOCKET;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) 
    {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
    char recvbuf[4096];
    int recvbuflen = 4096;
    int recv_status = 0;

    do
    {
        recv_status = recv(ClientSocket, recvbuf, recvbuflen, 0);
        
        if (recv_status > 0)
        {
            std::string cmd(recvbuf, recv_status);

            printf("nano daw autotest> %s\n", cmd.c_str());

            std::string resp = transmitter(cmd);

            printf("%s\n", resp.c_str());

            if (send(ClientSocket, resp.c_str(), resp.size(), 0) == SOCKET_ERROR)
            {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return;
            }
        }
        else if (recv_status == 0)
        {
            printf("Connection closing...\n");
        }
        else 
        {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return;
        }

    } while (recv_status > 0);

    // shutdown the connection since we're done
    if (shutdown(ClientSocket, SD_SEND) == SOCKET_ERROR) 
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
}