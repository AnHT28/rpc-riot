#include "riot_uart_transport.hpp"
#include <cstdio>

RiotUartTransport::RiotUartTransport(uart_t uart_dev, uint32_t baud_rate)
    : m_uart_dev(uart_dev), m_baud_rate(baud_rate)
{
    tsrb_init(&m_rx_buffer, m_rx_buffer_storage, sizeof(m_rx_buffer_storage));
    sema_create(&m_rx_sema, 0);
}

RiotUartTransport::~RiotUartTransport()
{
}

void RiotUartTransport::rx_cb(void *arg, uint8_t data)
{
    RiotUartTransport *transport = static_cast<RiotUartTransport *>(arg);
    // Add byte to ring buffer
    if (tsrb_add_one(&transport->m_rx_buffer, data) == 0) {
        // Signal that data is available
        sema_post(&transport->m_rx_sema);
    } else {
        // Buffer full, drop byte or handle error
        // For now we just drop it, but in a real app we might want to track this
    }
}

erpc_status_t RiotUartTransport::init(void)
{
    // Initialize UART with callback
    if (uart_init(m_uart_dev, m_baud_rate, rx_cb, this) != 0) {
        return kErpcStatus_InitFailed;
    }
    return kErpcStatus_Success;
}

erpc_status_t RiotUartTransport::underlyingReceive(uint8_t *data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++) {
        // Wait for data if buffer is empty
        while (tsrb_empty(&m_rx_buffer)) {
            sema_wait(&m_rx_sema);
        }
        // Read data
        int c = tsrb_get_one(&m_rx_buffer);
        if (c < 0) {
            return kErpcStatus_ReceiveFailed;
        }
        data[i] = (uint8_t)c;
    }
    return kErpcStatus_Success;
}

erpc_status_t RiotUartTransport::underlyingSend(const uint8_t *data, uint32_t size)
{
    uart_write(m_uart_dev, data, size);
    return kErpcStatus_Success;
}

void *erpc_uart_transport_init(uart_t uart_dev, uint32_t baud_rate)
{
    RiotUartTransport *transport = new RiotUartTransport(uart_dev, baud_rate);
    if (transport->init() == kErpcStatus_Success)
    {
        return reinterpret_cast<void *>(transport);
    }
    delete transport;
    return NULL;
}
