Repository: RIOT + eRPC demo (rpc-riot)

This repository contains a RIOT OS tree with an example application that demonstrates eRPC (Embedded RPC) usage between a client and server. The guidance below is aimed at automated coding agents (Copilot-style) to make productive edits quickly and safely.

High-level context
- Top-level RIOT tree: `RIOT/` — the OS, build system and examples. Use `RIOT/README.md` for general RIOT guidance.
- eRPC module: `modules/erpc/erpc/README.md` — upstream eRPC docs and examples.
- Example app: `app/erpc_multiply/` — a one-process demo that contains both the RIOT server and a TCP-based host client.

What this demo shows (important files)
- `app/erpc_multiply/Makefile` — RIOT application Makefile. Note `EXTERNAL_MODULE_DIRS` points to `/home/an/rpc-riot/modules` and `USEMODULE += erpc` to pull in the eRPC module.
- `app/erpc_multiply/main.cpp` — creates a RIOT server thread and a client thread; uses `riot_uart_transport_create()` for a UART transport on native builds.
- `app/erpc_multiply/riot_uart_transport.cpp` — minimal FramedTransport implementation for RIOT's UART/native stdio.
- `app/erpc_multiply/test_server_app.cpp`, `multiply_impl.cpp` — example service implementation (MultiplyService_impl).
- `app/erpc_multiply/test_client_app.cpp` — a host-style TCP client using `erpc_transport_tcp_init("127.0.0.1", 50051, false)`; useful as a runnable example outside of embedded hardware.
- `app/erpc_multiply/*.erpc` and generated headers (`multiply_demo_*`) — IDL and generated client/server shims; changes to IDL require running `erpcgen` (see modules/erpc docs).

Build / run notes for agents
- Typical RIOT app build: the app Makefile uses `RIOTBASE` and `BOARD` variables. Example: builds are done using the RIOT build system from the repository root. When adding code, prefer small incremental builds to avoid long CI runs.
- The `app/erpc_multiply/Makefile` expects `RIOTBASE` to be set (common pattern): you can run builds from the repo root or set `RIOTBASE` to the `RIOT/` folder. Example developer command (not run by the agent): make -C app/erpc_multiply BOARD=native
- Host TCP client: `test_client_app.cpp` is a plain C++ program that uses eRPC TCP transport. It can be compiled/link by the RIOT native build or adapted as a standalone binary; the code shows how to initialize transport, mbf, and client manager (see lines using `erpc_client_init` and `erpc::ClientManager`). Use this file as the template when needing a test client on the host.

Patterns & conventions to follow
- Single-responsibility C++ classes for transports/services: transports inherit from eRPC FramedTransport (see `riot_uart_transport.cpp`), services implement generated interface classes (see `MultiplyService_impl`). Match method signatures exactly – generated headers expect the concrete names and symbols (e.g. `get_multiply_impl`).
- C / C++ mixed usage: RIOT apps commonly use extern "C" for RIOT C APIs and for the C-based eRPC setup. Preserve extern "C" blocks around C headers (`erpc_client_setup.h`, `thread.h`, etc.).
- Generated code expectations: the repository includes generated headers (`multiply_demo_client.hpp`, `multiply_demo_interface.hpp`, `c_multiply_demo_client.h`). If changing IDL, run `erpcgen` to regenerate these; don't hand-edit generated files unless fixing an immediate bug and documenting why.
- Error handling: many examples return nullptr or print to stderr on failure (see `test_client_app.cpp`). When adding new init sequences, follow this simple fail-fast pattern and free resources with `erpc_client_deinit`, `erpc_mbf_dynamic_deinit`, and transport deinit calls where available.

Integration points & external dependencies
- eRPC module: `modules/erpc/` contains the eRPC implementation and `erpcgen`. See `modules/erpc/erpc/README.md` for how to build or run `erpcgen` if you need to change IDL.
- RIOT build system: uses Makefiles with `RIOTBASE` and `BOARD` environment variables. The `app/erpc_multiply/Makefile` demonstrates `CXXEXFLAGS` and `CPPFLAGS` use for C++ options.
- Native vs hardware: On `BOARD=native`, UART0 maps to stdio (see `riot_uart_transport.cpp`); on real hardware, UART transport uses `uart_init` and `uart_write`/callbacks.

Concrete examples agents can use when editing
- To add a new service, follow `app/erpc_multiply/multiply_demo_interface.hpp` usage: implement the generated interface, add a factory function `extern "C" <Service> *get_<service>_impl()` and register via `erpc_add_service_to_server` in `main.cpp`.
- To add a transport, inherit from `erpc::FramedTransport` and implement `underlyingSend`/`underlyingReceive` (see `riot_uart_transport.cpp`). Provide a C factory returning `void *` for compatibility with the C setup code.
- To test locally without hardware, prefer editing/using `test_client_app.cpp` which uses `erpc_transport_tcp_init("127.0.0.1", 50051, false)`; run the RIOT native build for the server and the host client in a separate process if necessary.

Quality and safety rules for automated edits
- Do not modify generated files unless the change is a documented bugfix; instead modify the `.erpc` IDL and run `erpcgen`.
- Preserve `extern "C"` linkage blocks around C headers and C-style factory functions.
- Keep builds small: change a single module or app at a time and run the native board build (BOARD=native) to validate compile errors early.

If something is unclear or you need developer-specific workflows (custom CI, special tool versions for `erpcgen`, or preferred test harnesses), ask the maintainers and include evidence / failing build output. After updates, run a native build to validate the change.

Feedback requested
- Does this cover the developer workflows you expect Copilot-style agents to follow? Tell me if you want included: exact build commands you run locally, CI matrix details, or `erpcgen` wrapper scripts.
