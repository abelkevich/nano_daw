#include "pure_wave.h"
#include <iostream>
#include <windows.h>

void codecTest()
{
    CodecFileInfo file_info;

    if (loadFile(file_info, "res\\samples_16bit_48khz\\drums_overhead_l.wav") != 0)
    {
        std::cout << "Error loading";
    }

    file_info.path = "res\\mix.wav";

    if (saveFile(file_info, 2) != 0)
    {
        std::cout << "Error saving";
    }
}

void TestSetAndGetBytes()
{
#define N 5
    int16_t input_file[N] = { -32000, -16000, 0, 16000, 32000 };

    uint8_t* input_buffer = (uint8_t*) input_file;
    uint32_t input_buffer_size = N * 2;

    int32_t inter_buff[N] = { 0, 0, 0, 0, 0 };

    for (uint32_t smp_ind = 0; smp_ind < N; smp_ind++)
    {
        uint32_t offset = smp_ind * 2;
        int32_t val = getBytes(input_buffer + offset, 2);
        inter_buff[smp_ind] = val;
    }

    printf("input_file vals: ");
    for (uint8_t i = 0; i < N; i++)
    {
        printf("%x ", input_file[i] & 0x0000FFFF);
    }
    printf("\n\n");


    printf("inter_buff vals: ");
    for (uint8_t i = 0; i < N; i++)
    {
        printf("%x ", inter_buff[i]);
    }
    printf("\n\n");
}

int main()
{
    //__debugbreak();

    //TestSetAndGetBytes();
    codecTest();


    return 0;
}