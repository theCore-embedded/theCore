// TODO: use better name for this module
#ifndef USART_HANDLER_HPP
#define USART_HANDLER_HPP

#include <sys/usart_cfgs.h>

#include <stm32f4xx_usart.h>
#include <stm32f4xx_rcc.h>

#include <cstdint>
#include <unistd.h>

#include <functional>
#include <platform/irq_manager.hpp>

// TODO: make it singleton since each HW device must have dedicated driver
// instance.
// TODO: think about making driver config (USART_TypeDef&)
// reusable in runtime, not compile time
template< UsartDevices	USARTx,
		  UsartMode		mode		= UsartMode::POLL,
		  int32_t		flags		= UsartState::EMPTY >
class UsartHandler
{
	// Supported only IRQ and POLL modes.
	static_assert((mode == UsartMode::POLL) ||
				  (mode == UsartMode::IRQ), "Invalid USART mode used!");
public:
	UsartHandler();
	~UsartHandler();

	// Lazy initialization, -1 if error. 0 otherwise
	int init();

	// -1 if error, 0 otherwise
	int open();
	// -1 if error, 0 otherwise
	int close();

	// -1 if error, [0, count] otherwise
	ssize_t write(const uint8_t *data, size_t count);

	// -1 if error, [0, count] otherwise
	ssize_t read(uint8_t *data, size_t count);

	// Queries peripheral status
	// INVALID if method failed, anything else from UsartState otherwise
	int32_t getState() const;

	// Clears state
	// If nothing is given then whole state is cleared
	int clearState(int32_t state);
	int clearState();

	// Set of routines to work with IRQs
	// -1 if error, 0 otherwise
	int registerIRQ(const std::function< void() > &handler);

	// Completes IRQ for given status bits
	// If nothing is given then all pending interrupts are dropped
	// -1 if error, 0 otherwise
	int completeIRQ(int32_t state);
	int completeIRQ();

	// -1 if error, 0 otherwise
	int deregisterIRQ();

private:
	// Picks proper RCC at compile time
	static constexpr auto pickRCC();
	// Picks proper RCC operation function at compile time
	static constexpr auto pickRCC_fn();
	// Picks IRQ number at compile time
	static constexpr auto pickIRQn();
	// Converts to proper USART type
	static constexpr auto pickUSART();

	// Convinient method
	static void initIRQ();

	// Indicates that USART is ready for use
	int m_inited = 0;
	// Flag to prevent multiple opening
	int m_opened = 0;
};

// Interface
// -----------------------------------------------------------------------------

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
UsartHandler< USARTx, mode, flags >::UsartHandler()
{
}

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
UsartHandler< USARTx, mode, flags >::~UsartHandler()
{
	// TODO: implement
	if (m_inited) {
		// TODO
	}
}

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
int UsartHandler< USARTx, mode, flags >::init()
{
	USART_InitTypeDef initStruct;

	// TODO: atomic cas?
	if (m_inited)
		return -1;

	// Must be optimized at compile time
	constexpr auto RCC_Periph = pickRCC();
	constexpr auto RCC_fn     = pickRCC_fn();
	constexpr auto usart      = pickUSART();

	// Enable peripheral clock
	RCC_fn(RCC_Periph, ENABLE);

	// Configure UART
	// TODO: make configuration values be chosen at compile time
	initStruct.USART_BaudRate             = 115200;
	initStruct.USART_WordLength           = USART_WordLength_8b;
	initStruct.USART_StopBits             = USART_StopBits_1;
	initStruct.USART_Parity               = USART_Parity_No;
	initStruct.USART_Mode                 = USART_Mode_Rx | USART_Mode_Tx;
	initStruct.USART_HardwareFlowControl  = USART_HardwareFlowControl_None;

	// Init UART
	USART_Init(usart, &initStruct);
	m_inited = 1;

	// TODO: disable UART interrupts in NVIC

	// TODO: add static assert for supported modes and flags
	// Intended to be optimized at compile time

	if (mode == UsartMode::IRQ) {
		initIRQ();
	}

	return 0;
}

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
int UsartHandler< USARTx, mode, flags >::open()
{
	if (!m_inited || m_opened)
		return -1;

	constexpr auto usart = pickUSART();

	// Enable UART
	USART_Cmd(usart, ENABLE);
	m_opened = 1;
	return 0;
}

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
int UsartHandler< USARTx, mode, flags >::close()
{
	if (!m_opened)
		return -1;

	constexpr auto usart = pickUSART();

	// Disable UART
	USART_Cmd(usart, DISABLE);
	m_opened = 0;
	return 0;
}


