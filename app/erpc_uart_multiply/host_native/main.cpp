#include <cstdio>

extern "C" {
#include "erpc_server_setup.h"
#include "erpc_mbf_setup.h"
#include "periph/uart.h"
}

#include "multiply_demo_server.hpp"
#include "multiply_demo_interface.hpp"
#include "c_multiply_demo_server.h"

extern "C" void *riot_uart_transport_create(uart_t dev, uint32_t baud);
erpcShim::MultiplyService_interface *get_multiply_impl(void);

using namespace erpcShim;

int main(void)
{
    std::fprintf(stderr, "eRPC multiply server (native host -> nRF52840DK over UART)\n");

    erpc_transport_t transport = reinterpret_cast<erpc_transport_t>(
        riot_uart_transport_create(UART_DEV(0), 115200));
    if (!transport) {
        std::fprintf(stderr, "[server] ERROR: UART transport init failed\n");
        return 1;
    }

    erpc_mbf_t mbf = erpc_mbf_dynamic_init();
    if (!mbf) {
        std::fprintf(stderr, "[server] ERROR: message buffer init failed\n");
        return 1;
    }

    erpc_server_t server = erpc_server_init(transport, mbf);
    if (!server) {
        std::fprintf(stderr, "[server] ERROR: server init failed\n");
        return 1;
    }

    static MultiplyService_service service(get_multiply_impl());
    erpc_add_service_to_server(server,
        reinterpret_cast<erpc_service_t>(&service));

    std::fprintf(stderr, "[server] ready for RPC calls...\n");

    erpc_status_t status = erpc_server_run(server);
    std::fprintf(stderr, "[server] erpc_server_run returned status %d\n", (int)status);
    return (status == kErpcStatus_Success) ? 0 : 1;
}
