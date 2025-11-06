#include <cstdint>
#include <cstdio>
#include "multiply_demo_interface.hpp"   // generated
using namespace erpcShim;

class MultiplyService_impl : public MultiplyService_interface {
public:
    int32_t multiply(int32_t a, int32_t b) override {
        int32_t result = a * b;
        std::fprintf(stderr, "Server received: %ld * %ld = %ld\n",
                     (long)a, (long)b, (long)result);
        return result;
    }
};

// Factory the server thread will use
MultiplyService_interface *get_multiply_impl() {
    static MultiplyService_impl impl;
    return &impl;
}
