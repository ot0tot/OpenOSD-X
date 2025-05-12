
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "main.h"
#include "log.h"
#include "cli.h"

#define STX     0x02
#define ETX     0x03
#define MAGIC   0xa5

typedef enum {
    WAIT_STX = 0,
    WAIT_MAGIC,
    WAIT_LEN,
    WAIT_PAYLOAD,
    WAIT_CRC1,
    WAIT_CRC2,
    WAIT_ETX,
    WAIT_RESTART,
} CLI_STATE;

typedef enum {
    CMD_VERSION=0,
    CMD_BOOTLOADER,
    CMD_RESET,
    CMD_VTXTEST
} CLI_CMD;


uint8_t cli_buff[256];
CLI_STATE cli_state = WAIT_STX;
uint8_t ack_paket[] = {STX, MAGIC, 0x1, 0, 0, ETX};

uint16_t crc16_modbus(const uint8_t* data, uint16_t length) 
{
    uint16_t crc = 0xFFFF;
    for (uint16_t pos = 0; pos < length; pos++) {
        crc ^= (uint16_t)data[pos];
        for (int i = 0; i < 8; i++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

int decode_le128(const uint8_t *in, uint32_t *value) {
    *value = 0;
    for (int i = 0; i < 5; i++) {
        *value |= (in[i] & 0x7F) << (7 * i);
        if ((in[i] & 0x80) == 0) {
            return i + 1;
        }
    }
    return -1;
}


void initCli(void)
{
    cli_state = WAIT_STX;
}

void sendCli(uint8_t cmd, uint8_t *data, uint8_t len)
{
 static uint8_t send_buf[128];

    send_buf[0] = STX;
    send_buf[1] = MAGIC;
    send_buf[2] = len+1;
    send_buf[3] = cmd;
    if (len){
        memcpy(&send_buf[4], data, len);
    }
    uint16_t crc = crc16_modbus(&send_buf[3], len+1);
    send_buf[len+4] = crc & 0xff;
    send_buf[len+5] = (crc>>8) & 0xff;
    send_buf[len+6] = ETX;
    HAL_UART_Transmit(&huart1, send_buf, len + 7, 100);
    for(int x=0; x<len+7; x++){; 
        DEBUG_PRINTF("%02x ", send_buf[x]);
    }
}

int32_t frameCli(uint8_t data)
{
 static uint8_t cnt = 0;
 static uint32_t len = 0;
 static uint16_t crc = 0;
 int32_t ret = -1;

    switch( cli_state ){
        case WAIT_STX:
            if (data == STX){
                cli_state = WAIT_MAGIC;
            }
            break;
        case WAIT_MAGIC:
            if (data == MAGIC){
                cli_state = WAIT_LEN;
            }else{
                cli_state = WAIT_STX;
            }
            break;
        case WAIT_LEN:
            len = data;
            cli_state = WAIT_PAYLOAD;
            cnt = 0;
            break;
        case WAIT_PAYLOAD:
            cli_buff[cnt++] = data;
            if (cnt == len){
                cli_state = WAIT_CRC1;
            }
            break;
        case WAIT_CRC1:
            crc = data;
            cli_state = WAIT_CRC2;
            break;
        case WAIT_CRC2:
            crc += data<<8;
            if (crc == crc16_modbus(cli_buff, len)){
                cli_state = WAIT_ETX;
            }else{
                cli_state = WAIT_STX;
            }
            break;
        case WAIT_ETX:
            if (data == ETX){
                ret = len;
                cli_state = WAIT_RESTART;
            }
            cli_state = WAIT_STX;
            break;
        default:
            break;
    }

    return ret;
}


int32_t dataCli(uint8_t rxdata)
{
    int decode_result;
    int32_t len = frameCli(rxdata);
    if (0 < len) {
        uint32_t cmd;
        uint8_t *clib = &cli_buff[0];
        if ((decode_result = decode_le128(clib, &cmd)) < 0) {
            DEBUG_PRINTF("cli decode error");
            return 0;
        }
        clib += decode_result;
        switch(cmd){
            case CMD_VERSION:
                DEBUG_PRINTF("cli version");
                sendCli(2, (uint8_t*)"\x8v0.00.00", 10);
                break;
            case CMD_BOOTLOADER:
                DEBUG_PRINTF("cli bootloader");
                sendCli(0, NULL, 0);     // ack
                break;
            case CMD_RESET:
                DEBUG_PRINTF("cli reset");
                sendCli(0, NULL, 0);     // ack
                break;
            case CMD_VTXTEST:
                {
                    uint32_t freq;
                    uint32_t vpd;
                    if ((decode_result = decode_le128(clib, &freq)) < 0) {
                        DEBUG_PRINTF("cli decode error");
                        return 0;
                    }
                    clib += decode_result;                   
                    if ((decode_result = decode_le128(clib, &vpd)) < 0) {
                        DEBUG_PRINTF("cli decode error");
                        return 0;
                    }
                    DEBUG_PRINTF("cli vtx_test freq:%d vpd:%d", freq, vpd);
                    sendCli(0, NULL, 0);     // ack
                }   
                break;
        }
        cli_state = WAIT_STX;
        return len;
    }
    return 0;
}



