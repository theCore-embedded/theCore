#ifndef SYS_USART_CFGS_H
#define SYS_USART_CFGS_H

#include <cstdint>

// Represents distinct peripheral devices
enum class UsartDevices
{
	DEV_1,
	DEV_2,
	DEV_3,
	DEV_4,
	DEV_5,
	DEV_6,
	DEV_7,
	DEV_8
};

// Provides general UART state bits
// TODO: extend
// TODO: substitude constexpr to something better
namespace UsartState
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

enum class UsartMode
{
	POLL,
	IRQ,
	DMA,		// Implies IRQ
	DMA_NO_IRQ	// DMA without IRQ
};

#endif // SYS_USART_CFGS_H

