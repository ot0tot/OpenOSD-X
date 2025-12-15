cmake_minimum_required(VERSION 3.22)

project(stm32g4xx)
add_library(stm32g4xx INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(stm32g4xx INTERFACE 
	USE_HAL_DRIVER 
	STM32G431xx
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(stm32g4xx INTERFACE
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Inc
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Inc/Legacy
    ../../Src/Drivers/CMSIS/Device/ST/STM32G4xx/Include
    ../../Src/Drivers/CMSIS/Include
)

target_sources(stm32g4xx INTERFACE
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc_ex.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc_ex.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ex.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ramfunc.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_exti.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma_ex.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr_ex.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_comp.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dac.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dac_ex.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_opamp.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_opamp_ex.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_spi.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_spi_ex.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim_ex.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart.c
    ../../Src/Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart_ex.c
)

target_link_directories(stm32g4xx INTERFACE
)

target_link_libraries(stm32g4xx INTERFACE
)

# Validate that stm32g4xx code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


