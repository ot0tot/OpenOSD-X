

#include "main.h"
#include "log.h"
#include "flash.h"


bool flash_erase(uint32_t addr, uint16_t size)
{
    DEBUG_PRINTF("erase start");

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    static FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = GET_FLASH_PAGE(addr);
    EraseInitStruct.NbPages     = (sizeof(size) / FLASH_PAGE_SIZE) + 1;
    uint32_t PageError = 0;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK){
        ;
    }
    HAL_FLASH_Lock();

    DEBUG_PRINTF("erase end");

    return true;
}


bool flash_write(uint32_t addr, uint8_t *data, uint16_t size)
{
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    // write
    for(int x=0; x<size; x+=8){
        uint64_t wdata = *data++;
        wdata += (uint64_t)(*data++)<<8;
        wdata += (uint64_t)(*data++)<<16;
        wdata += (uint64_t)(*data++)<<24;
        wdata += (uint64_t)(*data++)<<32;
        wdata += (uint64_t)(*data++)<<40;
        wdata += (uint64_t)(*data++)<<48;
        wdata += (uint64_t)(*data++)<<56;

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, addr, wdata) == HAL_OK){
            ;
        }
        addr += 8;
    }
    HAL_FLASH_Lock();

    return true;
}


