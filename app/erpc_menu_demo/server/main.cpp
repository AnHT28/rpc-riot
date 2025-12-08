/*
 * Server-side implementation of the Enhanced RPC Demo Service
 * Bachelor Thesis: "Implementation and Evaluation of a Lightweight RPC System 
 * for Resource-Constrained Devices using RIOT OS"
 */

#include <cstdio>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <cstdlib>
extern "C" {
#include "thread.h"
#include "erpc_server_setup.h"
#include "erpc_mbf_setup.h"
#include "periph/uart.h"
}

// Local eRPC allocation hooks (kept C-linkage for the C-based eRPC APIs)
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

/* Generated C++ service wrapper */
#include "demo_service_server.hpp"
#include "demo_service_interface.hpp"

using namespace erpcShim;

// Implementation of the DemoService interface (server-side)
class DemoService_impl : public DemoService_interface {
public:
    // Option 1: Integer calculation
    CalcResult * calculate(int32_t a, int32_t b, ArithmeticOp operation) override {
        std::printf("[Server] calculate(%d, %d, op=%d)\n", a, b, (int)operation);
        
        // Allocate result with erpc_malloc - eRPC will free it after serialization
        CalcResult *result = (CalcResult *)erpc_malloc(sizeof(CalcResult));
        if (!result) {
            std::printf("[Server] ERROR: Failed to allocate CalcResult\n");
            return nullptr;
        }
        
        result->status = 0;  // success by default
        
        switch (operation) {
            case OP_ADD:
                result->result = a + b;
                std::printf("[Server] ADD: %d + %d = %d\n", a, b, result->result);
                break;
            case OP_SUBTRACT:
                result->result = a - b;
                std::printf("[Server] SUBTRACT: %d - %d = %d\n", a, b, result->result);
                break;
            case OP_MULTIPLY:
                result->result = a * b;
                std::printf("[Server] MULTIPLY: %d * %d = %d\n", a, b, result->result);
                break;
            case OP_DIVIDE:
                if (b == 0) {
                    std::printf("[Server] ERROR: Division by zero!\n");
                    result->result = 0;
                    result->status = -1;  // error
                } else {
                    result->result = a / b;  // Integer division
                    std::printf("[Server] DIVIDE: %d / %d = %d\n", a, b, result->result);
                }
                break;
            default:
                std::printf("[Server] ERROR: Unknown operation\n");
                result->status = -1;
                result->result = 0;
                break;
        }
        
        return result;
    }
    
    // Option 2: String processing
    StringResult * processString(const char * input, StringOp operation) override {
        std::printf("[Server] processString(\\\"%s\\\", op=%d)\n", input, (int)operation);
        
        // Allocate result with erpc_malloc - eRPC will free it after serialization
        StringResult *result = (StringResult *)erpc_malloc(sizeof(StringResult));
        if (!result) {
            std::printf("[Server] ERROR: Failed to allocate StringResult\n");
            return nullptr;
        }
        
        result->status = 0;  // success by default
        result->count = 0;
        
        size_t len = strlen(input);
        if (len > 63) len = 63;  // Safety limit
        
        // Allocate string buffer with erpc_malloc - eRPC will free it
        char *out_str = (char *)erpc_malloc(65);  // Max 64 + null terminator
        if (!out_str) {
            std::printf("[Server] ERROR: Failed to allocate output string\n");
            erpc_free(result);
            return nullptr;
        }
        
        switch (operation) {
            case STR_REMOVE_SPACES: {
                size_t j = 0;
                for (size_t i = 0; i < len && j < 63; i++) {
                    if (input[i] != ' ') {
                        out_str[j++] = input[i];
                    }
                }
                out_str[j] = '\0';
                std::printf("[Server] REMOVE_SPACES: \\\"%s\\\"\n", out_str);
                break;
            }
            case STR_TO_UPPER: {
                for (size_t i = 0; i < len; i++) {
                    out_str[i] = std::toupper((unsigned char)input[i]);
                }
                out_str[len] = '\0';
                std::printf("[Server] TO_UPPER: \\\"%s\\\"\n", out_str);
                break;
            }
            case STR_TO_LOWER: {
                for (size_t i = 0; i < len; i++) {
                    out_str[i] = std::tolower((unsigned char)input[i]);
                }
                out_str[len] = '\0';
                std::printf("[Server] TO_LOWER: \\\"%s\\\"\n", out_str);
                break;
            }
            case STR_COUNT_DIGITS: {
                int count = 0;
                for (size_t i = 0; i < len; i++) {
                    if (std::isdigit((unsigned char)input[i])) {
                        count++;
                    }
                }
                result->count = count;
                out_str[0] = '\0';  // Empty string for count operation
                std::printf("[Server] COUNT_DIGITS: %d digits found\n", count);
                break;
            }
            default:
                std::printf("[Server] ERROR: Unknown string operation\n");
                result->status = -1;
                out_str[0] = '\0';
                break;
        }
        
        result->output_str = out_str;
        return result;
    }
    
