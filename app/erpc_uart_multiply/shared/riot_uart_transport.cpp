// riot_uart_transport.cpp — blocking UART FramedTransport for RIOT native + nRF52
extern "C" {
#include "periph/uart.h"
}
#include "erpc_framed_transport.hpp"
#include <cstdint>
#include <cstdio>

#ifdef __linux__
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

using namespace erpc;

namespace {

speed_t _baud_to_speed(uint32_t baud)
{
    switch (baud) {
    case 9600: return B9600;
    case 19200: return B19200;
    case 38400: return B38400;
    case 57600: return B57600;
    case 115200: return B115200;
    default: return 0;
    }
}

class RiotUartTransport : public FramedTransport {
public:
    explicit RiotUartTransport(int fd) : _fd(fd) {}

    erpc_status_t init()
    {
        return (_fd >= 0) ? kErpcStatus_Success : kErpcStatus_Fail;
    }

    ~RiotUartTransport() override
    {
        if (_fd >= 0) {
            ::close(_fd);
        }
    }

protected:
    erpc_status_t underlyingSend(const uint8_t *data, uint32_t size) override
    {
        const uint8_t *cursor = data;
        uint32_t remaining = size;
        while (remaining) {
            ssize_t written = ::write(_fd, cursor, remaining);
            if (written <= 0) {
                std::fprintf(stderr, "[transport] write failed: %s\n",
                             std::strerror(errno));
                return kErpcStatus_Fail;
            }
            cursor += static_cast<size_t>(written);
            remaining -= static_cast<uint32_t>(written);
        }
        return kErpcStatus_Success;
    }

    erpc_status_t underlyingReceive(uint8_t *data, uint32_t size) override
    {
        uint8_t *cursor = data;
        uint32_t remaining = size;
        while (remaining) {
            ssize_t got = ::read(_fd, cursor, remaining);
            if (got <= 0) {
                std::fprintf(stderr, "[transport] read failed: %s\n",
                             (got == 0) ? "EOF" : std::strerror(errno));
                return kErpcStatus_Fail;
            }
            cursor += static_cast<size_t>(got);
            remaining -= static_cast<uint32_t>(got);
        }
        return kErpcStatus_Success;
    }

private:
    int _fd;
};

int _open_serial_fd(const char *path, uint32_t baud)
{
    if (!path) {
        std::fprintf(stderr, "[transport] ERPC_HOST_TTY not set\n");
        return -1;
    }

    int fd = ::open(path, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        std::fprintf(stderr, "[transport] open('%s') failed: %s\n",
                     path, std::strerror(errno));
        return -1;
    }

    termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        std::fprintf(stderr, "[transport] tcgetattr failed: %s\n",
                     std::strerror(errno));
        ::close(fd);
        return -1;
    }

    speed_t speed = _baud_to_speed(baud);
    if (speed == 0) {
        std::fprintf(stderr, "[transport] unsupported baud %u\n", baud);
        ::close(fd);
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP |
                     INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD | CSTOPB | CRTSCTS);
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::fprintf(stderr, "[transport] tcsetattr failed: %s\n",
                     std::strerror(errno));
        ::close(fd);
        return -1;
    }

    tcflush(fd, TCIOFLUSH);
    return fd;
}

} // namespace

extern "C" void *riot_uart_transport_create(uart_t, uint32_t baud)
{
    const char *path = std::getenv("ERPC_HOST_TTY");
    int fd = _open_serial_fd(path, baud);
    if (fd < 0) {
        return nullptr;
    }

    auto *t = new RiotUartTransport(fd);
    if (!t) {
        ::close(fd);
        return nullptr;
    }
    if (t->init() != kErpcStatus_Success) {
        delete t;
        return nullptr;
    }
    return t;
}

#else

extern "C" {
#include "xtimer.h"
#include "thread.h"
#include "mutex.h"
}

using namespace erpc;

// Simple UART receive buffer for eRPC
struct UartBuffer {
    uint8_t data[256];
    volatile size_t write_pos;
    volatile size_t read_pos;
    mutex_t mutex;
};

static UartBuffer uart_rx_buffer = {.write_pos = 0, .read_pos = 0};

// UART receive callback for RIOT
static void uart_rx_callback(void *arg, uint8_t data) {
    (void)arg;
    mutex_lock(&uart_rx_buffer.mutex);
    uart_rx_buffer.data[uart_rx_buffer.write_pos] = data;
    uart_rx_buffer.write_pos = (uart_rx_buffer.write_pos + 1) % sizeof(uart_rx_buffer.data);
    mutex_unlock(&uart_rx_buffer.mutex);
}

class RiotUartTransport : public FramedTransport {
public:
    RiotUartTransport(uart_t dev, uint32_t baud) : _dev(dev), _baud(baud) {}

    erpc_status_t init() {
        mutex_init(&uart_rx_buffer.mutex);
        uart_rx_buffer.write_pos = 0;
        uart_rx_buffer.read_pos = 0;
        
        int rc = uart_init(_dev, _baud, uart_rx_callback, nullptr);
        return (rc == 0) ? kErpcStatus_Success : kErpcStatus_Fail;
    }

protected:
    erpc_status_t underlyingSend(const uint8_t *data, uint32_t size) override {
        uart_write(_dev, data, size);
        return kErpcStatus_Success;
    }

    erpc_status_t underlyingReceive(uint8_t *data, uint32_t size) override {
        for (uint32_t i = 0; i < size; ++i) {
            // Wait for data in buffer
            while (uart_rx_buffer.read_pos == uart_rx_buffer.write_pos) {
                xtimer_usleep(1000); // 1ms delay
            }
            
            mutex_lock(&uart_rx_buffer.mutex);
            data[i] = uart_rx_buffer.data[uart_rx_buffer.read_pos];
            uart_rx_buffer.read_pos = (uart_rx_buffer.read_pos + 1) % sizeof(uart_rx_buffer.data);
            mutex_unlock(&uart_rx_buffer.mutex);
        }
        return kErpcStatus_Success;
    }

private:
    uart_t   _dev;
    uint32_t _baud;
};

extern "C" void *riot_uart_transport_create(uart_t dev, uint32_t baud) {
    auto *t = new RiotUartTransport(dev, baud);
    return (t && t->init() == kErpcStatus_Success) ? t : nullptr;
}

#endif
