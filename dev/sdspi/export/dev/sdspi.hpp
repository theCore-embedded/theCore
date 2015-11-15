#ifndef DEV_SDSPI_HPP
#define DEV_SDSPI_HPP

#include <algorithm>
#include <target/usart.hpp>
#include <ecl/iostream.hpp>

// TODO: mention about 8 additional clocks before each command!!!
template< class SPI_dev, class GPIO_CS >
class SD_SPI
{
public:
	SD_SPI();
	~SD_SPI();

	// Lazy initialization, -1 if error. 0 otherwise
	int init();

	// -1 if error, 0 otherwise
	int open();
	// -1 if error, 0 otherwise
	int close();

	// < count if error, count otherwise
	ssize_t write(const uint8_t *data, size_t count);
	// -1 if error, [0, count] otherwise
	ssize_t read(uint8_t *data, size_t count);

	// TODO: extend it with SEEK_CUR and SEEK_END
	off_t seek(off_t offset /*, SEEK_SET */);

	// Tell current position
	// -1 if error, valid offset otherwise
	off_t tell() const;

private:
	using DMA_rx = typename SPI_dev::DMA_RX_t;
	using DMA_tx = typename SPI_dev::DMA_TX_t;

	struct R1
	{
		uint8_t     response;
	};

	struct R3
	{
		R1          r1;
		uint8_t     OCR[4];
	};

	using R7 = R3; // TODO: clarify

	// http://elm-chan.org/docs/mmc/mmc_e.html
	// SD SPI command set ------------------------------------------------------

	// GO_IDLE_STATE            - Software reset.
	int CMD0(R1 &r);

	// SEND_OP_COND             - Initiate initialization process.
	int CMD1(R1 &r);

	// APP_SEND_OP_COND         - For only SDC. Initiate initialization process.
	int ACMD41(R1 &r, bool HCS);

	// SEND_IF_COND             - For only SDC V2. Check voltage range.
	int CMD8(R7 &r, uint8_t VHS, uint8_t crc);

	// SEND_CSD                 - Read CSD register.
	int CMD9(R1 &r, uint8_t *buf, size_t size);

	// SEND_CID                 - Read CID register.
	int CMD10(R1 &r, uint8_t *buf, size_t size);

	// STOP_TRANSMISSION        - Stop to read data.
	// int CMD12(R1 &r); R1b is required here

	// SET_BLOCKLEN             - Change R/W block size.
	int CMD16(R1 &r, uint32_t block_length);

	// READ_SINGLE_BLOCK        - Read a block.
	int CMD17(R1 &r, uint32_t address, uint8_t *buf, size_t size);

	// READ_MULTIPLE_BLOCK      - Read multiple blocks.
	int CMD18(R1 &r, uint32_t address, uint8_t *buf, size_t size);

	// SET_BLOCK_COUNT          - For only MMC. Define number of blocks to transfer
	//                            with next multi-block read/write command.
	int CMD23(R1 &r, uint16_t block_count);

	// SET_WR_BLOCK_ERASE_COUNT - For only SDC. Define number of blocks to pre-erase
	//                              with next multi-block write command.
	int ACMD23(R1 &r, uint16_t block_count);

	// WRITE_BLOCK              - Write a block.
	int CMD24(R1 &r, uint32_t address, uint8_t *buf, size_t size);

	// WRITE_MULTIPLE_BLOCK     - Write multiple blocks.
	int CMD25(R1 &r, uint32_t address, uint8_t *buf, size_t size);

	// APP_CMD                  - Leading command of ACMD<n> command.
	int CMD55(R1 &r);

	// READ_OCR                 - Read OCR.
	int CMD58(R3 &r);

	// Sends >= 47 empty clocks to initialize the device
	// int send_init();

	// Useful abstractions
	int receive_R1(R1 &r);
	int receive_R3(R3 &r);

	ssize_t send(const uint8_t *buf, size_t size);
	ssize_t receive(uint8_t *buf, size_t size);


	SPI_dev m_spi;
};

