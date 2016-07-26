//!
//! \file
//! \brief DMA wrapper interface for STM32 platform
//!
#ifndef DMA_WRAP_INTERFACE_HPP_
#define DMA_WRAP_INTERFACE_HPP_

#include <stm32_device.hpp>

namespace ecl
{

//! Common data sizes of the DMA transaction, defined by STM32 SPL
enum class dma_data_sz
{
    byte  = DMA_MemoryDataSize_Byte,
    word  = DMA_MemoryDataSize_Word,
    hword = DMA_MemoryDataSize_HalfWord,
};

//! Common DMA modes, defined by STM32 SPL
enum class dma_mode
{
    normal   = DMA_Mode_Normal,
    circular = DMA_Mode_Circular,
};

//------------------------------------------------------------------------------

//! Generic interface of the DMA on STM32 platforms.
//! \details The STM32 MCU family module must implement each method in its own
//! headers. Depends on the family the entity behind the class may differ.
//! E.g. in STM32F4 each DMA instance is unambiguously described by the pair of
//! "stream" and "channel" in contrast to just "channel" in STM32L1.
//! Template parameter allows to incapsulate additional methods into
//! separate class required by the MCU family while keeping in it the scope
//! of a base class and not exposing it in its body.
//! \tparam Impl Desired additional implementation, defined by the platform.
template<class Impl>
class dma_wrap_base
{
public:
    dma_wrap_base() = delete;
    dma_wrap_base(dma_wrap_base &) = delete;

    //! Gets IRQ number of given DMA entity.
    static constexpr auto get_irqn();

    //! Initializes DMA.
    //! \pre DMA is not initialized.
    //! \post DMA is initialized and ready to work.
    static void init();

    //! Prepares DMA transaction from the memory buffer to the given peripheral.
    //! \pre    DMA was initialized by calling init() method.
    //! \pre    src != nullptr, periph != nullptr, size > 0
    //! \post   DMA is configured with given parameters and ready to go.
    //! \details Transaction can be triggered by enable() call. DMA events
    //! can be enabled using enable_events_irq() routine. If circular mode is chosen
    //! then new DMA transaction will start automatically, without need of
    //! the explicit enable() call.
    //! \tparam     Size    Required size of a single data unit.
    //! \tparam     Mode    Mode of operation.
    //! \param[in]  src     The source memory buffer from which data will be read.
    //! \param[in]  size    Size of the memory buffer buffer in bytes.
    //! \param[in]  periph  The valid peripheral address.
    template<dma_data_sz Size = dma_data_sz::byte, dma_mode Mode = dma_mode::normal>
    static void
    mem_to_periph(const uint8_t *src, size_t size, volatile uint16_t *periph);

    //! Prepares DMA transaction that sends given data unit to the given peripheral.
    //! \pre    DMA was initialized by calling init() method.
    //! \pre    cnt > 0, periph != nullptr
    //! \post   DMA is configured with given parameters and ready to go.
    //! \details Transaction can be triggered by enable() call. DMA events can
    //! be enabled using enable_events_irq() routine. If circular mode is chosen
    //! then new DMA transaction will start automatically, without need of
    //! the explicit enable() call.
    //! \tparam     Size    Required size of a single data unit.
    //! \tparam     Mode    Mode of operation.
    //! \param[in]  filler  Data unit to be transfered.
    //! \param[in]  cnt     Amount of data units to be transfered.
    //! \param[in]  periph  The valid peripheral address.
    template<dma_data_sz Size = dma_data_sz::byte, dma_mode Mode = dma_mode::normal>
    static void
    mem_to_periph(uint16_t filler, size_t cnt, volatile uint16_t *periph);

    //! Prepares DMA transaction from the peripheral to the given memory buffer.
    //! \pre    DMA was initialized by calling init() method.
    //! \pre    size > 0, periph != nullptr, dst != nullptr
    //! \post   DMA is configured with given parameters and ready to go.
    //! \details Transaction can be triggered by enable() call. DMA events can
    //! be enabled using enable_events_irq() routine. If circular mode is chosen
    //! then new DMA transaction will start automatically, without need of
    //! the explicit enable() call.
    //! \tparam     Size    Size of a single data unit.
    //! \tparam     Mode    Mode of operation.
    //! \param[in]  periph  The valid peripheral address.
    //! \param[in]  size    Size of the memory buffer buffer in bytes.
    //! \param[in]  dst     The destination buffer.
    template<dma_data_sz Size = dma_data_sz::byte, dma_mode Mode = dma_mode::normal>
    static void
    periph_to_mem(volatile uint16_t *periph, uint8_t *dst, size_t size);

    //! Prepares DMA transaction that reads data from peripheral without storing it.
    //! \pre    DMA was initialized by calling init() method.
    //! \pre    size > 0, periph != nullptr
    //! \post   DMA is configured with given parameters and ready to go.
    //! \details Transaction can be triggered by enable() call. DMA events can
    //! be enabled using enable_events_irq() routine. If circular mode is chosen
    //! then new DMA transaction will start automatically, without need of
    //! the explicit enable() call.
    //! \tparam     Size    Size of a single data unit.
    //! \tparam     Mode    Mode of operation.
    //! \param[in]  periph  The valid peripheral address.
    //! \param[in]  size    Size of the memory buffer buffer in bytes.
    //! \param[in]  dst     The destination buffer.
    template<dma_data_sz Size = dma_data_sz::byte, dma_mode Mode = dma_mode::normal>
    static void
    periph_to_mem(volatile uint16_t *periph, size_t size);

