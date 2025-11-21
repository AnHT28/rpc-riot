# eRPC nRF52840DK Demo

This demo shows how to run eRPC between two nRF52840DK boards using UART.

## Structure

- `client/`: Client application (runs on Board B)
- `server/`: Server application (runs on Board A)
- `shared/`: Shared eRPC generated files and transport implementation

## Hardware Setup

You need two nRF52840DK boards.

**Wiring:**

|  Signal  | Board A (Server) | Board B (Client) |
|----------|------------------|------------------|
| TX -> RX | P1.02 (UART1 TX) | P1.01 (UART1 RX) |
| RX <- TX | P1.01 (UART1 RX) | P1.02 (UART1 TX) |
| GND      | GND              | GND              |

**Note:** UART0 (USB Virtual COM) is used for logging (printf). UART1 is used for eRPC transport.

## Building and Flashing

**Server (Board A):**
```bash
cd server
make BOARD=nrf52840dk flash term
```
make -C app/erpc_nrf_demo/server BOARD=nrf52840dk PORT=/dev/ttyACM0 term

**Client (Board B):**
```bash
cd client
make BOARD=nrf52840dk flash term
```
make -C app/erpc_nrf_demo/client BOARD=nrf52840dk PORT=/dev/ttyACM3 term

## Implementation Details

- **Transport:** `riot_uart_transport.cpp` implements `erpc::FramedTransport` using RIOT's `periph_uart`.
- **Buffering:** Uses `tsrb` (Thread Safe Ring Buffer) and `sema` (Semaphore) to handle UART interrupts and blocking reads.
- **Service:** The Calculator service is used as an example.