    // Option 3: Array sorting
    IntArray * sortArray(const IntArray * input_array) override {
        std::printf("[Server] sortArray(length=%d)\n", input_array->length);
        
        // Validate length
        if (input_array->length < 0 || input_array->length > 10) {
            std::printf("[Server] ERROR: Invalid array length\n");
            // Return minimal valid result
            IntArray *result = (IntArray *)erpc_malloc(sizeof(IntArray));
            if (result) {
                result->length = 0;
                result->data.elementsCount = 0;
                result->data.elements = nullptr;
            }
            return result;
        }
        
        // Allocate result with erpc_malloc - eRPC will free it
        IntArray *result = (IntArray *)erpc_malloc(sizeof(IntArray));
        if (!result) {
            std::printf("[Server] ERROR: Failed to allocate IntArray\n");
            return nullptr;
        }
        
        // Allocate array buffer with erpc_malloc - eRPC will free it
        int32_t *arr = (int32_t *)erpc_malloc(input_array->data.elementsCount * sizeof(int32_t));
        if (!arr) {
            std::printf("[Server] ERROR: Failed to allocate array buffer\n");
            erpc_free(result);
            return nullptr;
        }
        
        result->length = input_array->length;
        result->data.elementsCount = input_array->data.elementsCount;
        result->data.elements = arr;
        
        // Print input array and copy to result
        std::printf("[Server] Input array: [");
        for (uint32_t i = 0; i < input_array->data.elementsCount; i++) {
            arr[i] = input_array->data.elements[i];
            std::printf("%d", input_array->data.elements[i]);
            if (i < input_array->data.elementsCount - 1) std::printf(", ");
        }
        std::printf("]\n");
        
        // Sort the array (ascending order)
        std::sort(arr, arr + result->data.elementsCount);
        
        // Print sorted array
        std::printf("[Server] Sorted array: [");
        for (uint32_t i = 0; i < result->data.elementsCount; i++) {
            std::printf("%d", arr[i]);
            if (i < result->data.elementsCount - 1) std::printf(", ");
        }
        std::printf("]\n");
        
        return result;
    }
};


int main(void)
{
    std::puts("====================================");
    std::puts("  Enhanced RPC Demo - SERVER");
    std::puts("  RIOT OS + eRPC");
    std::puts("====================================");

    /* create TCP transport for native builds (host-to-host RPC) */
    erpc_transport_t transport = erpc_transport_tcp_init("0.0.0.0", 50051, true);
    if (!transport) {
        std::puts("[ERROR] TCP transport create failed");
        return 1;
    }

    /* init message buffer factory */
    erpc_mbf_t mbf = erpc_mbf_dynamic_init();
    if (!mbf) {
        std::puts("[ERROR] MBF init failed");
        return 1;
    }

    /* init server */
    erpc_server_t srv = erpc_server_init((erpc_transport_t)transport, mbf);
    if (!srv) {
        std::puts("[ERROR] Server init failed");
        return 1;
    }

    /* create implementation and service wrapper */
    static DemoService_impl impl;
    static DemoService_service service(&impl);

    /* register service */
    erpc_add_service_to_server(srv, reinterpret_cast<void *>(&service));

    std::puts("[Server] Ready and listening on port 50051...");
    std::puts("[Server] Waiting for client connections...\n");

    /* run server loop */
    while (1) {
        erpc_status_t st = erpc_server_run(srv);
        if (st != kErpcStatus_Success) {
            // This is normal when no client is connected yet
            // Just yield and try again
            thread_yield();
        }
    }

    return 0;
}
