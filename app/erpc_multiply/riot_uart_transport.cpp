// riot_uart_transport.cpp — minimal, blocking transport for RIOT
extern "C" {
#include "periph/uart.h"
}
#include "erpc_framed_transport.hpp"
#include <cstdint>
#include <cstdio>

using namespace erpc;

class RiotUartTransport : public FramedTransport {
public:
    RiotUartTransport(uart_t dev, uint32_t baud) : _dev(dev), _baud(baud) {}

    erpc_status_t init() {
        // No RX callback: we do simple blocking reads via getchar() on native
        int rc = uart_init(_dev, _baud, nullptr, nullptr);
        return (rc == 0) ? kErpcStatus_Success : kErpcStatus_Fail;
    }

protected:
    // Match FramedTransport's signature (const pointer!)
    erpc_status_t underlyingSend(const uint8_t *data, uint32_t size) override {
        // RIOT's uart_write returns void → send byte-by-byte
        for (uint32_t i = 0; i < size; ++i) {
            uart_write(_dev, &data[i], 1);
        }
        return kErpcStatus_Success;
    }

    erpc_status_t underlyingReceive(uint8_t *data, uint32_t size) override {
        // On BOARD=native, UART0 maps to stdio → use getchar() blocking
        for (uint32_t i = 0; i < size; ++i) {
            int c;
            do { c = std::getchar(); } while (c == EOF);
            data[i] = static_cast<uint8_t>(c);
        }
        return kErpcStatus_Success;
    }

private:
    uart_t   _dev;
    uint32_t _baud;
};

// C factory used by main.cpp
extern "C" void *riot_uart_transport_create(uart_t dev, uint32_t baud) {
    auto *t = new RiotUartTransport(dev, baud);
    return (t && t->init() == kErpcStatus_Success) ? t : nullptr;
}
