/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *USART Print debugging routine:
 *USART1_Tx(PA9).
 *This example demonstrates using USART1(PA9) as a print debug port output.
 *
 */

#include "debug.h"
#include "pff.h"          // Add this include
#include "family.h"
/* Global typedef */

/* Global define */

/* Global Variable */
void led_blinking_task(void)
{
    static bool led_state = false;
    board_led_write(led_state);
    led_state = 1 - led_state; // toggle
}

void FATFSTEST()
{
    FATFS fs;
    DIR dir;
    FILINFO fno;
    FRESULT res;
    int retry = 0;

    // Add delay and retry logic for SD card power-up
    printf("Waiting for SD card to be ready...\r\n");
    // Retry mounting up to 3 times
    for (retry = 0; retry < 3; retry++)
    {
        res = pf_mount(&fs);
        printf("pf_mount attempt %d: %d\r\n", retry + 1, res);

        if (res == FR_OK)
        {
            break;
        }
        delay_ms(500); // Wait before retrying
    }

    if (res != FR_OK)
    {
        printf("Failed to mount SD card after %d attempts\r\n", retry);
        return;
    }

    if (res == FR_OK)
    {
        res = pf_opendir(&dir, ""); // Open root directory
        printf("pf_opendir: %d\r\n", res);

        while (res == FR_OK)
        {
            res = pf_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0)
                break; // End of directory
            printf("%s\t%ld\r\n", fno.fname, fno.fsize);
        }

        // Open and read file with speed measurement
        UINT br;
        BYTE buffer[512]; // Larger buffer for better speed measurement
        res = pf_open("START4CD.ELF");
        printf("pf_open: %d\r\n", res);
        if (res == FR_OK)
        {
            uint32_t total_bytes = 0;
            uint32_t start_time = board_millis();

            // Read entire file
            do
            {
                res = pf_read(buffer, sizeof(buffer), &br);
                if (res != FR_OK)
                    break;
                total_bytes += br;
                led_blinking_task();
                // Print first 256 bytes in hex format
                if (total_bytes <= 512)
                {
                    for (UINT i = 0; i < br && (total_bytes - br + i) < 256; i++)
                    {
                        printf("%02X ", buffer[i]);
                        if (((total_bytes - br + i + 1) % 16) == 0)
                            printf("\r\n");
                    }
                }
            } while (br == sizeof(buffer));

            uint32_t time_ms = board_millis() - start_time;

            printf("\r\n\r\n=== Read Statistics ===\r\n");
            printf("Total bytes read: %lu\r\n", total_bytes);
            printf("Time elapsed: %lu ms\r\n", time_ms);

            if (time_ms > 0)
            {
                uint32_t speed_kbps = (total_bytes * 1000) / (time_ms * 1024);
                printf("Read speed: %lu KB/s\r\n", speed_kbps);
            }
            else
            {
                printf("Read speed: too fast to measure accurately\r\n");
            }
        }
    }
}



/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    USART_Printf_Init(115200);
    board_init();
    printf("SystemClk:%ld\r\n", SystemCoreClock);
    printf("ChipID:%08lx\r\n", DBGMCU_GetCHIPID());
    printf("This is printf example\r\n");
    // Add delay for system stabilization after power-up
    printf("System initializing...\r\n");
    FATFSTEST();
    uint32_t blink_interval_ms = 1000;
    uint32_t start_ms = 0;
    while (1)
    {
        // Blink every interval ms
        if (board_millis() - start_ms < blink_interval_ms)
            continue; // not enough time
        start_ms += blink_interval_ms;
        led_blinking_task();
    }
}
