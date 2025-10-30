// Simple C-style eRPC client using generated C client wrappers.
#include <stdio.h>
#include "c_calculator_client.h"
#include "riot_uart_transport.hpp"
#include "erpc_client_setup.h"
#include "erpc_mbf_setup.h"
#include "periph/uart.h"

int main(void)
{
    // Initialize UART (native build uses stdio-based transport implementation)
    uart_init(UART_DEV(0), 115200, NULL, NULL);

    // Create transport (C factory provided by riot_uart_transport.hpp)
    erpc_transport_t transport = (erpc_transport_t)erpc_uart_transport_init(UART_DEV(0));
    if (!transport) {
        printf("Failed to create UART transport\n");
        return 1;
    }

    // Create message buffer factory
    erpc_mbf_t mbf = erpc_mbf_dynamic_init();
    if (!mbf) {
        printf("Failed to create message buffer factory\n");
        return 1;
    }

    // Initialize client
    erpc_client_t client = erpc_client_init(transport, mbf);
    if (!client) {
        printf("Failed to initialize eRPC client\n");
        erpc_mbf_dynamic_deinit(mbf);
        return 1;
    }

    // Initialize generated C client wrapper
    initCalculator_client(client);

    printf("eRPC Calculator Client starting...\n");

    int32_t a = 42;
    int32_t b = 7;

    printf("Testing remote calculations with a=%d, b=%d\n", a, b);

    // Use generated C client functions. They return results directly.
    int32_t sum = add(a, b);
    printf("Remote add result: %d\n", sum);

    int32_t diff = subtract(a, b);
    printf("Remote subtract result: %d\n", diff);

    int32_t product = multiply(a, b);
    printf("Remote multiply result: %d\n", product);

    float quotient = divide(a, b);
    printf("Remote divide result: %f\n", quotient);

    // Cleanup
    deinitCalculator_client();
    erpc_client_deinit(client);
    erpc_mbf_dynamic_deinit(mbf);

    return 0;
}