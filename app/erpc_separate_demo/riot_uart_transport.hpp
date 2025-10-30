#ifndef _RIOT_UART_TRANSPORT_HPP_
#define _RIOT_UART_TRANSPORT_HPP_

#include "erpc_framed_transport.hpp"
#include "erpc_message_buffer.hpp"
#include "periph/uart.h"

/*!
 * @brief RIOT UART transport that uses UART peripheral driver.
 */
class RiotUartTransport : public erpc::FramedTransport {
public:
    RiotUartTransport(uart_t uart_dev);
    virtual ~RiotUartTransport();

    /*!
     * @brief Initialize UART peripheral configuration structure with values specified in RiotUartTransport constructor.
     *
     * @retval kErpcStatus_Success When init function was executed successfully.
     * @retval kErpcStatus_InitFailed When init function wasn't executed successfully.
     */
    virtual erpc_status_t init(void);

protected:
    virtual erpc_status_t underlyingReceive(uint8_t *data, uint32_t size);
    virtual erpc_status_t underlyingSend(const uint8_t *data, uint32_t size);

private:
    uart_t m_uart_dev; /*!< UART device to use */
};

extern "C" {
/*!
 * @brief Create a UART transport.
 *
 * @param[in] uart_dev UART device to use
 *
 * @return Pointer to created UART transport or NULL if an error occurred.
 */
void *erpc_uart_transport_init(uart_t uart_dev);
}

#endif /* _RIOT_UART_TRANSPORT_HPP_ */