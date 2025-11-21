# Architecture: eRPC nRF52840DK Demo

This document describes the architecture of the `erpc_nrf_demo` application, which demonstrates Remote Procedure Calls (eRPC) between two Nordic nRF52840DK boards running RIOT OS.

## System Overview

The system consists of two distinct nodes:
1.  **Client Node**: Accepts user input via the console, sends calculation requests via eRPC, and displays results.
2.  **Server Node**: Listens for eRPC requests, performs calculations, and returns results.

Communication happens over a physical UART connection using a custom RIOT-based transport layer.

## Hardware Architecture

### Components
*   **Boards**: 2x Nordic nRF52840DK (PCA10056).
*   **Interconnect**: 3x Jumper wires (TX, RX, GND).

### Pin Configuration
Both Client and Server use the same pin mapping for `UART_DEV(1)`:

| Signal | Pin Name | Pin Number | Description |
| :--- | :--- | :--- | :--- |
| **TX** | P1.02 | D1 | Transmit Data |
| **RX** | P1.01 | D0 | Receive Data |
| **GND** | GND | GND | Common Ground |

### Wiring Diagram
```
[Client Board]      [Server Board]
   P1.02 (TX) --------> P1.01 (RX)
   P1.01 (RX) <-------- P1.02 (TX)
      GND     --------    GND
```

## Software Architecture

### Layered Stack

| Layer | Component | Description |
| :--- | :--- | :--- |
| **Application** | `client/main.cpp` | Interactive CLI, calls generated eRPC functions. |
| | `server/main.cpp` | Implements `Calculator_service`, initializes server loop. |
| **Service (IDL)** | `calculator.erpc` | Defines the interface (add, subtract, multiply, divide). |
| **eRPC Shim** | Generated Code | `c_calculator_client.cpp`, `calculator_server.cpp`, etc. |
| **Transport** | `RiotUartTransport` | Custom C++ class inheriting from `erpc::FramedTransport`. |
| **OS Drivers** | `periph_uart` | RIOT's low-level UART driver. |
| **OS Primitives** | `tsrb`, `sema` | Thread-Safe Ring Buffer for RX, Semaphores for blocking wait. |
| **Kernel** | RIOT OS | Threading, ISR management, Hardware abstraction. |

### Key Modules

#### 1. RiotUartTransport (`shared/riot_uart_transport.cpp`)
This is the bridge between eRPC and RIOT.
*   **Interrupt Driven RX**: Uses `uart_init` with a callback (`rx_cb`).
*   **Buffering**: Incoming bytes are stored in a `tsrb` (Ring Buffer) inside the ISR.
*   **Synchronization**: A semaphore (`sema`) is posted in the ISR and waited on in `underlyingReceive`, allowing the eRPC thread to sleep when no data is available.

#### 2. Client Application (`client/main.cpp`)
*   **Initialization**: Sets up `UART_DEV(0)` for stdio and `UART_DEV(1)` for eRPC.
*   **User Interface**: Uses `fgets` to safely read user input from the debug console.
*   **Stack Size**: Increased `THREAD_STACKSIZE_MAIN` to 2048 to handle `printf` and eRPC overhead.

#### 3. Server Application (`server/main.cpp`)
*   **Service Implementation**: `Calculator_impl` class implements the generated C++ interface.
*   **Event Loop**: `erpc_server_run` blocks indefinitely, processing incoming requests.

## Data Flow

1.  **User Input**: User types numbers into the Client's USB Serial Console.
2.  **Client App**: Parses input, calls `add(a, b)`.
3.  **eRPC Proxy**: Serializes arguments into a binary message.
4.  **Client Transport**: Sends binary data via `UART_DEV(1)` (P1.02).
5.  **Physical Link**: Data travels over wires to Server's P1.01.
6.  **Server Transport**: ISR receives byte -> Ring Buffer -> Wakes Server Thread.
7.  **eRPC Stub**: Deserializes message, identifies function `add`.
8.  **Server Service**: Executes `Calculator_impl::add(a, b)`.
9.  **Return Path**: Result is serialized and sent back via the reverse path.
