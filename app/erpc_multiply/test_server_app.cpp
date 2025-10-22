// test_server_app.cpp  (RIOT version: service impl only, no main)
#include <cstdio>
#include <cstdint>
#include "multiply_demo_interface.hpp"   // generated
// Some generated code references this symbol; keep it off.
//extern "C" bool nestingDetection = false;
bool nestingDetection = false;

using namespace erpcShim;

// Your actual service implementation...
class MultiplyService_impl : public MultiplyService_interface {
public:
    int32_t multiply(int32_t a, int32_t b) override {
        int32_t result = a * b;
        std::printf("Server received: %ld * %ld = %ld\n", (long)a, (long)b, (long)result);
        return result;
    }
};

// Factory for the RIOT server thread to fetch the impl
extern "C" MultiplyService_interface *get_multiply_impl(void) {
    static MultiplyService_impl impl;
    return &impl;
}
