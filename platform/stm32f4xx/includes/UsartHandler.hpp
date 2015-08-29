// TODO: use better name for this module
#ifndef USART_HANDLER_HPP
#define USART_HANDLER_HPP

#include <stm32f4xx_usart.h>
#include <stm32f4xx_rcc.h>

#include <cstdint>
#include <unistd.h>

// TODO: make it singleton since each HW device must have dedicated driver
// instance.
// TODO: think about making driver config (USART_TypeDef&)
// reusable in runtime, not compile time
template< std::uintptr_t USARTx >
class UsartHandler
{
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

private:
	// Picks proper RCC in compile time
	static constexpr auto pickRCC();
	// Picks proper RCC operation function in compile time
	static constexpr auto pickRCC_fn();
	// Converts to proper USART type
	static constexpr auto pickUSART();
	//constexpr auto pickUSART(std::uintptr_t usart) const;

	// Indicates that USART is ready for use
	int m_inited = 0;
	// Flag to prevent multiple opening
	int m_opened = 0;
};

// Interface
// -----------------------------------------------------------------------------

template< std::uintptr_t USARTx >
UsartHandler< USARTx >::UsartHandler()
{
}

template< std::uintptr_t USARTx >
UsartHandler< USARTx >::~UsartHandler()
{
	// TODO: implement
	if (m_inited) {
		// TODO
	}
}

template< std::uintptr_t USARTx >
int UsartHandler< USARTx >::init()
{
	USART_InitTypeDef initStruct;

	// TODO: atomic cas?
	if (m_inited)
		return -1;

	// Must be optimized at compile time
	constexpr auto RCC_Periph = pickRCC();
	constexpr auto RCC_fn     = pickRCC_fn();

	// Enable peripheral clock
	RCC_fn(RCC_Periph, ENABLE);

	USART_StructInit(&initStruct);

	// Configure UART
	// TODO: make configuration values be chosen at compile time
	initStruct.USART_BaudRate             = 115200;
	initStruct.USART_WordLength           = USART_WordLength_8b;
	initStruct.USART_StopBits             = USART_StopBits_1;
	initStruct.USART_Parity               = USART_Parity_No;
	initStruct.USART_Mode                 = USART_Mode_Rx | USART_Mode_Tx;
	initStruct.USART_HardwareFlowControl  = USART_HardwareFlowControl_None;

	// Init UART
	USART_Init(pickUSART(), &initStruct);
	m_inited = 1;

	return 0;
}

template< std::uintptr_t USARTx >
int UsartHandler< USARTx >::open()
{
	if (!m_inited || m_opened)
		return -1;

	// Enable UART
	USART_Cmd(pickUSART(), ENABLE);
	m_opened = 1;
	return 0;
}

template< std::uintptr_t USARTx >
ssize_t UsartHandler< USARTx >::read(uint8_t *data, size_t count)
{
	// Not used

	if (!count)
		return 0;

	if (!m_opened)
		return -1;

	while (USART_GetFlagStatus(pickUSART(), USART_FLAG_RXNE) == RESET) { }

	// USART HW buffer is only one byte long
	*data = USART_ReceiveData(pickUSART());

	return 1;
}

template< std::uintptr_t USARTx >
ssize_t UsartHandler< USARTx >::write(const uint8_t *data, size_t count)
{
	(void) count;

	if (!count)
		return 0;

	if (!m_opened)
		return -1;

	while (USART_GetFlagStatus(pickUSART(), USART_FLAG_TXE) == RESET) { }

	// USART HW buffer is only one byte long
	USART_SendData(pickUSART(), *data);

	return 1;
}


// Private members
// -----------------------------------------------------------------------------
template< std::uintptr_t USARTx >
constexpr auto UsartHandler< USARTx >::pickRCC()
{
	// USART1 and USART6 are on APB2
	// USART2, USART3, UART4, UART5 are on APB1
	// See datasheet for detailed explanations
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
		return RCC_APB1Periph_USART2;
	if (usart == USART6)
		return RCC_APB2Periph_USART6;
}

template< std::uintptr_t USARTx >
constexpr auto UsartHandler< USARTx >::pickRCC_fn()
{
	// USART1 and USART6 are on APB2
	// USART2, USART3, UART4, UART5 are on APB1
	// See datasheet for detailed explanations
	constexpr USART_TypeDef *usart = (USART_TypeDef *) USARTx;
	if (usart == USART1 || usart == USART6) {
		return RCC_APB2PeriphClockCmd;
	} else if (usart == USART2 || usart == USART3 ||
			   usart == UART4  || usart == USART3 ||
			   usart == UART5) {
		return RCC_APB1PeriphClockCmd;
	}
}

template< std::uintptr_t USARTx >
constexpr auto UsartHandler< USARTx >::pickUSART()
{
	return (USART_TypeDef *) USARTx;
}


#endif // USART_HANDLER_HPP
