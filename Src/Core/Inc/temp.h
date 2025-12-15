#ifndef __TEMP_H
#define __TEMP_H

typedef enum {
    TEMP_NORMAL=0,
    TEMP_CAUTION,
    TEMP_WARNING,
    TEMP_DANGER
} TEMP_SAFE;

void initTemp(void);
int32_t getTemp(void);
bool procTemp(void);

#endif
