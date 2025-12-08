#pragma once

#include "erpc_framed_transport.hpp"
#include "periph/uart.h"
#include "sema.h"
#include "tsrb.h"

class RiotUartTransport : public erpc::FramedTransport
{
public:
    RiotUartTransport(uart_t uart_dev, uint32_t baud_rate);
    virtual ~RiotUartTransport();

    virtual erpc_status_t init(void);

protected:
    virtual erpc_status_t underlyingReceive(uint8_t *data, uint32_t size) override;
    virtual erpc_status_t underlyingSend(const uint8_t *data, uint32_t size) override;

private:
    static void rx_cb(void *arg, uint8_t data);

    uart_t m_uart_dev;
    uint32_t m_baud_rate;
    tsrb_t m_rx_buffer;
    uint8_t m_rx_buffer_storage[1024]; 
    sema_t m_rx_sema;
};

extern "C" {
    void *erpc_uart_transport_init(uart_t uart_dev, uint32_t baud_rate);
}
