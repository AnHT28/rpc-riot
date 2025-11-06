#include <cstdio>

extern "C" {
#include "xtimer.h"
#include "erpc_client_setup.h"
#include "erpc_mbf_setup.h"
#include "periph/uart.h"
}

extern "C" void *riot_uart_transport_create(uart_t dev, uint32_t baud);
extern "C" void initMultiplyService_client(erpc_client_t client);
extern "C" int32_t multiply_rpc(int32_t a, int32_t b);

static void demo_call(int32_t a, int32_t b)
{
    int32_t r = multiply_rpc(a, b);
    // Remove printf to avoid UART interference with eRPC
    // std::printf("multiply(%ld, %ld) -> %ld\n", (long)a, (long)b, (long)r);
    (void)r; // Suppress unused variable warning
}

int main(void)
{
    // Remove puts to avoid UART interference with eRPC
    // std::puts("eRPC multiply client (nRF52840DK -> native host over UART)");

    erpc_transport_t transport = reinterpret_cast<erpc_transport_t>(
        riot_uart_transport_create(UART_DEV(0), 115200));
    if (!transport) {
        // std::puts("[client] ERROR: UART transport init failed");
        return 1;
    }

    erpc_mbf_t mbf = erpc_mbf_dynamic_init();
    if (!mbf) {
        // std::puts("[client] ERROR: message buffer init failed");
        return 1;
    }

    erpc_client_t client = erpc_client_init(transport, mbf);
    if (!client) {
        // std::puts("[client] ERROR: client init failed");
        return 1;
    }

    initMultiplyService_client(client);

    xtimer_sleep(2);
    demo_call(6, 7);

    while (true) {
        xtimer_sleep(5);
        demo_call(3, 9);
    }

    return 0;
}
