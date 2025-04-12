TARGET = openosd-x

# debug build?
DEBUG ?= 1
# optimization
OPT = -Og

# Build path
BUILD_DIR = build

# C sources
C_SOURCES_HAL_G4xx =  \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc_ex.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_ll_adc.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc_ex.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ex.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ramfunc.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_exti.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma_ex.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr_ex.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_comp.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dac.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dac_ex.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_opamp.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_opamp_ex.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_spi.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_spi_ex.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim_ex.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart.c \
Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart_ex.c \

C_SOURCES_Src =  \
Src/Core/Src/main.c \
Src/Core/Src/stm32g4xx_it.c \
Src/Core/Src/stm32g4xx_hal_msp.c \
Src/Core/Src/system_stm32g4xx.c \
Src/Core/Src/sysmem.c \
Src/Core/Src/syscalls.c


C_SOURCES = ${C_SOURCES_HAL_G4xx} ${C_SOURCES_Src}

# ASM sources
ASM_SOURCES =  \
Src/Core/Startup/startup_stm32g431kbtx.s


PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32G431xx


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-ISrc/Core/Inc \
-ISrc/Drivers/STM32G4xx_HAL_Driver/Inc \
-ISrc/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy \
-ISrc/Drivers/CMSIS/Device/ST/STM32G4xx/Include \
-ISrc/Drivers/CMSIS/Include


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = Src/stm32g431kbtx_flash.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@
$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
-include $(wildcard $(BUILD_DIR)/*.d)
