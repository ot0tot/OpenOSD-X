#ifndef __MSP_H
#define __MSP_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MSP_V1,
    MSP_V2
} msp_protocol_version_t;

#define MSP_VTX_CONFIG 88           // Common value, verify
#define MSP_SET_VTX_CONFIG 89       // Common value, verify
#define MSP_VTXTABLE_BAND        137    //out message         vtxTable band/channel data
#define MSP_SET_VTXTABLE_BAND    227    //in message          set vtxTable band/channel data (one band at a time)
#define MSP_VTXTABLE_POWERLEVEL  138    //out message         vtxTable powerLevel data
#define MSP_SET_VTXTABLE_POWERLEVEL 228 //in message          set vtxTable powerLevel data (one powerLevel at a time)
#define MSP_EEPROM_WRITE 250        // Common value, verify
#define MSP_REBOOT 68               // Common value, verify
#define MSP_DISPLAYPORT                 182
#define MSP_SET_OSD_CANVAS 188      // Set osd canvas size COLSxROWS
#define MSP_OSD_CANVAS 189          // Get osd canvas size COLSxROWS

void msp_init(void);
bool msp_parse_char(uint8_t c);
void msp_send_reply(uint16_t command, const uint8_t *payload, uint8_t payload_size, msp_protocol_version_t version);

#endif
