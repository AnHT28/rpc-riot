# Deployment Checklist

Use this checklist to ensure a successful deployment of the `erpc_nrf_demo`.

## 1. Hardware Preparation
- [ ] **Board 1 (Client)**: nRF52840DK connected via USB.
- [ ] **Board 2 (Server)**: nRF52840DK connected via USB.
- [ ] **Wiring**:
    - [ ] Client P1.02 (TX) connected to Server P1.01 (RX).
    - [ ] Client P1.01 (RX) connected to Server P1.02 (TX).
    - [ ] Common Ground (GND) connected between boards.
- [ ] **Identification**: Note the serial ports (e.g., `/dev/ttyACM0`, `/dev/ttyACM1`).

## 2. Environment Setup
- [ ] RIOT OS repository is cloned and `RIOTBASE` is set (or relative paths in Makefile are correct).
- [ ] ARM GCC Toolchain is installed and in `PATH`.
- [ ] `openocd` is installed for flashing.

## 3. Server Deployment
- [ ] Navigate to server directory: `cd app/erpc_nrf_demo/server`
- [ ] Clean build: `make clean`
- [ ] Build and Flash:
    ```bash
    make BOARD=nrf52840dk PORT=/dev/ttyACM2 flash
    ```
    *(Replace `/dev/ttyACM2` with your Server's port)*
- [ ] Verify Flashing: Look for "Verified ... bytes" and "Done flashing".

## 4. Client Deployment
- [ ] Navigate to client directory: `cd app/erpc_nrf_demo/client`
- [ ] Clean build: `make clean`
- [ ] Build and Flash:
    ```bash
    make BOARD=nrf52840dk PORT=/dev/ttyACM0 flash
    ```
    *(Replace `/dev/ttyACM0` with your Client's port)*
- [ ] Verify Flashing: Look for "Verified ... bytes" and "Done flashing".

## 5. Verification & Testing
- [ ] Open Server Terminal:
    ```bash
    pyterm -p /dev/ttyACM2 -b 115200
    ```
- [ ] Open Client Terminal:
    ```bash
    pyterm -p /dev/ttyACM0 -b 115200
    ```
- [ ] **Reset Boards**: Press the RESET button on both boards.
- [ ] **Check Server Output**: Should see:
    ```
    eRPC NRF Server starting...
    Server initialized. Running...
    ```
- [ ] **Check Client Output**: Should see:
    ```
    eRPC NRF Client starting...
    eRPC Client initialized. Starting interactive loop...
    --- eRPC Calculator Client ---
    Enter value A:
    ```
- [ ] **Perform Test**:
    1.  Enter `10` on Client.
    2.  Enter `5` on Client.
    3.  Verify Client Output: `Result: 15`, `Result: 50`.
    4.  Verify Server Output: `Server: add(10, 5)`, `Server: multiply(10, 5)`.

## 6. Troubleshooting
- [ ] **Infinite Loop on Input**: Ensure you are sending a newline character (Enter key).
- [ ] **No Response**: Check wiring (TX/RX swapped?).
- [ ] **Hard Fault**: Ensure `THREAD_STACKSIZE_MAIN` is at least 2048 in `client/Makefile`.
