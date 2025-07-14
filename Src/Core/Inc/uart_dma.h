#ifndef UART_DMA_H
#define UART_DMA_H

#include "stm32g4xx.h"
#include <stdint.h>
#include <stddef.h>

// ===== Configuration =====
#define UART_TX_BUFFER_SIZE 256
#define UART_RX_BUFFER_SIZE 256
// =========================

/**
 * @brief Initializes UART and DMA for ring buffer mode operation.
 * @param baudrate The desired baud rate (e.g., 115200).
 * @param rto_timeout_ms The receiver timeout in milliseconds. This value is used to
 * calculate the RTO register value based on the bit time.
 */
void uart_init(void);

/**
 * @brief Adds data to the transmit ring buffer (non-blocking).
 * @param data Pointer to the data to be sent.
 * @param len Length of the data to be sent.
 * @return uint16_t The number of bytes actually added to the buffer.
 */
uint16_t uart_send(const uint8_t* data, uint16_t len);

/**
 * @brief Reads one byte from the receive ring buffer (non-blocking).
 * @param data Pointer to a variable to store the read byte.
 * @return int8_t 0 on success, -1 if no data is available.
 */
int8_t uart_read_byte(uint8_t* data);

/**
 * @brief Polling function to process outgoing data.
 * Must be called periodically in the main loop.
 */
void uart_poll(void);

#endif // UART_DMA_H



