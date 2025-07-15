#ifndef __MSPVTX_H
#define __MSPVTX_H

uint8_t getBandLetterByIdx(uint8_t idx);
void mspvtx_VtxConfig(uint8_t *packet);
void mspvtx_VtxTablePowerLevel(uint8_t *packet);
void mspvtx_VtxTableBand(uint8_t *packet);
void mspvtx_Ack(uint16_t function);
void mspUpdate(void);
void mspVtx_init(void);

#endif

