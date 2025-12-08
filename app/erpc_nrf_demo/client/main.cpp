#include <stdio.h>
#include <stdlib.h>
#include "xtimer.h"
#include "c_calculator_client.h"
#include "riot_uart_transport.hpp"
#include "erpc_client_setup.h"
#include "erpc_mbf_setup.h"
#include "periph/uart.h"

int main(void)
{
    // Small delay to allow terminal to connect
    xtimer_sleep(1);
    
    printf("eRPC NRF Client starting...\n");

    // Initialize UART transport
    // UART_DEV(1) is mapped to D0/D1 on nRF52840DK
    erpc_transport_t transport = (erpc_transport_t)erpc_uart_transport_init(UART_DEV(1), 115200);
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
        printf("Failed to initialize eRPC client");
        return 1;
    }

    // Initialize generated C client wrapper
    initCalculator_client(client);

    
    /*
    char line_buf[64];
    
    while (1) {
        printf("\n--- eRPC Calculator Client ---\n");
        
        printf("Enter value A: ");
        if (fgets(line_buf, sizeof(line_buf), stdin) == NULL) {
            // If we get EOF/Error, it might be because stdin is not ready or disconnected.
            // Wait a bit and retry.
            clearerr(stdin); 
            xtimer_sleep(2);
            continue;
        }
        // Simple validation: check if line is just newline
        if (line_buf[0] == '\n') {
            continue;
        }
        int32_t a = (int32_t)strtol(line_buf, NULL, 10);

        printf("Enter value B: ");
        if (fgets(line_buf, sizeof(line_buf), stdin) == NULL) {
            clearerr(stdin);
            xtimer_sleep(2);
            continue;
        }
        if (line_buf[0] == '\n') {
            continue;
        }
        int32_t b = (int32_t)strtol(line_buf, NULL, 10);

        printf("Calling add(%ld, %ld)...\n", (long)a, (long)b);
        int32_t result_add = add(a, b);
        printf("Result: %ld\n", (long)result_add);

        printf("Calling multiply(%ld, %ld)...\n", (long)a, (long)b);
        int32_t result_mul = multiply(a, b);
        printf("Result: %ld\n", (long)result_mul);
    }
    */

    int32_t a, b;

    printf("Enter value A: \n");
    if (scanf("%ld", &a) != 1) {
        printf("Invalid input for A\n");
        return 1;
    }

    printf("Enter value B: \n");
    if (scanf("%ld", &b) != 1) {
        printf("Invalid input for B\n");
        return 1;
    }

    printf("Testing remote calculations with A=%ld, B=%ld\n", (long)a, (long)b);

    int32_t sum = add(a, b);
    printf("Client add result: %ld\n", sum);

    int32_t diff = subtract(a, b);
    printf("Client subtract result: %ld\n", diff);

    int32_t product = multiply(a, b);
    printf("Client multiply result: %ld\n", product);

    float quotient = divide(a, b);
    printf("Client divide result: %f\n", quotient);

    return 0;
}