    // TODO: consider merging enable_events_irq()/enable() routines into `start()` method
    // and disable_events()/disable() into `stop()` method

    //! Enables IRQ for the events from DMA entity.
    //! \pre  DMA was initialized by calling init() method.
    //! \post DMA configured to trigger IRQ every time corresponding event occur.
    //! \details Events will be generated by the DMA according to type of
    //! events requested. If enabled, events will be delivered via IRQ.
    //! To subscribe for it use IRQ descriptor returned by the get_irqn() routine.
    //! Use tc(), ht() and err() to check if event occurred or not.
    //! \tparam EnableTC    Set if transfer complete event must be enabled.
    //! \tparam EnableHT    Set if half transfer event must be enabled.
    //! \tparam EnableErr   Set if error event must be enabled.
    //! \note Event IRQ allowance is not changed if relevant argument is not set.
    //! See example below
    //! \code{.cpp}
    //!
    //!     // Initial state of the IRQ allowance:
    //!     // TC - disabled, HT - disabled, ERR - disabled
    //!
    //!     dma_tx::template enable_events_irq<true, false, false>();
    //!
    //!     // New state of the IRQ allowance:
    //!     // TC - enabled, HT - disabled, ERR - disabled
    //!
    //!     dma_tx::template enable_events_irq<false, false, true>();
    //!
    //!     // New state of the IRQ allowance. Note that HT event state was not
    //!     // changed:
    //!     // TC - enabled, HT - disabled, ERR - enabled
    //! \endcode
    template<bool EnableTC = true, bool EnableHT = true, bool EnableErr = true>
    static void enable_events_irq();

    //! Disables IRQ for the events from DMA entity.
    //! \pre  DMA was initialized by calling init() method.
    //! \post DMA configure to do not trigger IRQ for selected events.
    //! \details IRQ previously enabled by enable_events_irq() can be disabled
    //! using this method..
    //! \tparam EnableTC    Set if transfer complete event must be enabled.
    //! \tparam EnableHT    Set if half transfer event must be enabled.
    //! \tparam EnableErr   Set if error event must be enabled.
    //! \note Event IRQ allowance is not changed if relevant argument is not set.
    //! See example below
    //! \code{.cpp}
    //!
    //!     // Initial state of the IRQ allowance:
    //!     // TC - enabled, HT - enabled, ERR - enabled
    //!
    //!     dma_tx::template disable_events_irq<true, false, false>();
    //!
    //!     // New state of the IRQ allowance:
    //!     // TC - disabled, HT - enabled, ERR - enabled
    //!
    //!     dma_tx::template disable_events_irq<false, false, true>();
    //!
    //!     // New state of the IRQ allowance. Note that HT event state was not
    //!     // changed:
    //!     // TC - disabled, HT - enabled, ERR - disabled
    //! \endcode
    template<bool DisableTC = true, bool DisableHT = true, bool DisableErr = true>
    static void disable_events_irq();

    //! Enables DMA transaction.
    //! \pre  DMA was configured by one of the periph_to_mem() or mem_to_periph() methods.
    //! \pre  DMA was not yet started or already stopped previously by disable() call.
    //! \post DMA transaction enabled and started.
    //! \details By invoking this method one can lauch previously configured
    //! transaction. Use tc(), ht() and err() to check particular event state.
    //! Corresponding IRQ will be triggered if configured by enable_events_irq().
    static void enable();

    //! Disables DMA transaction.
    //! \pre  DMA was enabled by enable() and not yet disabled.
    //! \post DMA transaction stopped and disabled.
    //! \details If to call disable() before TC event occur then DMA will stop
    //! only after last data unit will be transfered.
    static void disable();

    //! Checks if TC event occurred.
    //! \pre    DMA was initialized by calling init() method.
    //! \return true if tranfer completed, false otherwise.
    static bool tc();

    //! Checks if HT event occurred.
    //! \pre    DMA was initialized by calling init() method.
    //! \return true if tranfer is half-completed, false otherwise.
    static bool ht();

    //! Checks if TC event occurred.
    //! \pre    DMA was initialized by calling init() method.
    //! \return true if error occurred during transaction, false otherwise.
    static bool err();

    //! Clears TC event.
    //! \pre    DMA was initialized by calling init() method.
    //! \post   tc() == false.
    static void clear_tc();

    //! Clears HT event.
    //! \pre    DMA was initialized by calling init() method.
    //! \post   ht() == false.
    static void clear_ht();

    //! Clears ERR event.
    //! \pre    DMA was initialized by calling init() method.
    //! \post   err() == false.
    static void clear_err();

    //! Checks how many bytes left to transmit via DMA entity.
    //! \pre    DMA was initialized by calling init() method.
    static auto bytes_left();
};

} // namespace ecl

#endif //DMA_WRAP_INTERFACE_HPP_
