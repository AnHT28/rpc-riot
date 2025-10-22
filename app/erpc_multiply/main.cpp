#include <cstdio>

/* ---- RIOT C headers ---- */
extern "C" {
#include "thread.h"

/* eRPC setup C API */
#include "erpc_client_setup.h"
#include "erpc_server_setup.h"
#include "erpc_mbf_setup.h"
}

extern "C" {
    /* yield/sleep helpers from RIOT */
    #include "thread.h"
}

extern "C" {
    void    initMultiplyService_client(erpc_client_t client);
    int32_t multiply_rpc(int32_t a, int32_t b);
}


/* ---- Generated C++ server-side wrappers ---- */
#include "multiply_demo_server.hpp"
#include "multiply_demo_interface.hpp"

/* ---- Loopback transport factories (we wrote these) ---- */
extern "C" void *erpc_loopback_create_A(void);
extern "C" void *erpc_loopback_create_B(void);

/* ---- Your service implementation (from test_server_app.cpp) ---- */
extern "C" erpcShim::MultiplyService_interface *get_multiply_impl(void);

typedef void* erpc_service_t;   // opaque handle; matches the generated typedef
using namespace erpcShim;

static char server_stack[THREAD_STACKSIZE_MAIN + 1024];
static char client_stack[THREAD_STACKSIZE_MAIN + 1024];

/* Share one message-buffer-factory between threads to avoid double-init issues */
static erpc_mbf_t g_mbf = nullptr;

static void *server_thread(void *arg)
{
    (void)arg;

    /* Loopback endpoint A */
    void *t = erpc_loopback_create_A();
    if (!t) { puts("[server] ERROR: transport create failed"); return nullptr; }

    erpc_mbf_t mbf = g_mbf;
    if (!mbf) { puts("[server] ERROR: mbf not initialized"); return nullptr; }

    erpc_server_t srv = erpc_server_init((erpc_transport_t)t, mbf);
    if (!srv) { puts("[server] ERROR: server init failed"); return nullptr; }

    /* Bind your C++ implementation to the generated C++ service wrapper */
    MultiplyService_interface *impl = get_multiply_impl();
    if (!impl) { puts("[server] ERROR: service impl null"); return nullptr; }
    static MultiplyService_service service(impl);

    /* Register the service with the C API */
    erpc_add_service_to_server(srv, reinterpret_cast<erpc_service_t>(&service));

    puts("[server] running...");

    /* Prefer a single blocking run call; if it returns repeatedly, yield to avoid starving other threads */
    while (1) {
        erpc_status_t st = erpc_server_run(srv);
        if (st != kErpcStatus_Success) {
            /* avoid tight busy-loop if server_run is non-blocking or returns quickly */
            puts("[server] erpc_server_run returned, yielding to other threads...");
            thread_yield();
        }
    }
    return nullptr;
}

static void *client_thread(void *arg)
{
    (void)arg;

    /* Loopback endpoint B */
    void *t = erpc_loopback_create_B();
    if (!t) { puts("[client] ERROR: transport create failed"); return nullptr; }

    erpc_mbf_t mbf = g_mbf;
    if (!mbf) { puts("[client] ERROR: mbf not initialized"); return nullptr; }

    erpc_client_t cl = erpc_client_init((erpc_transport_t)t, mbf);
    if (!cl) { puts("[client] ERROR: client init failed"); return nullptr; }

    /* Use your real client (provided via riot_client_shim.cpp or generated code) */
    initMultiplyService_client(cl);
    puts("[client] calling multiply...");
    int32_t r = multiply_rpc(6, 7);
    std::printf("multiply(6, 7) = %ld\n", (long)r);
    return nullptr;
}

int main(void)
{
    puts("RIOT eRPC multiply (one-process loopback)");

    /* init single MBF for both threads */
    g_mbf = erpc_mbf_dynamic_init();
    if (!g_mbf) {
        puts("[main] ERROR: erpc_mbf_dynamic_init failed");
        return 1;
    }

    thread_create(server_stack, sizeof(server_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                  server_thread, nullptr, "erpc_server");

    thread_create(client_stack, sizeof(client_stack),
                  THREAD_PRIORITY_MAIN - 2, THREAD_CREATE_STACKTEST,
                  client_thread, nullptr, "erpc_client");

    return 0;
}
