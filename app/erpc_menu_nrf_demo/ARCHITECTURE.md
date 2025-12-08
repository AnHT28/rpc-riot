# System Architecture

This document describes the software architecture and design decisions for the eRPC Menu Demo on RIOT OS.

## System Design

The system follows a classic RPC Client-Server model adapted for embedded constraints.

```mermaid
graph LR
    subgraph "Client Node (nRF52840)"
        UI[User Interface (UART0/USB)]
        AppC[Client App]
        Proxy[Generated Proxy]
        TransC[UART Transport]
    end

    subgraph "Server Node (nRF52840)"
        TransS[UART Transport]
        Stub[Generated Stub]
        Impl[Service Implementation]
        Log[Logger (UART0/USB)]
    end

    UI --> AppC
    AppC --> Proxy
    Proxy --> TransC
    TransC <==>|UART1 (P1.01/P1.02)| TransS
    TransS --> Stub
    Stub --> Impl
    Impl --> Log
```

## Components

### 1. Application Layer
- **Client (`client/main.cpp`)**: 
  - Handles user input via `stdin` (USB Console).
  - Manages the application state machine.
  - Invokes remote methods as if they were local function calls.
- **Server (`server/main.cpp`)**: 
  - Initializes the eRPC server loop.
  - Implements the `DemoService` interface.
  - Executes the actual business logic (Calculation, String Processing, Sorting).

### 2. eRPC Layer (Middleware)
- **IDL (`demo_service.erpc`)**: Defines the interface contract.
  - `calculate(int32, int32, enum) -> struct`
  - `processString(string, enum) -> struct`
  - `sortArray(list<int32>) -> list<int32>`
- **Generated Shims**:
  - **Client Proxy**: Serializes function calls into binary messages.
  - **Server Stub**: Deserializes messages and dispatches them to the implementation.
- **Codec**: Uses `BasicCodec` for binary serialization.

### 3. Transport Layer (`riot_uart_transport`)
A custom transport implementation for RIOT OS:
- **Hardware**: Uses `periph_uart` driver.
- **Interrupt Driven**: Uses `tsrb` (Thread Safe Ring Buffer) and `sema` (Semaphores) to handle incoming data asynchronously without blocking the CPU in busy loops.
- **Framing**: Inherits from `FramedTransport` to handle message packetization (Header + CRC).

### 4. Memory Management
- **Dynamic Allocation**: Uses `erpc_malloc` wrappers around standard `malloc`/`free`.
- **Policy**: `ERPC_ALLOCATION_POLICY_DYNAMIC` is enforced to handle variable-length data (strings, arrays) efficiently.

## Integration with RIOT OS

| Feature | RIOT Module | Usage |
| :--- | :--- | :--- |
| **Threading** | `core` | Server runs in main thread; Client blocks on RPC calls. |
| **Timing** | `xtimer` | Used for timeouts and delays. |
| **IO** | `periph_uart` | Low-level UART communication. |
| **Synchronization** | `sema` | Signaling between UART ISR and Transport thread. |
| **Data Structures** | `tsrb` | Buffering RX data from ISR. |

## Data Flow

1.  **Request**: Client App calls `add(2, 3)`.
2.  **Serialization**: Proxy marshals `2`, `3`, and `OP_ADD` into a buffer.
3.  **Transmission**: Transport sends buffer via UART1 TX.
4.  **Reception**: Server UART1 RX ISR receives bytes -> Ring Buffer -> Semaphore signal.
5.  **Processing**: Server thread wakes up, reads buffer, unmarshals data.
6.  **Execution**: Server calls `impl->add(2, 3)`.
7.  **Response**: Result `5` is marshaled and sent back via UART1 TX.
8.  **Completion**: Client receives response, unmarshals `5`, and returns to App.
