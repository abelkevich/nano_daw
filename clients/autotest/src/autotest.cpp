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


SOCKET g_ClientSocket = INVALID_SOCKET;

extern "C" EXPORTED std::string getCommand()
{
    char recvbuf[4096];
    int recvbuflen = 4096;
    int recv_status = recv(g_ClientSocket, recvbuf, recvbuflen, 0);

    if (recv_status > 0)
    {
        std::string cmd(recvbuf, recv_status);
        printf("nano daw autotest> %s\n", cmd.c_str());
        return cmd;
    }
    else if (recv_status == 0)
    {
        printf("Connection closing...\n");
    }
    else
    {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(g_ClientSocket);
        WSACleanup();
    }
}

extern "C" EXPORTED void receiveResponse(const std::string & response)
{
    printf("%s\n", response.c_str());

    if (send(g_ClientSocket, response.c_str(), response.size(), 0) == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(g_ClientSocket);
        WSACleanup();
    }
}

extern "C" EXPORTED bool init()
{
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed\n");
        return false;
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
        return false;
    }

    // Create a SOCKET for connecting to server
    SOCKET ListenSocket = INVALID_SOCKET;

    ListenSocket = socket(result_addrinfo->ai_family, result_addrinfo->ai_socktype, result_addrinfo->ai_protocol);

    if (ListenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result_addrinfo);
        WSACleanup();
        return false;
    }

    // Setup the TCP listening socket
    if (bind(ListenSocket, result_addrinfo->ai_addr, (int)result_addrinfo->ai_addrlen) == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result_addrinfo);
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }

    freeaddrinfo(result_addrinfo);

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }

    // Accept a client socket
    g_ClientSocket = accept(ListenSocket, NULL, NULL);
    if (g_ClientSocket == INVALID_SOCKET)
    {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    return true;
}

extern "C" EXPORTED void kill()
{
    // shutdown the connection since we're done
    if (shutdown(g_ClientSocket, SD_SEND) == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(g_ClientSocket);
        WSACleanup();
        return;
    }

    // cleanup
    closesocket(g_ClientSocket);
    WSACleanup();
}