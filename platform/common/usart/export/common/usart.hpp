#ifndef COMMON_USART_HPP_
#define COMMON_USART_HPP_

#include <cstdint>

// Represents distinct peripheral devices
enum class usart_device
{
    dev_1,
    dev_2,
    dev_3,
    dev_4,
    dev_5,
    dev_6,
    dev_7,
    dev_8
};

// Provides general UART state bits
// TODO: extend
// TODO: substitude constexpr to something better
namespace USART_state
{
constexpr int32_t INVALID	=	-1;			// Invalid state
constexpr int32_t EMPTY		=	0;			// No state
constexpr int32_t ERROR		=	1 << 0;		// General error
constexpr int32_t ORE		=	1 << 1;		// Overrun
constexpr int32_t PE		=	1 << 2;		// Parity error
constexpr int32_t FE		=	1 << 3;		// Frame error
constexpr int32_t NE		=	1 << 4;		// Noise error
constexpr int32_t TXC		=	1 << 5;		// Transmit complete
constexpr int32_t RXP		=	1 << 6;		// Receive pending
}

enum class USART_mode
{
    poll,
    IRQ,
    DMA,		// Implies IRQ
    DMA_no_IRQ	// DMA without IRQ
};

#endif // COMMON_USART_HPP_