template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
ssize_t UsartHandler< USARTx, mode, flags >::read(uint8_t *data, size_t count)
{
	if (!count)
		return 0;

	if (!m_opened)
		return -1;

	// TODO: implement support for IRQ mode

	constexpr auto usart = pickUSART();

	while (USART_GetFlagStatus(usart, USART_FLAG_RXNE) == RESET) {
		if (mode == UsartMode::IRQ) {
			// Do not wait anything in async mode.
			return -2;
		}
	}

	// USART HW buffer is only one byte long
	*data = USART_ReceiveData(usart);

	return 1;
}

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
ssize_t UsartHandler< USARTx, mode, flags >::write(const uint8_t *data, size_t count)
{
	if (!count)
		return 0;

	if (!m_opened)
		return -1;

	// TODO: implement support for IRQ mode

	constexpr auto usart = pickUSART();

	while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET) {
		if (mode == UsartMode::IRQ) {
			// Do not wait anything in async mode.
			return -2;
		}
	}

	// USART HW buffer is only one byte long
	USART_SendData(usart, *data);

	return 1;
}

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
int32_t UsartHandler< USARTx, mode, flags >::getState() const
{
	constexpr auto usart = pickUSART();

	int32_t status = UsartState::EMPTY;

	if (USART_GetFlagStatus(usart, USART_FLAG_TXE) == SET) {
		status |= UsartState::TXC;
	}

	if (USART_GetFlagStatus(usart, USART_FLAG_RXNE) == SET) {
		status |= UsartState::RXP;
	}

	if (USART_GetFlagStatus(usart, USART_FLAG_ORE) == SET) {
		status |= UsartState::ORE;
	}

	if (USART_GetFlagStatus(usart, USART_FLAG_NE) == SET) {
		status |= UsartState::NE;
	}

	if (USART_GetFlagStatus(usart, USART_FLAG_FE) == SET) {
		status |= UsartState::FE;
	}

	if (USART_GetFlagStatus(usart, USART_FLAG_PE) == SET) {
		status |= UsartState::PE;
	}

	return status;
}

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
int UsartHandler< USARTx, mode, flags >::clearState(int32_t state)
{
	constexpr auto usart = pickUSART();

	// TODO: make sure that state is cleared even if
	// 'special' software sequence is required.
	// See 'USART_ClearFlag()'
	if (state & UsartState::ERROR) {
		// Clear all errors
	}

	if (state & UsartState::RXP) {
		USART_ClearFlag(usart, USART_FLAG_RXNE);
	}

	if (state & UsartState::TXC) {
		USART_ClearFlag(usart, USART_FLAG_TC);
	}

	return 0;
}


template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
int UsartHandler< USARTx, mode, flags >::registerIRQ(const std::function< void() > &handler)
{
	// TODO
	constexpr auto IRQn = pickIRQn();
	IRQ_Manager::subscribe(IRQn, handler);
	IRQ_Manager::unmask(IRQn);
	return 0;
}


template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
int UsartHandler< USARTx, mode, flags >::completeIRQ(int32_t state)
{
	constexpr auto usart = pickUSART();
	constexpr auto IRQn = pickIRQn();

	// TODO: make sure that state is cleared even if
	// 'special' software sequence is required.
	// See 'USART_ClearITPendingBit()'

	if (state & UsartState::ERROR) {
		// Clear all errors
	}

	if (state & UsartState::RXP) {
		USART_ClearITPendingBit(usart, USART_IT_RXNE);
	}

	if (state & UsartState::TXC) {
		USART_ClearITPendingBit(usart, USART_IT_TC);
	}

	IRQ_Manager::clear(IRQn);
	IRQ_Manager::unmask(IRQn);

	return 0;
}

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
int UsartHandler< USARTx, mode, flags >::completeIRQ()
{
	return this->completeIRQ(UsartState::INVALID);
}


