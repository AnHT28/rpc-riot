#include <cstdio>
#include "riot_uart_transport.hpp"
#include "calculator_server.hpp"
#include "calculator_interface.hpp"

extern "C" {
#include "erpc_server_setup.h"
#include "erpc_mbf_setup.h"
#include "periph/uart.h"
}

using namespace erpcShim;

// Implementation of the Calculator interface
class Calculator_impl : public Calculator_interface {
public:
    int32_t add(int32_t a, int32_t b) override {
        std::printf("Server: add(%ld, %ld)\n", (long)a, (long)b);
        return a + b;
    }
    int32_t subtract(int32_t a, int32_t b) override {
        std::printf("Server: subtract(%ld, %ld)\n", (long)a, (long)b);
        return a - b;
    }
    int32_t multiply(int32_t a, int32_t b) override {
        std::printf("Server: multiply(%ld, %ld)\n", (long)a, (long)b);
        return a * b;
    }
    float divide(int32_t a, int32_t b) override {
        std::printf("Server: divide(%ld, %ld)\n", (long)a, (long)b);
        return b != 0 ? (float)a / b : 0.0f;
    }
};

int main(void)
{
    std::printf("eRPC NRF Server starting...\n");

    // Initialize UART transport
    // UART_DEV(1) is mapped to D0/D1 on nRF52840DK
    erpc_transport_t transport = reinterpret_cast<erpc_transport_t>(erpc_uart_transport_init(UART_DEV(1), 115200));
    if (!transport) {
        std::printf("Failed to create UART transport\n");
        return 1;
    }

    // Create message buffer factory
    erpc_mbf_t mbf = erpc_mbf_dynamic_init();
    if (!mbf) {
        std::printf("Failed to create message buffer factory\n");
        return 1;
    }

    // Initialize server
    erpc_server_t server = erpc_server_init(transport, mbf);

    // Add service
    Calculator_service *service = new Calculator_service(new Calculator_impl());
    erpc_add_service_to_server(server, service);

    std::printf("Server initialized. Running...\n");

    // Run server
    erpc_server_run(server);

    return 0;
}
