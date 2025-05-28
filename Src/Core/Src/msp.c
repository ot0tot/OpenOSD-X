#include <stdbool.h>
#include <stdio.h>
#include "main.h"
#include "char_canvas.h"


typedef enum {
    WAIT_HEAD1 = 0,
    WAIT_HEAD2,
    WAIT_HEAD3,
    WAIT_LEN,
    WAIT_PAYLOAD,
    WAIT_CRC
} RXSTATE;

typedef enum {
    MSP_DP_HEARTBEAT = 0,         // Release the display after clearing and updating
    MSP_DP_RELEASE = 1,         // Release the display after clearing and updating
    MSP_DP_CLEAR_SCREEN = 2,    // Clear the display
    MSP_DP_WRITE_STRING = 3,    // Write a string at given coordinates
    MSP_DP_DRAW_SCREEN = 4,     // Trigger a screen draw
    MSP_DP_OPTIONS = 5,         // Not used by Betaflight. Reserved by Ardupilot and INAV
    MSP_DP_SYS = 6,             // Display system element displayportSystemElement_e at given coordinates
    MSP_DP_COUNT,
} displayportMspCommand_e;

static uint8_t msp_rxdata[256];

void initMsp(void)
{
}


uint8_t* getMspdata(void)
{
    return msp_rxdata;
}

void txMsp(uint8_t cmd, uint8_t *payload, uint8_t payloadlen)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)"$M<", 3, 100);
    HAL_UART_Transmit(&huart1, &payloadlen, 1, 100);
    HAL_UART_Transmit(&huart1, &cmd, 1, 100);
    HAL_UART_Transmit(&huart1, payload, payloadlen, 100);
    uint8_t sum = payloadlen ^ cmd;
    for(int x=0; x<payloadlen; x++){
        sum ^= payload[x];
    }
    HAL_UART_Transmit(&huart1, &sum, 1, 100);
}

uint16_t rxFrameMsp(uint8_t rxdata)
{
    static uint16_t count = 0;
    static uint16_t len = 0;
    static uint8_t sum = 0;
    static RXSTATE msp_state = WAIT_HEAD1;

    int ret = 0;

    switch(msp_state){
        case WAIT_HEAD1:
            if(rxdata == '$'){
                msp_state = WAIT_HEAD2;
            }
            break;
        case WAIT_HEAD2:
            if(rxdata == 'M'){
                msp_state = WAIT_HEAD3;
            }else{
                msp_state = WAIT_HEAD1;
            }
            break;
        case WAIT_HEAD3:
            if(rxdata == '>'){
                sum = 0;
                msp_state = WAIT_LEN;
            }else{
                msp_state = WAIT_HEAD1;
            }
            break;
        case WAIT_LEN:
            len = rxdata;
            count = 0;
            sum ^= rxdata;
            msp_state = WAIT_PAYLOAD;
            break;
        case WAIT_PAYLOAD:
            msp_rxdata[count] = rxdata;
            sum ^= rxdata;
            if (count == len){
                msp_state = WAIT_CRC;
            }
            if (++count > sizeof(msp_rxdata)){
                msp_state = WAIT_HEAD1;
            }
            break;
        case WAIT_CRC:
            if(sum == rxdata){
                ret = len;
            }
            msp_state = WAIT_HEAD1;
            break;
    }
    return ret;
}

int32_t rxMsp(uint8_t data)
{
    static bool msp_first = true;
    uint32_t detect_cmd = -1;
    
    uint16_t len = rxFrameMsp(data);
    if ( len ){
#if 1
        printf("rx(%02d) %02x-%02x:",
            len,
            msp_rxdata[0],
            msp_rxdata[1]);
        for (int x=2; x<len+1; x++){
            printf(" %02x", msp_rxdata[x]);
        }
        printf("\r\n");
#endif
        if (msp_rxdata[0] == 0xb6){
            detect_cmd = msp_rxdata[1];
            switch(msp_rxdata[1]){
                case MSP_DP_HEARTBEAT:
                    if (msp_first){
//                        msp_first = false;
                        uint8_t data[2] = {COLUMN_SIZE,ROW_SIZE};
                        txMsp(0xbc, data, 2);
                    }
                    break;
                case MSP_DP_CLEAR_SCREEN:
                    charCanvasClear();
                    break;
                case MSP_DP_WRITE_STRING:
                    charCanvasWrite(msp_rxdata[2], msp_rxdata[3], &msp_rxdata[5], len-4);
                    break;
                case MSP_DP_DRAW_SCREEN:
                    charCanvasDraw();
                    break;
            }
        }
    }
    return detect_cmd;
}

