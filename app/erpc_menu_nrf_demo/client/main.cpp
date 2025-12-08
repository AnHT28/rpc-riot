/*
 * Client-side menu application for Enhanced RPC Demo (UART, nRF52840-DK)
 */

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "riot_uart_transport.hpp"
extern "C" {
#include "xtimer.h"
#include "erpc_client_setup.h"
#include "erpc_mbf_setup.h"
#include "periph/uart.h"
}

// Local eRPC allocation hooks (C-linkage for eRPC C APIs)
extern "C" {
void *erpc_malloc(size_t size)
{
    return malloc(size);
}

void erpc_free(void *ptr)
{
    free(ptr);
}
}

/* Generated C client wrappers */
#include "c_demo_service_client.h"
#include "demo_service_common.h"

// Helper function to clear input buffer
static void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Helper function to display main menu
static void display_main_menu(void) {
    printf("\n");
    printf("====================================\n");
    printf("  Lightweight RPC Demo (UART link)\n");
    printf("====================================\n");
    printf("Select function:\n\n");
    printf("  1) Integer calculation\n");
    printf("  2) String processing\n");
    printf("  3) Array sorting\n");
    printf("  4) Exit\n\n");
    printf("Enter your choice: \n");
}

// Option 1: Integer calculation
static void option_integer_calculation(void) {
    int32_t a, b;
    int op_choice;
    
    printf("\n--- Integer Calculation ---\n");
    
    // Get first number
    printf("Enter first integer (a): \n");
    if (scanf("%ld", &a) != 1) {
        printf("Invalid input!\n");
        clear_input_buffer();
        return;
    }
    
    // Get second number
    printf("Enter second integer (b): \n");
    if (scanf("%ld", &b) != 1) {
        printf("Invalid input!\n");
        clear_input_buffer();
        return;
    }
    
    // Get operation
    printf("\nSelect operation:\n");
    printf("  0) Add\n");
    printf("  1) Subtract\n");
    printf("  2) Multiply\n");
    printf("  3) Divide\n");
    printf("Choice: \n");
    if (scanf("%d", &op_choice) != 1 || op_choice < 0 || op_choice > 3) {
        printf("Invalid operation!\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();
    
    // Call RPC
    printf("\n[Client] Calling remote calculate(%ld, %ld, %d)...\n", (long)a, (long)b, op_choice);
    CalcResult *result = calculate(a, b, (ArithmeticOp)op_choice);
    
    // Display result
    if (result && result->status == 0) {
        const char *op_symbols[] = {"+", "-", "*", "/"};
        printf("\n==> Result: %ld %s %ld = %ld\n", (long)a, op_symbols[op_choice], (long)b, (long)result->result);
    } else {
        printf("\n==> Error: Operation failed (status=%ld)\n", result ? (long)result->status : -1L);
    }
}

// Option 2: String processing
static void option_string_processing(void) {
    char input[65];
    int op_choice;
    
    printf("\n--- String Processing ---\n");
    
    // Get input string
    printf("Enter a string (max 63 chars): \n");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Invalid input!\n");
        return;
    }
    
    // Check if we got just a newline (empty input) - if so, read again
    if (input[0] == '\n') {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Invalid input!\n");
            return;
        }
    }
    
    // Remove trailing newline
    size_t len = strlen(input);
    if (len > 0 && input[len-1] == '\n') {
        input[len-1] = '\0';
    }
    
    // Get operation
    printf("\nSelect operation:\n");
    printf("  0) Remove spaces\n");
    printf("  1) To UPPERCASE\n");
    printf("  2) To lowercase\n");
    printf("  3) Count digits\n");
    printf("Choice: \n");
    if (scanf("%d", &op_choice) != 1 || op_choice < 0 || op_choice > 3) {
        printf("Invalid operation!\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();
    
    // Call RPC
    printf("\n[Client] Calling remote processString(\"%s\", %d)...\n", input, op_choice);
    StringResult *result = processString(input, (StringOp)op_choice);
    
    // Display result
    if (result && result->status == 0) {
        if (op_choice == 3) {  // Count digits
            printf("\n==> Number of digits in \"%s\": %ld\n", input, (long)result->count);
        } else {
            const char *op_names[] = {"Remove spaces", "UPPERCASE", "lowercase"};
            printf("\n==> %s result: \"%s\"\n", op_names[op_choice], result->output_str);
        }
    } else {
        printf("\n==> Error: Operation failed (status=%ld)\n", result ? (long)result->status : -1L);
    }
}

// Option 3: Array sorting
static void option_array_sorting(void) {
    IntArray input_array;
    int n;
    static int32_t array_data[10];  // Static storage for array elements
    
    printf("\n--- Array Sorting ---\n");
    
    // Get number of elements
    printf("Enter number of elements (1-10): \n");
    if (scanf("%d", &n) != 1 || n < 1 || n > 10) {
        printf("Invalid number! Must be between 1 and 10.\n");
        clear_input_buffer();
        return;
    }
    
    input_array.length = n;
    input_array.data.elementsCount = n;
    input_array.data.elements = array_data;
    
    // Get array elements
    printf("Enter %d integers:\n", n);
    for (int i = 0; i < n; i++) {
        printf("  Element %d: \n", i + 1);
        if (scanf("%ld", &array_data[i]) != 1) {
            printf("Invalid input!\n");
            clear_input_buffer();
            return;
        }
    }
    clear_input_buffer();
    
    // Display input array
    printf("\nInput array: [");
    for (int i = 0; i < n; i++) {
        printf("%ld", (long)array_data[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
    
    // Call RPC
    printf("\n[Client] Calling remote sortArray()...\n");
    IntArray *result = sortArray(&input_array);
    
    // Display sorted array
    if (result) {
        printf("\n==> Sorted array (ascending): [");
        for (uint32_t i = 0; i < result->data.elementsCount; i++) {
            printf("%ld", (long)result->data.elements[i]);
            if (i < result->data.elementsCount - 1) printf(", ");
        }
        printf("]\n");
    } else {
        printf("\n==> Error: sortArray failed\n");
    }
}

int main(void)
{
    // Allow time for serial console and peer board to come up
    xtimer_sleep(1);

    printf("\n====================================\n");
    printf("  Enhanced RPC Demo - CLIENT (UART)\n");
    printf("  RIOT OS + eRPC on nRF52840-DK\n");
    printf("====================================\n\n");
    printf("Using UART1 (D0/D1) @115200. Make sure boards are cross-connected.\n\n");

    erpc_transport_t transport = (erpc_transport_t)erpc_uart_transport_init(UART_DEV(1), 115200);
    if (!transport) {
        printf("[ERROR] Failed to create UART transport\n");
        return 1;
    }
    
    // Create message buffer factory
    erpc_mbf_t mbf = erpc_mbf_dynamic_init();
    if (!mbf) {
        printf("[ERROR] Failed to create message buffer factory\n");
        return 1;
    }

    // Initialize client
    erpc_client_t client = erpc_client_init(transport, mbf);
    if (!client) {
        printf("[ERROR] Failed to initialize eRPC client\n");
        erpc_mbf_dynamic_deinit(mbf);
        return 1;
    }

    // Initialize generated C client wrapper
    initDemoService_client(client);

    // Quick connectivity test
    printf("\n[Test] Attempting connection test (2 + 2)...\n");
    CalcResult *test_result = calculate(2, 2, OP_ADD);
    if (!(test_result && test_result->status == 0 && test_result->result == 4)) {
        printf("[ERROR] Connection test failed. Check wiring and reboot both boards.\n");
        deinitDemoService_client();
        erpc_client_deinit(client);
        erpc_mbf_dynamic_deinit(mbf);
        return 1;
    }
    printf("[Test] OK: RPC working over UART!\n\n");

    // Main menu loop
    int choice;
    bool running = true;
    
    printf("\n*** Menu is ready! ***\n");
    printf("*** Type a number (1-4) and press ENTER ***\n\n");
    
    while (running) {
        display_main_menu();
        
        if (scanf("%d", &choice) != 1) {
            int c = getchar();
            if (c == EOF) {
                printf("\n[ERROR] Input stream closed. Exiting...\n");
                break;
            }
            printf("Invalid input! Please enter a number.\n");
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();
        
        printf("\n[You selected: %d]\n", choice);
        
        switch (choice) {
            case 1:
                option_integer_calculation();
                break;
            case 2:
                option_string_processing();
                break;
            case 3:
                option_array_sorting();
                break;
            case 4:
                printf("\nExiting... Goodbye!\n");
                running = false;
                break;
            default:
                printf("\nInvalid choice! Please select 1-4.\n");
                break;
        }
        
        if (running) {
            printf("\nPress Enter to continue...\n");
            getchar();
        }
    }

    // Cleanup
    deinitDemoService_client();
    erpc_client_deinit(client);
    erpc_mbf_dynamic_deinit(mbf);

    return 0;
}
