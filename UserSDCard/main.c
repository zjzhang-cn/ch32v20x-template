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
// Add SPI header if available
#include "ch32v20x_spi.h" // Adjust if your SPI header has a different name
#include "pff.h"          // Add this include
#include "family.h"
/* Global typedef */

/* Global define */

/* Global Variable */

// SPI1 initialization function
void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure = {0};

    // Enable clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    // SPI1 NSS pin (CS) - Initialize first and set HIGH
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_3); // Set CS high immediately

    // SPI1 SCK, MOSI pin configuration
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // SPI1 MISO pin configuration
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // SPI1 configuration
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
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
            printf("%s\t%d\r\n", fno.fname, fno.fsize);
        }

        // Open and read file with speed measurement
        UINT br;
        BYTE buffer[512]; // Larger buffer for better speed measurement
        res = pf_open("BOOTCODE.BIN");
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
                
                // Print first 256 bytes in hex format
                if (total_bytes <= 256)
                {
                    for (UINT i = 0; i < br && (total_bytes - br + i) < 256; i++)
                    {
                        printf("%02X ", buffer[i]);
                        if (((total_bytes - br + i + 1) % 16) == 0)
                            printf("\r\n");
                    }
                }
            } while (br == sizeof(buffer));
            
            uint32_t time_ms = board_millis()-start_time;
            
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
    board_init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("This is printf example\r\n");
    // Add delay for system stabilization after power-up
    printf("System initializing...\r\n");
    
    SPI1_Init(); // Initialize SPI1

    FATFSTEST();

    while (1)
    {

        Delay_Ms(2000);
    }
}
