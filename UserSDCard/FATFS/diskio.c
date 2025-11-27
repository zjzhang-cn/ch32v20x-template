/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "ch32v20x_spi.h"
#include <stdio.h>
#include <string.h>
#include "family.h"
// SD card SPI commands
#define CMD0 (0x40 + 0) // GO_IDLE_STATE
#define CMD12 (0x40 + 12)
#define CMD17 (0x40 + 17) // READ_SINGLE_BLOCK
#define CMD24 (0x40 + 24) // WRITE_BLOCK
#define CMD55 (0x40 + 55)
#define CMD58 (0x40 + 58)
#define ACMD41 (0x40 + 41)
#define CMD8 (0x40 + 8) // SEND_IF_COND

#define SD_CS_HIGH() GPIO_SetBits(GPIOA, GPIO_Pin_3)
#define SD_CS_LOW() GPIO_ResetBits(GPIOA, GPIO_Pin_3)

// SPI1 initialization function
void SPI1_Init_LOW(void)
{
    SPI_InitTypeDef SPI_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

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

    SPI_StructInit(&SPI_InitStructure);
    // SPI1 configuration
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);
}
void SPI1_Init_HIGH(void)
{
    SPI_InitTypeDef SPI_InitStructure = {0};
    SPI_StructInit(&SPI_InitStructure);
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
    SPI_Cmd(SPI1, DISABLE);
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);
}

static uint8_t spi_send(uint8_t data)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
        ;
    return SPI_I2S_ReceiveData(SPI1);
}

static void spi_send_multi(const uint8_t *buff, uint32_t len)
{
    while (len--)
        spi_send(*buff++);
}

static uint8_t sd_send_cmd(uint8_t cmd, uint32_t arg)
{
    uint8_t res, n;

    SD_CS_HIGH();
    spi_send(0xFF);
    SD_CS_LOW();
    spi_send(0xFF);

    spi_send(cmd);
    spi_send((uint8_t)(arg >> 24));
    spi_send((uint8_t)(arg >> 16));
    spi_send((uint8_t)(arg >> 8));
    spi_send((uint8_t)arg);

    uint8_t crc = 0x01;
    if (cmd == CMD0)
        crc = 0x95;
    if (cmd == CMD8)
        crc = 0x87;
    spi_send(crc);

    n = 10;
    do
    {
        res = spi_send(0xFF);
    } while ((res & 0x80) && --n);

    return res;
}

static int sd_wait_ready(void)
{
    uint8_t res;
    uint32_t tmr = 50000;
    do
    {
        res = spi_send(0xFF);
        if (res == 0xFF)
            return 1;
    } while (--tmr);
    return 0;
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(void)
{

    DSTATUS stat = STA_NOINIT;
    uint8_t i, resp;
    SPI1_Init_LOW(); // Initialize SPI1
    // Send CMD12 to terminate any pending transfer
    sd_send_cmd(CMD12, 0);
    SD_CS_HIGH();
    spi_send(0xFF);
    // Send 80 dummy clocks
    SD_CS_HIGH();
    for (i = 0; i < 10; i++)
    {
        spi_send(0xFF);
    }
    delay_ms(500);
    // Send CMD0 to enter idle state
    while (sd_send_cmd(CMD0, 0) != 0x01)
    {
        // Retry until in idle state
        SD_CS_HIGH();
        for (i = 0; i < 10; i++)
            spi_send(0xFF); // 80 dummy clocks
        delay_ms(100);
    }

    if (sd_send_cmd(CMD8, 0x1AA) == 0x01)
    {
        // SDC Ver2+
        for (i = 0; i < 4; i++)
            spi_send(0xFF); // Get trailing return value of R7 resp
        // Wait for leaving idle state (ACMD41 with HCS bit)
        do
        {
            resp = sd_send_cmd(CMD55, 0);
            resp = sd_send_cmd(ACMD41, 0x40000000);
        } while (resp != 0x00);
        stat = 0; // Initialization succeeded
        SPI1_Init_HIGH();
    }
    else
    {
        // SDC Ver1 or MMC
        for (i = 0; i < 4; i++)
            spi_send(0xFF); // Get trailing return value of R7 resp
        // Wait for leaving idle state
        do
        {
            resp = sd_send_cmd(CMD55, 0);
            resp = sd_send_cmd(ACMD41, 0);
        } while (resp != 0x00);
        stat = 0; // Initialization succeeded
    }

    SD_CS_HIGH();
    spi_send(0xFF);

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp(
    BYTE *buff,   /* Pointer to the destination object */
    DWORD sector, /* Sector number (LBA) */
    UINT offset,  /* Offset in the sector */
    UINT count    /* Byte count (bit15:destination) */
)
{
    DRESULT res = RES_ERROR;
    uint8_t token;
    uint16_t bc;

    if (sd_send_cmd(CMD17, sector) == 0x00)
    {
        // Wait for data token (0xFE)
        for (bc = 0; bc < 10000; bc++)
        {
            token = spi_send(0xFF);
            if (token == 0xFE)
                break;
        }
        if (token == 0xFE)
        {
            // Skip bytes before offset
            for (bc = 0; bc < offset; bc++)
                spi_send(0xFF);
            // Read requested bytes
            for (bc = 0; bc < count; bc++)
                *buff++ = spi_send(0xFF);
            // Skip remaining bytes + CRC
            for (bc = 0; bc < (512 - offset - count + 2); bc++)
                spi_send(0xFF);
            res = RES_OK;
        }
    }
    SD_CS_HIGH();
    spi_send(0xFF);
    return res;
}

/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

static uint16_t wp_offset = 0;
static uint8_t wp_buffer[512];
static DWORD wp_sector = 0;

DRESULT disk_writep(
    const BYTE *buff, /* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
    DWORD sc          /* Sector number (LBA) or Number of bytes to send */
)
{
    DRESULT res = RES_ERROR;

    if (!buff)
    {
        if (sc)
        {
            // Initiate write process
            wp_offset = 0;
            wp_sector = sc;
            memset(wp_buffer, 0xFF, sizeof(wp_buffer));
            res = RES_OK;
        }
        else
        {
            // Finalize write process
            if (sd_send_cmd(CMD24, wp_sector) == 0x00)
            {
                spi_send(0xFF);
                spi_send(0xFE); // Data token
                spi_send_multi(wp_buffer, 512);
                spi_send(0xFF); // Dummy CRC
                spi_send(0xFF);
                // Wait for data response
                uint8_t resp = spi_send(0xFF);
                if ((resp & 0x1F) == 0x05)
                {
                    if (sd_wait_ready())
                        res = RES_OK;
                }
            }
            SD_CS_HIGH();
            spi_send(0xFF);
        }
    }
    else
    {
        // Send data to the disk (buffered)
        memcpy(&wp_buffer[wp_offset], buff, sc);
        wp_offset += sc;
        res = RES_OK;
    }

    return res;
}
