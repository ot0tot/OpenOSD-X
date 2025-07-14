#ifndef __FLASH_H
#define __FLASH_H

#define GET_FLASH_PAGE(addr)  ((addr - FLASH_BASE) / FLASH_PAGE_SIZE)

bool flash_erase(uint32_t addr, uint16_t size);
bool flash_write(uint32_t addr, uint8_t *data, uint16_t size);

#endif
