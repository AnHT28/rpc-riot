#include "riot_uart_transport.hpp"
#include "erpc_message_buffer.hpp"
#include <cstdio>

RiotUartTransport::RiotUartTransport(uart_t uart_dev)
    : FramedTransport(), m_uart_dev(uart_dev)
{
}

RiotUartTransport::~RiotUartTransport()
{
}

erpc_status_t RiotUartTransport::init(void)
{
    return kErpcStatus_Success;
}

erpc_status_t RiotUartTransport::underlyingReceive(uint8_t *data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++) {
        int c = getchar();
        if (c == EOF) {
            return kErpcStatus_ReceiveFailed;
        }
        data[i] = (uint8_t)c;
    }
    return kErpcStatus_Success;
}

erpc_status_t RiotUartTransport::underlyingSend(const uint8_t *data, uint32_t size)
{
    size_t written = fwrite(data, 1, size, stdout);
    fflush(stdout);
    return (written == size) ? kErpcStatus_Success : kErpcStatus_SendFailed;
}

////////////////////////////////////////////////////////////////////////////////
// External C Interface
////////////////////////////////////////////////////////////////////////////////
void *erpc_uart_transport_init(uart_t uart_dev)
{
    RiotUartTransport *transport = new RiotUartTransport(uart_dev);
    if (transport->init() == kErpcStatus_Success)
    {
        return reinterpret_cast<void *>(transport);
    }
    delete transport;
    return NULL;
}