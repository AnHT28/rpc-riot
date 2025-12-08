/*
 * Client-side menu application for Enhanced RPC Demo
 * Bachelor Thesis: "Implementation and Evaluation of a Lightweight RPC System 
 * for Resource-Constrained Devices using RIOT OS"
 */

#include <stdio.h>
#include <string.h>
#include <cstdlib>
extern "C" {
#include "xtimer.h"
#include "erpc_client_setup.h"
#include "erpc_mbf_setup.h"
#include "periph/uart.h"
}

// Local eRPC allocation hooks (C-linkage to match the eRPC C API)
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
void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Helper function to display main menu
void display_main_menu(void) {
    printf("\n");
    printf("====================================\n");
    printf("  Lightweight RPC Demo (eRPC + RIOT)\n");
    printf("====================================\n");
    printf("Select function:\n\n");
    printf("  1) Integer calculation\n");
    printf("  2) String processing\n");
    printf("  3) Array sorting\n");
    printf("  4) Exit\n\n");
    printf("Enter your choice: \n");
}

// Option 1: Integer calculation
void option_integer_calculation(void) {
    int32_t a, b;
    int op_choice;
    
    printf("\n--- Integer Calculation ---\n");
    
    // Get first number
    printf("Enter first integer (a): \n");
    if (scanf("%d", &a) != 1) {
        printf("Invalid input!\n");
        clear_input_buffer();
        return;
    }
    
    // Get second number
    printf("Enter second integer (b): \n");
    if (scanf("%d", &b) != 1) {
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
    printf("\n[Client] Calling remote calculate(%d, %d, %d)...\n", a, b, op_choice);
    CalcResult *result = calculate(a, b, (ArithmeticOp)op_choice);
    
    // Display result
    if (result && result->status == 0) {
        const char *op_symbols[] = {"+", "-", "*", "/"};
        printf("\n==> Result: %d %s %d = %d\n", a, op_symbols[op_choice], b, result->result);
    } else {
        printf("\n==> Error: Operation failed (status=%d)\n", result ? result->status : -1);
    }
}

// Option 2: String processing
void option_string_processing(void) {
    char input[65];
    int op_choice;
    
    printf("\n--- String Processing ---\n");
    
    // Get input string
    printf("Enter a string (max 63 chars): \n");
    // Use scanf with %[^\n] to read until newline (but this requires non-empty input)
    // Better: use fgets and handle empty lines
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
            printf("\n==> Number of digits in \"%s\": %d\n", input, result->count);
        } else {
            const char *op_names[] = {"Remove spaces", "UPPERCASE", "lowercase"};
            printf("\n==> %s result: \"%s\"\n", op_names[op_choice], result->output_str);
        }
    } else {
        printf("\n==> Error: Operation failed (status=%d)\n", result ? result->status : -1);
    }
}

// Option 3: Array sorting
void option_array_sorting(void) {
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
        if (scanf("%d", &array_data[i]) != 1) {
            printf("Invalid input!\n");
            clear_input_buffer();
            return;
        }
    }
    clear_input_buffer();
    
    // Display input array
    printf("\nInput array: [");
    for (int i = 0; i < n; i++) {
        printf("%d", array_data[i]);
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
            printf("%d", result->data.elements[i]);
            if (i < result->data.elementsCount - 1) printf(", ");
        }
        printf("]\n");
    } else {
        printf("\n==> Error: sortArray failed\n");
    }
}

int main(void)
{
    // Initialize UART (native build uses stdio-based transport)
    uart_init(UART_DEV(0), 115200, NULL, NULL);

    printf("\n====================================\n");
    printf("  Enhanced RPC Demo - CLIENT\n");
    printf("  RIOT OS + eRPC\n");
    printf("====================================\n");
    printf("\nConnecting to server...\n");
    printf("(Make sure server is running on port 50051)\n");
    fflush(stdout);

    // Retry connection a few times
    erpc_transport_t transport = NULL;
    int retries = 5;
    for (int i = 0; i < retries && !transport; i++) {
        if (i > 0) {
            printf("[Retry %d/%d] Attempting to connect...\n", i + 1, retries);
            xtimer_sleep(2);
        }
        transport = erpc_transport_tcp_init("127.0.0.1", 50051, false);
    }
    
    if (!transport) {
        printf("\n[ERROR] Failed to connect to server after %d attempts\n", retries);
        printf("[ERROR] Make sure the server is running first:\n");
        printf("[ERROR]   Terminal 1: cd server && make BOARD=native term\n");
        printf("[ERROR]   Then run this client again\n");
        return 1;
    }
    
    printf("[OK] Connected to server!\n");

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

    printf("\n====================================\n");
    printf("  Enhanced RPC Demo - CLIENT\n");
    printf("  RIOT OS + eRPC\n");
    printf("====================================\n");
    printf("\nWaiting for server connection...\n");
    printf("(Make sure server is running on port 50051)\n");
    xtimer_sleep(2);
    
    // Test connection with a simple calculation
    printf("\n[Test] Attempting connection test...\n");
    CalcResult *test_result = calculate(2, 2, OP_ADD);
    if (test_result && test_result->status == 0) {
        printf("[Test] Connection successful!\n");
        if (test_result->result == 4) {
            printf("[Test] RPC working correctly: 2 + 2 = %d\n", test_result->result);
        } else {
            printf("[Test] Connected but result unexpected: 2 + 2 = %d (expected 4)\n", test_result->result);
        }
    } else {
        printf("[ERROR] Connection test failed!\n");
        printf("[ERROR] Make sure the server is running first.\n");
        deinitDemoService_client();
        erpc_client_deinit(client);
        erpc_mbf_dynamic_deinit(mbf);
        return 1;
    }
    printf("\n");

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