template< class SPI_dev, class GPIO_CS >
SD_SPI< SPI_dev, GPIO_CS >::SD_SPI()
{
}

template< class SPI_dev, class GPIO_CS >
SD_SPI< SPI_dev, GPIO_CS >::~SD_SPI()
{
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::init()
{
	m_spi.init();

	m_spi.lock();
	GPIO_CS::set();

	m_spi.unlock();

	return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::open()
{
	m_spi.open();
	m_spi.lock();

	// TODO: place >= 47 clocks in init()
	uint8_t buf[100];
	std::fill_n(buf, sizeof(buf), 0xff);

	send(buf, sizeof(buf));

	R1 r1;
	R3 r3;
	R7 r7;
	//int rc;

	CMD0(r1);

	if (r1.response == 0xff) {
		ecl::cout << "Response wasn't received" << ecl::endl;
		return -1;
	} else if (r1.response == 0x1) {
		ecl::cout << "Response OK" << ecl::endl;
	} else {
		ecl::cout << "Response received but indicates error\n" << ecl::endl;
		return -1;
	}

	CMD8(r7, 0x01, 0x87);

	if (r7.r1.response == 0x1) {
		if (r7.OCR[3] != 0xAA) {
			ecl::cout << "Check pattern mismatch" << ecl::endl;
			return -1;
		}

		if (!r7.OCR[2]) {
			ecl::cout << "Voltage range not accepted" << ecl::endl;
			return -1;
		}

		ecl::cout << "Interface conditions are OK" << ecl::endl;
	} else {
		ecl::cout << "Interface conditions are failed to obtain" << ecl::endl;
	}

	r1.response = 0x1;

	while (r1.response == 0x1) {
		ACMD41(r1, true);

		if (r1.response & 0xfe) {
			ecl::cout << "Command failed\n" << ecl::endl;
			return -1;
		}
	}

	ecl::cout << "Card initialized properly" << ecl::endl;


	CMD58(r3);

	if (r3.r1.response == 0x1) {
		ecl::cout << "CMD58 successed" << ecl::endl;

		if (r3.OCR[3] & 1) {
			ecl::cout << "Card is powered-up" << ecl::endl;
			if (r3.OCR[3] & 2) {
				ecl::cout << "Card is High capacity" << ecl::endl;
			} else {
				ecl::cout << "Card is Low capacity" << ecl::endl;
			}
		}

#if 0
		for (uint bit = 15; bit < 24; ++bit) {
			if (r3.OCR & (1 << bit)) {
				uint8_t V = (bit - 15) * 2 + 27;

				ecl::cout << "VOL :" << ecl::endl;
				char intgr = V / 10;
				char fract = V - intgr * 10;

				char str[] = { (char) (intgr + '0'), '.', (char) (fract + '0'),
							   '\r', '\n', '\0' };

				write_string(str);
			}
		}
#endif
	} else {
		ecl::cout << "CMD58 failed" << ecl::endl;
	}

	m_spi.unlock();
	return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::close()
{
	return -1;
}

template< class SPI_dev, class GPIO_CS >
ssize_t SD_SPI< SPI_dev, GPIO_CS >::write(const uint8_t *data, size_t count)
{
	return -1;
}

template< class SPI_dev, class GPIO_CS >
ssize_t SD_SPI< SPI_dev, GPIO_CS >::read(uint8_t *data, size_t count)
{
	return -1;
}


template< class SPI_dev, class GPIO_CS >
off_t SD_SPI< SPI_dev, GPIO_CS >::seek(off_t offset)
{
	return -1;
}


template< class SPI_dev, class GPIO_CS >
off_t SD_SPI< SPI_dev, GPIO_CS >::tell() const
{
	return -1;
}

// Private methods -------------------------------------------------------------

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::send(const uint8_t *buf, size_t size)
{
	volatile bool completed = false;

	auto handler = [&completed]() {
		completed = true;
	};

	typename SPI_dev::XFER_t spi_xfer;
	spi_xfer.set_buffers(buf, nullptr, size);
	spi_xfer.set_handler(handler);
	m_spi.xfer(spi_xfer);

	while (!completed) { }
	while ((m_spi.get_status() & SPI_dev::flags::BSY)) { }

	// TODO: verify that all data was transfered
	return size;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::receive(uint8_t *buf, size_t size)
{
	volatile bool completed = false;

	auto handler = [&completed]() {
		completed = true;
	};

	typename SPI_dev::XFER_t spi_xfer;
	spi_xfer.set_buffers(nullptr, buf, size);
	spi_xfer.set_handler(handler);
	m_spi.xfer(spi_xfer);

	while (!completed) { }
	while ((m_spi.get_status() & SPI_dev::flags::BSY)) { }

	// TODO: verify that all data was transfered
	return size;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::receive_R1(R1 &r)
{
	uint8_t tries = 8;
	do {
		receive(&r.response, 1);
	} while (r.response == 0xff && --tries);

	return tries ? 0 : -1;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::receive_R3(R3 &r)
{
	r.r1.response = 0;
	std::fill_n(r.OCR, sizeof(r.OCR), 0);

   if (receive_R1(r.r1) < 0)
		return -1;

	r.r1.response = 1;
	receive((uint8_t *)r.OCR, sizeof(r.OCR));

	return 0;
}



template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::CMD0(R1 &r)
{
	constexpr uint8_t CMD0[] = { 0x40, 0, 0, 0, 0, 0x95 };

	GPIO_CS::reset();

	// Prepare a card
	uint8_t dummy = 0xff;
	send(&dummy, 1);

	// Ask for initialization
	send(CMD0, sizeof(CMD0));
	// Rerieve a result
	receive_R1(r);

	GPIO_CS::set();
	return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::CMD8(R7 &r, uint8_t VHS, uint8_t crc)
{
	VHS &= 0x03;
	const uint8_t CMD8[] = { 0x40 | 8, 0, 0, VHS, 0xAA, crc };

	GPIO_CS::reset();

	// Prepare a card
	uint8_t dummy = 0xff;
	send(&dummy, 1);

	// Send VHS
	send(CMD8, sizeof(CMD8));
	// Rerieve a result
	// Byte layout of R3 == layout of R7
	receive_R3(r);

	GPIO_CS::set();
	return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::CMD58(R3 &r)
{
	constexpr uint8_t CMD58[] = { 0x40 | 58, 0, 0, 0, 0, 0 };

	GPIO_CS::reset();

	// Prepare a card
	uint8_t dummy = 0xff;
	send(&dummy, 1);

	// Retrieve OCR
	send(CMD58, sizeof(CMD58));
	// Rerieve a result
	receive_R3(r);

	GPIO_CS::set();
	return 0;
}

template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::CMD55(R1 &r)
{
	constexpr uint8_t CMD55[] = { 0x40 | 55, 0, 0, 0, 0, 0 };

	GPIO_CS::reset();

	// Prepare a card
	uint8_t dummy = 0xff;
	send(&dummy, 1);

	// Notify about the type of next command
	send(CMD55, sizeof(CMD55));
	// Rerieve a result
	receive_R1(r);

	GPIO_CS::set();
	return 0;
}


template< class SPI_dev, class GPIO_CS >
int SD_SPI< SPI_dev, GPIO_CS >::ACMD41(R1 &r, bool HCS)
{
	uint8_t HCS_byte = HCS ? (1 << 30) : 0;
	uint8_t ACMD41[] = { 0x40 | 41, HCS_byte, 0, 0, 0, 0 };

	CMD55(r);

	// Ignore idle bit
	if (r.response & 0xfe) {
	   return 0;
	}

	GPIO_CS::reset();

	// Prepare a card
	uint8_t dummy = 0xff;
	send(&dummy, 1);

	// Send HCS
	send(ACMD41, sizeof(ACMD41));
	// Rerieve a result
	receive_R1(r);

	GPIO_CS::set();

	return 0;
}

#endif // DEV_SDSPI_H