template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
int UsartHandler< USARTx, mode, flags >::deregisterIRQ()
{
	constexpr auto IRQn = pickIRQn();
	IRQ_Manager::mask(IRQn);
	IRQ_Manager::unsubscribe(IRQn);

	return -1;
}

// Private members
// -----------------------------------------------------------------------------
template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
constexpr auto UsartHandler< USARTx, mode, flags >::pickRCC()
{
	// USART1 and USART6 are on APB2
	// USART2, USART3, UART4, UART5 are on APB1
	// See datasheet for detailed explanations
	// TODO: Switch instead of 'if's
	constexpr auto usart = pickUSART();
	if (usart == USART1)
		return RCC_APB2Periph_USART1;
	if (usart == USART2)
		return RCC_APB1Periph_USART2;
	if (usart == USART3)
		return RCC_APB1Periph_USART3;
	if (usart == UART4)
		return RCC_APB1Periph_UART4;
	if (usart == UART5)
		return RCC_APB1Periph_UART5;
	if (usart == USART6)
		return RCC_APB2Periph_USART6;

	return static_cast< uint32_t >(-1);
}

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
constexpr auto UsartHandler< USARTx, mode, flags >::pickRCC_fn()
{
	// USART1 and USART6 are on APB2
	// USART2, USART3, UART4, UART5 are on APB1
	// See datasheet for detailed explanations
	constexpr USART_TypeDef *usart = pickUSART();
	// TODO: switch instead of 'if's
	if (usart == USART1 || usart == USART6) {
		return RCC_APB2PeriphClockCmd;
	} else if (usart == USART2 || usart == USART3 ||
			   usart == UART4  || usart == UART5) {
		return RCC_APB1PeriphClockCmd;
	} else {
		// TODO: clarify
		return static_cast< decltype(&RCC_APB1PeriphClockCmd) >(nullptr);
	}
}

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
constexpr auto UsartHandler< USARTx, mode, flags >::pickIRQn()
{
	constexpr auto usart = pickUSART();

	if (usart == USART1) {
		return USART1_IRQn;
	} else if (usart == USART2) {
		return USART2_IRQn;
	} else if (usart == USART3) {
		return USART3_IRQn;
	} else if (usart == UART4) {
		return UART4_IRQn;
	} else if (usart == UART5) {
		return UART5_IRQn;
	} else if (usart == USART6) {
		return USART6_IRQn;
	}
}

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
constexpr auto UsartHandler< USARTx, mode, flags >::pickUSART()
{
	switch (USARTx) {
	case UsartDevices::DEV_1:
		return USART1;
	case UsartDevices::DEV_2:
		return USART2;
	case UsartDevices::DEV_3:
		return USART3;
	case UsartDevices::DEV_4:
		return UART4;
	case UsartDevices::DEV_5:
		return UART5;
	case UsartDevices::DEV_6:
		return USART6;
	// TODO: clarify
	default:
		return static_cast< decltype(USART1) >(nullptr);
	};
}

template< UsartDevices USARTx,
		  UsartMode mode,
		  int32_t flags >
void UsartHandler< USARTx, mode, flags >::initIRQ()
{
	constexpr auto usart = pickUSART();

	if ((flags & UsartState::ERROR)
			|| (flags & UsartState::FE)
			|| (flags & UsartState::NE)
			|| (flags & UsartState::ORE)) {

		USART_ITConfig(usart, USART_IT_ERR, ENABLE);
	}

	if (flags & UsartState::TXC) {
		USART_ITConfig(usart, USART_IT_TXE, ENABLE);
	}

	if (flags & UsartState::RXP) {
		USART_ITConfig(usart, USART_IT_RXNE, ENABLE);
	}
}

#endif // USART_HANDLER_HPP
