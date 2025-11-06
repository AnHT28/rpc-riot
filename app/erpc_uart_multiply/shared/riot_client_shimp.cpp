// riot_client_shim.cpp — C-linkage wrappers that call the generated C++ client
#include <cstdint>

#include "erpc_manually_constructed.hpp"   // ERPC_MANUALLY_CONSTRUCTED_STATIC
#include "multiply_demo_client.hpp"        // erpcShim::MultiplyService_client
#include "erpc_client_manager.h"           // erpc::ClientManager (C++)
#include "erpc_client_setup.h"             // erpc_client_t (C)
#include "c_multiply_demo_client.h"        // C prototypes we must satisfy

using erpcShim::MultiplyService_client;
using erpc::ClientManager;

#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
static MultiplyService_client *s_MultiplyService_client = nullptr;
#else
ERPC_MANUALLY_CONSTRUCTED_STATIC(MultiplyService_client, s_MultiplyService_client);
#endif

extern "C" {

// C API expected by your main.cpp

int32_t multiply_rpc(int32_t a, int32_t b)
{
#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
    if (!s_MultiplyService_client) return 0;
    return s_MultiplyService_client->multiply(a, b);
#else
    return s_MultiplyService_client.get()->multiply(a, b);
#endif
}

void initMultiplyService_client(erpc_client_t client)
{
    auto *mgr = reinterpret_cast<ClientManager *>(client);

#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
    if (!s_MultiplyService_client) {
        s_MultiplyService_client = new MultiplyService_client(mgr);
    }
#else
    if (!s_MultiplyService_client.isUsed()) {
        s_MultiplyService_client.construct(mgr);
    }
#endif
}

void deinitMultiplyService_client(void)
{
#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
    if (s_MultiplyService_client) {
        delete s_MultiplyService_client;
        s_MultiplyService_client = nullptr;
    }
#else
    if (s_MultiplyService_client.isUsed()) {
        s_MultiplyService_client.destroy();
    }
#endif
}

} // extern "C"
