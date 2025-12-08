# eRPC Menu Demo on nRF52840-DK (UART)

This application demonstrates a Remote Procedure Call (RPC) system running on RIOT OS using the Embedded RPC (eRPC) framework. It implements a client-server architecture between two nRF52840-DK boards connected via UART.

## Overview

The demo provides a "Menu Service" where the client can request various operations to be performed by the server:
1.  **Integer Calculation**: Basic arithmetic (Add, Subtract, Multiply, Divide).
2.  **String Processing**: String manipulation (Remove spaces, To Upper, To Lower, Count digits).
3.  **Array Sorting**: Sorting an array of integers.

The communication happens over a dedicated UART interface (UART1), leaving the default UART0 available for the USB serial console/shell.

## Hardware Setup

You need two nRF52840-DK boards.

### Wiring (UART1 Cross-Connection)

Connect the UART1 pins between the two boards as follows:

| Signal | Board A (Server) | Board B (Client) | Pin Label on DK |
| :--- | :--- | :--- | :--- |
| **TX** | P1.02 | P1.01 | D1 -> D0 |
| **RX** | P1.01 | P1.02 | D0 -> D1 |
| **GND** | GND | GND | GND |

*Note: The default RIOT console (STDIO) uses the USB connection (UART0), so you can interact with both boards via their respective USB ports.*

## Directory Structure

```
app/erpc_menu_nrf_demo/
├── client/                 # Client application
│   ├── main.cpp           # Client logic, menu UI, and eRPC calls
│   ├── erpc_malloc.cpp    # Memory allocation wrappers
│   └── Makefile           # Client build configuration
├── server/                 # Server application
│   ├── main.cpp           # Server logic, service implementation
│   ├── erpc_malloc.cpp    # Memory allocation wrappers
│   └── Makefile           # Server build configuration
└── erpc_menu_nrf_shared/   # Shared code and generated files
    ├── *.erpc             # IDL definition
    ├── *_transport.*      # Custom RIOT UART transport
    ├── *_server.*         # Generated server shims
    ├── *_client.*         # Generated client shims
    └── Makefile           # Module configuration
```

## Building and Running

### Prerequisites
- RIOT OS environment set up.
- `arm-none-eabi-gcc` toolchain.
- Two nRF52840-DK boards connected via USB.

### 1. Flash the Server (Board A)
Open a terminal for Board A:
```bash
cd app/erpc_menu_nrf_demo/server
# Replace /dev/ttyACM0 with your Board A port
make BOARD=nrf52840dk PORT=/dev/ttyACM0 flash term
```

### 2. Flash the Client (Board B)
Open a terminal for Board B:
```bash
cd app/erpc_menu_nrf_demo/client
# Replace /dev/ttyACM1 with your Board B port
make BOARD=nrf52840dk PORT=/dev/ttyACM1 flash term
```

### 3. Usage
1.  Reset both boards.
2.  The Client will perform a startup connectivity test (2 + 2).
3.  If successful, the Client will display a menu.
4.  Enter a number (1-3) to select a function, then follow the prompts.
5.  The Client sends the request to the Server via UART.
6.  The Server processes the request and returns the result.
7.  The Client displays the result.

## Troubleshooting
- **Connection Failed**: Check wiring. Ensure TX goes to RX and RX goes to TX. Ensure common Ground.
- **No Output**: Ensure you are connecting to the correct USB COM ports.
