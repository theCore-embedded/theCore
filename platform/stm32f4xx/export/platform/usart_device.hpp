// TODO: use better name for this module
#ifndef USART_HANDLER_HPP
#define USART_HANDLER_HPP

#include <sys/usart_cfgs.h>

#include <stm32f4xx_usart.h>
#include <stm32f4xx_rcc.h>

#include <cstdint>
#include <unistd.h>


// TODO: make it singleton since each HW device must have dedicated driver
// instance.
// TODO: think about making driver config (USART_TypeDef&)
// reusable in runtime, not compile time
// TODO: replace USARTx with generic enumerator
template< UsartDevices USARTx >
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

	// Indicates that USART is ready for use
	int m_inited = 0;
	// Flag to prevent multiple opening
	int m_opened = 0;
};

// Interface
// -----------------------------------------------------------------------------

template< UsartDevices USARTx >
UsartHandler< USARTx >::UsartHandler()
{
}

template< UsartDevices USARTx >
UsartHandler< USARTx >::~UsartHandler()
{
	// TODO: implement
	if (m_inited) {
		// TODO
	}
}

template< UsartDevices USARTx >
int UsartHandler< USARTx >::init()
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

	return 0;
}

template< UsartDevices USARTx >
int UsartHandler< USARTx >::open()
{
	if (!m_inited || m_opened)
		return -1;

	constexpr auto usart = pickUSART();

	// Enable UART
	USART_Cmd(usart, ENABLE);
	m_opened = 1;
	return 0;
}

template< UsartDevices USARTx >
int UsartHandler< USARTx >::close()
{
	if (!m_opened)
		return -1;

	constexpr auto usart = pickUSART();

	// Disable UART
	USART_Cmd(usart, DISABLE);
	m_opened = 0;
	return 0;
}


template< UsartDevices USARTx >
ssize_t UsartHandler< USARTx >::read(uint8_t *data, size_t count)
{
	if (!count)
		return 0;

	if (!m_opened)
		return -1;

	constexpr auto usart = pickUSART();

	while (USART_GetFlagStatus(usart, USART_FLAG_RXNE) == RESET) { }

	// USART HW buffer is only one byte long
	*data = USART_ReceiveData(usart);

	return 1;
}

template< UsartDevices USARTx >
ssize_t UsartHandler< USARTx >::write(const uint8_t *data, size_t count)
{
	(void) count;

	if (!count)
		return 0;

	if (!m_opened)
		return -1;

	constexpr auto usart = pickUSART();

	while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET) { }

	// USART HW buffer is only one byte long
	USART_SendData(usart, *data);

	return 1;
}


// Private members
// -----------------------------------------------------------------------------
template< UsartDevices USARTx >
constexpr auto UsartHandler< USARTx >::pickRCC()
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

template< UsartDevices USARTx >
constexpr auto UsartHandler< USARTx >::pickRCC_fn()
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

template< UsartDevices USARTx >
constexpr auto UsartHandler< USARTx >::pickUSART()
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


#endif // USART_HANDLER_HPP
