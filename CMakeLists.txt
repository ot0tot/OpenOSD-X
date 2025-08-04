cmake_minimum_required(VERSION 3.22)

set(CMAKE_PROJECT_NAME openosd-x)

option(USE_RTT "Enable SEGGER RTT support" OFF)

# Add options to select resolution mode
option(RESOLUTION_HD "Build with HD resolution" OFF)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, defaulting to Release")
    # Default to Release if no build type is specified
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type")
endif()

# Set compile definition and suffix only if RESOLUTION_HD is ON
if(RESOLUTION_HD)
    message(STATUS "Resolution HD")
    add_compile_definitions(RESOLUTION_HD)
    set(HEX_SUFFIX "hd")
else()
    # No resolution macro defined
    message(STATUS "Resolution SD")
    set(HEX_SUFFIX "sd")
endif()

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")
if(USE_RTT)
    message(STATUS "RTT enabled")
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/rtt.cmake)
    add_definitions(-DUSE_RTT=1)
else()
    message(STATUS "RTT disabled")
    set(RTT_SOURCES "")
    set(RTT_INCLUDE_DIR "")
    add_definitions(-DUSE_RTT=0)
endif()
if(DEV_MODE)
    message(STATUS "DEV_MODE enabled")
	set(CLI_SOURCES ${CMAKE_CURRENT_LIST_DIR}/Src/Core/Src/cli.c)
    add_definitions(-DDEV_MODE=1)
else()
    set(CLI_SOURCES "")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add HAL stm32g4xx sources
add_subdirectory(cmake/stm32g4xx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add sources here
    ./Src/Core/Src/main.c
    ./Src/Core/Src/stm32g4xx_it.c
    ./Src/Core/Src/stm32g4xx_hal_msp.c
    ./Src/Core/Src/system_stm32g4xx.c
    ./Src/Core/Src/sysmem.c
    ./Src/Core/Src/syscalls.c
    ./Src/Core/Src/led.c
    ./Src/Core/Src/char_canvas.c
    ./Src/Core/Src/msp.c
    ./Src/Core/Src/sys_timer.c
    ./Src/Core/Src/cli.c
    ./Src/Core/Src/rtc6705.c
    ./Src/Core/Src/vtx.c
    ./Src/Core/Src/uart_dma.c
    ./Src/Core/Src/mspvtx.c
    ./Src/Core/Src/flash.c
    ./Src/Core/Src/videosignal_gen.c
    ./Src/Core/Src/setting.c
    ./Src/Core/Startup/startup_stm32g431kbtx.s
    ${RTT_SOURCES}
    ${CLI_SOURCES}
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add defined include paths
    ./Src/Core/Inc
    ${RTT_INCLUDE_DIR}
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32g4xx
    # Add defined libraries
)

# Define output file names
set(BIN_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_${HEX_SUFFIX}.bin")
set(HEX_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_${HEX_SUFFIX}.hex")
set(HEXTM1_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_${HEX_SUFFIX}_temp1.hex")
set(HEXTM2_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_${HEX_SUFFIX}_temp2.hex")
set(HEXHEADER_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_${HEX_SUFFIX}_header.hex")


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> ${BIN_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Oihex --gap-fill=0xFF $<TARGET_FILE:${PROJECT_NAME}> ${HEXTM1_FILE}
        # Calculate CRC and create header with size(91K=0x16E00)
        COMMAND srec_cat ${HEXTM1_FILE} -Intel -crop 0x08004200 0x0801B000 -STM32 0x08004004 -generate 0x08004000 0x08004004 -constant-l-e 0x16E00 4 -output ${HEXTM2_FILE} -Intel
        COMMAND srec_cat ${HEXTM2_FILE} -Intel -crop 0x08004000 0x08004008 -output ${HEXHEADER_FILE} -Intel
        # Merge with original file
        COMMAND srec_cat ${HEXHEADER_FILE} -Intel ${HEXTM1_FILE} -Intel -output ${HEX_FILE} -Intel
        # Delete temp files
	    COMMAND ${CMAKE_COMMAND} -E rm -f ${HEXTM1_FILE} ${HEXTM2_FILE} ${HEXHEADER_FILE}
		)




