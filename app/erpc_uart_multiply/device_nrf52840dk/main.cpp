#include <cstdio>
#include <cstdlib>

extern "C" {
#include "xtimer.h"
#include "erpc_client_setup.h"
#include "erpc_mbf_setup.h"
#include "periph/uart.h"
}

extern "C" void *riot_uart_transport_create(uart_t dev, uint32_t baud);
extern "C" void initMultiplyService_client(erpc_client_t client);
extern "C" int32_t multiply_rpc(int32_t a, int32_t b);

static void interactive_multiply() {
    int32_t a = 5, b = 7;  // Use hardcoded values for now to test eRPC
    
    std::printf("\nTesting eRPC with hardcoded values: a=5, b=7\n");
    std::printf("About to call eRPC multiply function...\n");
    std::fflush(stdout);
    
    std::printf("Calling eRPC: multiply(%ld, %ld)...\n", (long)a, (long)b);
    std::fflush(stdout);
    
    int32_t result = multiply_rpc(a, b);
    
    std::printf("eRPC call returned successfully!\n");
    std::printf("Result: %ld * %ld = %ld\n", (long)a, (long)b, (long)result);
    std::printf("eRPC call completed successfully!\n");
    std::fflush(stdout);
}

int main(void)
{
    std::puts("eRPC multiply client (nRF52840DK -> native host over UART)");
    std::puts("Using UART_DEV(1) for eRPC, UART_DEV(0) for console");

    // Use UART_DEV(1) for eRPC (Arduino D0/D1 pins -> /dev/ttyACM1)
    std::printf("Creating UART transport on UART_DEV(1)...\n");
    erpc_transport_t transport = reinterpret_cast<erpc_transport_t>(
        riot_uart_transport_create(UART_DEV(1), 115200));
    if (!transport) {
        std::puts("[client] ERROR: UART transport init failed");
        return 1;
    }
    std::printf("UART transport created successfully!\n");

    erpc_mbf_t mbf = erpc_mbf_dynamic_init();
    if (!mbf) {
        std::puts("[client] ERROR: message buffer init failed");
        return 1;
    }

    erpc_client_t client = erpc_client_init(transport, mbf);
    if (!client) {
        std::puts("[client] ERROR: client init failed");
        return 1;
    }

    initMultiplyService_client(client);

    std::puts("eRPC client initialized successfully!");
    std::puts("Running single eRPC test...\n");

    // Small delay to let UART settle
    xtimer_sleep(1);

    // Run one test and exit
    interactive_multiply();
    
    std::puts("Test completed. Application will halt.");
    while (true) {
        xtimer_sleep(10);
    }

    return 0;
}
