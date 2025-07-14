#include "main.h"
#include "uart_dma.h"
#include <string.h> // for memcpy

// Peripheral definitions (for STM32G474)
#define UART_INSTANCE       USART1
#define TX_DMA_CHANNEL      DMA2_Channel4
#define RX_DMA_CHANNEL      DMA2_Channel3
#define TX_DMA_REQUEST      DMA_REQUEST_USART1_TX
#define RX_DMA_REQUEST      DMA_REQUEST_USART1_RX
#define TX_DMA_TC_FLAG      DMA_ISR_TCIF1
#define TX_DMA_CLEAR_FLAGS  (DMA_IFCR_CTCIF1 | DMA_IFCR_CGIF1)

// Ring buffers and management pointers
static uint8_t tx_buffer[UART_TX_BUFFER_SIZE] __attribute__((aligned(4)));
static volatile uint16_t tx_head = 0;
static volatile uint16_t tx_tail = 0;

static uint8_t rx_buffer[UART_RX_BUFFER_SIZE] __attribute__((aligned(4)));
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;

// DMA state management
static volatile uint8_t is_tx_dma_running = 0;
static volatile uint16_t dma_tx_len = 0;
static volatile uint16_t last_rx_dma_pos = 0;

void uart_tx_poll(void)
{
    // Check for transfer completion if a transfer is running
    if (is_tx_dma_running) {
//        if ((DMA1->ISR & TX_DMA_TC_FLAG) != 0) {
        if ((UART_INSTANCE->ISR & UART_FLAG_TC) != 0) {
//            DMA1->IFCR = TX_DMA_CLEAR_FLAGS; // Clear transfer complete flags
            TX_DMA_CHANNEL->CCR &= ~DMA_CCR_EN; // Disable the DMA channel
            
            // Update the tail pointer by the amount of data just sent
            tx_tail = (tx_tail + dma_tx_len) % UART_TX_BUFFER_SIZE;
            dma_tx_len = 0;
            is_tx_dma_running = 0;
        } else {
            // Transfer is still in progress
            return;
        }
    }
    
    // If not running, check if there's new data to send
    if (tx_head != tx_tail) {
        // Calculate the largest contiguous block to send from the tail
        uint16_t current_tx_tail = tx_tail;
        if (tx_head > current_tx_tail) {
            // Data is in a single block
            dma_tx_len = tx_head - current_tx_tail;
        } else {
            // Data wraps around, send the block from tail to the end of the buffer
            dma_tx_len = UART_TX_BUFFER_SIZE - current_tx_tail;
        }

        if (dma_tx_len > 0) {
            // Configure DMA for the next transfer
            TX_DMA_CHANNEL->CMAR = (uint32_t)&tx_buffer[current_tx_tail];
            TX_DMA_CHANNEL->CNDTR = dma_tx_len;
            
            UART_INSTANCE->ICR = UART_CLEAR_TCF;
            
            is_tx_dma_running = 1;
            TX_DMA_CHANNEL->CCR |= DMA_CCR_EN; // Enable DMA to start transfer
        }
    }
}

void uart_init(void)
{

    LL_USART_Disable(USART1);
    LL_USART_EnableDMAReq_RX(USART1);
    LL_USART_EnableDMAReq_TX(USART1);
    LL_USART_EnableDirectionTx(USART1);
    LL_USART_EnableDirectionRx(USART1);
    LL_USART_EnableFIFO(USART1);
    LL_USART_Enable(USART1);

    // --- DMA Configuration ---
    // Receive DMA (Channel2): Circular Mode
    DMAMUX1_Channel8->CCR = RX_DMA_REQUEST;
    RX_DMA_CHANNEL->CCR = 0;
    RX_DMA_CHANNEL->CPAR = (uint32_t)&(UART_INSTANCE->RDR);
    RX_DMA_CHANNEL->CMAR = (uint32_t)rx_buffer;
    RX_DMA_CHANNEL->CNDTR = UART_RX_BUFFER_SIZE;
    RX_DMA_CHANNEL->CCR |= DMA_CCR_MINC | DMA_CCR_CIRC; // Memory increment, Circular mode
    RX_DMA_CHANNEL->CCR |= DMA_CCR_EN; // Start reception

    // Transmit DMA (Channel1): Normal Mode
    DMAMUX1_Channel9->CCR = TX_DMA_REQUEST;
    TX_DMA_CHANNEL->CCR = 0; // Ensure channel is disabled
    TX_DMA_CHANNEL->CPAR = (uint32_t)&(UART_INSTANCE->TDR);
    // CMAR and CNDTR will be set before each transfer
    TX_DMA_CHANNEL->CCR |= DMA_CCR_DIR      | // Read from memory (to peripheral)
                           DMA_CCR_MINC;      // Memory address increment
}

uint16_t uart_send(const uint8_t* data, uint16_t len) 
{
    uint16_t current_tx_head = tx_head;
    uint16_t current_tx_tail = tx_tail;
    uint16_t free_space;

    // (Data copy to ring buffer logic is the same)
    if (current_tx_head >= current_tx_tail) {
        free_space = UART_TX_BUFFER_SIZE - (current_tx_head - current_tx_tail) - 1;
    } else {
        free_space = current_tx_tail - current_tx_head - 1;
    }
    if (len > free_space) { len = free_space; }
    if (len == 0) { return 0; }
    if (current_tx_head + len > UART_TX_BUFFER_SIZE) {
        uint16_t part1_len = UART_TX_BUFFER_SIZE - current_tx_head;
        memcpy(&tx_buffer[current_tx_head], data, part1_len);
        uint16_t part2_len = len - part1_len;
        memcpy(&tx_buffer[0], data + part1_len, part2_len);
    } else {
        memcpy(&tx_buffer[current_tx_head], data, len);
    }
    
    // Update the head pointer atomically
    tx_head = (current_tx_head + len) % UART_TX_BUFFER_SIZE;

    // Try to start a transmission if not already running
    uart_tx_poll();

    return len;
}

void uart_poll(void)
{
    uart_tx_poll();
}


int8_t uart_read_byte(uint8_t* data) 
{
    rx_head = UART_RX_BUFFER_SIZE - LL_DMA_GetDataLength(DMA2, LL_DMA_CHANNEL_3);


    if (rx_head == rx_tail) {
        return -1; // No data
    }

    *data = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % UART_RX_BUFFER_SIZE;

    return 0;
}

