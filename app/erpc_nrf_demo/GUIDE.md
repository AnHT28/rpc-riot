# User Guide: eRPC nRF52840DK Demo

This guide explains how to build, run, and use the eRPC Calculator demo on two nRF52840DK boards.

## Introduction
This application demonstrates a Remote Procedure Call (eRPC) system where a **Client** board asks a **Server** board to perform mathematical calculations (Add, Subtract, Multiply, Divide). The communication happens over a UART serial connection.

## Directory Structure
```
app/erpc_nrf_demo/
├── client/                 # Client application source code
│   ├── main.cpp           # Interactive CLI and eRPC calls
│   └── Makefile           # Client build configuration
├── server/                 # Server application source code
│   ├── main.cpp           # Service implementation
│   └── Makefile           # Server build configuration
├── erpc_nrf_demo_shared/   # Shared code
│   ├── riot_uart_transport.cpp # Custom UART transport for RIOT
│   ├── calculator.erpc    # IDL definition
│   └── ...                # Generated eRPC files
├── ARCHITECTURE.md         # System design documentation
├── CHECKLIST.md            # Deployment steps
└── GUIDE.md                # This file
```

## Getting Started

### 1. Hardware Setup
You need two nRF52840DK boards.
1.  **Connect Ground**: Connect a wire from **GND** on Board A to **GND** on Board B.
2.  **Connect Data**:
    *   Connect **P1.02** (TX) on Board A to **P1.01** (RX) on Board B.
    *   Connect **P1.01** (RX) on Board A to **P1.02** (TX) on Board B.
3.  **Connect USB**: Plug both boards into your computer.

### 2. Building and Flashing

**Note**: You need to know which serial port corresponds to which board (e.g., `/dev/ttyACM0`, `/dev/ttyACM1`).

#### Server
1.  Open a terminal in `app/erpc_nrf_demo/server`.
2.  Run:
    ```bash
    make BOARD=nrf52840dk flash
    ```
    *(Add `PORT=/dev/ttyACM...` if you have multiple boards connected)*

#### Client
1.  Open a terminal in `app/erpc_nrf_demo/client`.
2.  Run:
    ```bash
    make BOARD=nrf52840dk flash
    ```

### 3. Running the Demo

1.  Open a serial terminal for the **Client** board (115200 baud).
    *   Example: `pyterm -p /dev/ttyACM0 -b 115200`
2.  (Optional) Open a serial terminal for the **Server** board to see debug logs.
3.  Reset the Client board. You should see:
    ```
    eRPC NRF Client starting...
    eRPC Client initialized. Starting interactive loop...

    --- eRPC Calculator Client ---
    Enter value A:
    ```
4.  Type a number (e.g., `42`) and press **Enter**.
5.  The prompt `Enter value B:` will appear.
6.  Type another number (e.g., `10`) and press **Enter**.
7.  The Client will send the request to the Server and print the results:
    ```
    Calling add(42, 10)...
    Result: 52
    Calling multiply(42, 10)...
    Result: 420
    ```

## Troubleshooting

| Issue | Possible Cause | Solution |
| :--- | :--- | :--- |
| **"Hard Fault" / Crash** | Stack overflow | Ensure `CFLAGS += -DTHREAD_STACKSIZE_MAIN=2048` is in `client/Makefile`. |
| **Prompts appear late** | Output buffering | The code uses `\n` to flush buffers. Ensure your terminal handles newlines correctly. |
| **No Output / Hangs** | Wiring issue | Check that TX is connected to RX, not TX to TX. |
| **Garbage characters** | Baud rate mismatch | Ensure your terminal is set to **115200** baud. |

## Modifying the Service
If you want to add new functions (e.g., `modulo`):
1.  Edit `erpc_nrf_demo_shared/calculator.erpc`.
2.  Regenerate code using `erpcgen`.
3.  Implement the new function in `server/main.cpp` (`Calculator_impl` class).
4.  Call the new function in `client/main.cpp`.
5.  Rebuild both Client and Server.
