#include <cstdio>
extern "C" {
#include "thread.h"
#include "erpc_server_setup.h"
#include "erpc_mbf_setup.h"
#include "periph/uart.h"
}

/* Generated C++ service wrapper */
#include "calculator_server.hpp"
#include "calculator_interface.hpp"

/* Our UART transport factory (returns void* like the examples' loopback) */
extern "C" void *erpc_uart_transport_init(uart_t uart_dev);

using namespace erpcShim;

// Simple implementation of the Calculator interface (server-side)
class Calculator_impl : public Calculator_interface {
public:
    int32_t add(int32_t a, int32_t b) override {
        std::printf("Server: add(%d, %d)\n", a, b);
        return a + b;
    }
    int32_t subtract(int32_t a, int32_t b) override {
        std::printf("Server: subtract(%d, %d)\n", a, b);
        return a - b;
    }
    int32_t multiply(int32_t a, int32_t b) override {
        std::printf("Server: multiply(%d, %d)\n", a, b);
        return a * b;
    }
    float divide(int32_t a, int32_t b) override {
        std::printf("Server: divide(%d, %d)\n", a, b);
        return b != 0 ? (float)a / b : 0.0f;
    }
};

int main(void)
{
    std::puts("eRPC Calculator server (native)");

    /* create TCP transport for host-to-host RPC */
    erpc_transport_t transport = erpc_transport_tcp_init("0.0.0.0", 50051, true);
    if (!transport) {
        std::puts("[server] ERROR: TCP transport create failed");
        return 1;
    }

    /* init MBF */
    erpc_mbf_t mbf = erpc_mbf_dynamic_init();
    if (!mbf) {
        std::puts("[server] ERROR: mbf init failed");
        return 1;
    }

    /* init server (cast transport to erpc_transport_t as examples do) */
    erpc_server_t srv = erpc_server_init((erpc_transport_t)transport, mbf);
    if (!srv) {
        std::puts("[server] ERROR: server init failed");
        return 1;
    }

    /* create our implementation and a generated service wrapper */
    static Calculator_impl impl;
    static Calculator_service service(&impl);

    /* register service */
    /* erpc_add_service_to_server expects a void* for the service handle */
    erpc_add_service_to_server(srv, reinterpret_cast<void *>(&service));

    std::puts("[server] running...");

    /* run server loop */
    while (1) {
        erpc_status_t st = erpc_server_run(srv);
        if (st != kErpcStatus_Success) {
            std::puts("[server] server returned non-success, yielding...");
            thread_yield();
        }
    }

    return 0;
}
