// loopback_transport.cpp — in-process, blocking loopback for eRPC
extern "C" {
#include "mutex.h"
#include "xtimer.h"
}
#include <cstdint>
#include <cstddef>
#include <cstring>
#include "erpc_framed_transport.hpp"

using namespace erpc;

#define ERPC_LOOPBACK_LOG 1
#if ERPC_LOOPBACK_LOG
#include <cstdio>
static inline void dump_prefix(const char *tag, const uint8_t *buf, size_t len, size_t max = 16) {
    std::printf("%s %uB:", tag, (unsigned)len);
    for (size_t i = 0; i < len && i < max; ++i) std::printf(" %02X", buf[i]);
    if (len > max) std::printf(" ...");
    std::printf("\n");
}
#endif


// Very small ring buffer (single producer/consumer)
struct Ring {
    static constexpr size_t CAP = 2048;
    uint8_t buf[CAP];
    size_t head = 0, tail = 0; // head: write, tail: read

    size_t avail() const { return (head + CAP - tail) % CAP; }            // bytes in buffer
    size_t space() const { return CAP - 1 - avail(); }                     // free space
    size_t read(uint8_t *dst, size_t n) {
        size_t got = 0;
        while (got < n && tail != head) {
            dst[got++] = buf[tail];
            tail = (tail + 1) % CAP;
        }
        return got;
    }
    size_t write(const uint8_t *src, size_t n) {
        size_t put = 0;
        while (put < n && space()) {
            buf[head] = src[put++];
            head = (head + 1) % CAP;
        }
        return put;
    }
};

struct Shared {
    mutex_t lock;
    Ring a2b; // bytes from endpoint A -> B
    Ring b2a; // bytes from endpoint B -> A
    Shared() { mutex_init(&lock); }
};

class LoopbackEndpoint : public FramedTransport {
public:
    // dir=false => this is A (receives from b2a, sends to a2b)
    // dir=true  => this is B (receives from a2b, sends to b2a)
    LoopbackEndpoint(Shared *sh, bool dirB) : _sh(sh), _dirB(dirB) {}
    erpc_status_t init() { return kErpcStatus_Success; }


protected:
    erpc_status_t underlyingSend(const uint8_t *data, uint32_t size) override {
        uint32_t left = size;
        while (left) {
            mutex_lock(&_sh->lock);
            Ring &out = _dirB ? _sh->b2a /*B->A*/ : _sh->a2b /*A->B*/;
            size_t wrote = out.write(data + (size - left), left);
            mutex_unlock(&_sh->lock);
            left -= static_cast<uint32_t>(wrote);
            if (left) xtimer_usleep(1000); // 1ms backoff until peer drains
        }
        #if ERPC_LOOPBACK_LOG
            dump_prefix("[loopback TX]", data, size);
        #endif
        return kErpcStatus_Success;
    }

    erpc_status_t underlyingReceive(uint8_t *data, uint32_t size) override {
        uint32_t got = 0;
        while (got < size) {
            mutex_lock(&_sh->lock);
            Ring &in = _dirB ? _sh->a2b /*A->B -> for B*/ : _sh->b2a /*B->A -> for A*/;
            size_t r = in.read(data + got, size - got);
            mutex_unlock(&_sh->lock);
            got += static_cast<uint32_t>(r);
            if (got < size) xtimer_usleep(1000); // wait for producer
        }
        #if ERPC_LOOPBACK_LOG
            dump_prefix("[loopback RX]", data, size);
        #endif
        return kErpcStatus_Success;
    }

private:
    Shared *_sh;
    bool _dirB;
};

// Simple factory that returns a pair of endpoints sharing the buffer
static Shared g_shared;
static LoopbackEndpoint *g_epA = nullptr;
static LoopbackEndpoint *g_epB = nullptr;

extern "C" void *erpc_loopback_create_A() {
    if (!g_epA) g_epA = new LoopbackEndpoint(&g_shared, false);
    return g_epA;
}
extern "C" void *erpc_loopback_create_B() {
    if (!g_epB) g_epB = new LoopbackEndpoint(&g_shared, true);
    return g_epB;
}
