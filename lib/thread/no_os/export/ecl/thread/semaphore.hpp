//! \file
//! \brief Semaphore implementation for projects without OS support.

#ifndef LIB_THREAD_NO_OS_SEMAPHORE_
#define LIB_THREAD_NO_OS_SEMAPHORE_

#include <ecl/err.hpp>
#include <chrono>

#include <atomic>

namespace ecl
{

//! Naive counting semaphore, acting like spinlock of some kind.
//! \details Suitable for targets without OS included and without WFI/WFE
//! enabled. Useful during bring-up of new platforms. Main requirement -
//! atomic support.
class semaphore
{
public:
    //! Constructs semaphore.
    constexpr semaphore()
        :m_counter{0} { }

    //! Signals semaphore, rises counter.
    //! \details Can be called from ISR.
    void signal();

    //! Waits for semaphore, decreases counter.
    //! \details Not callable from ISR. Blocks until signal() is called in
    //! different context.
    void wait();

    //! Tries to wait for semaphore signal.
    //! \details Will wait for semaphore signal with provided timeout.
    //! If no parameter given then it will immediately exit if there is no
    //! pending signals.
    //! \param[in] ms Milliseconds to wait.
    //! \retval true Semaphore signal consumed, counter decreased.
    //! \retval false Nothing to consume.
    bool try_wait(std::chrono::milliseconds ms = std::chrono::milliseconds(0));

    semaphore(const semaphore&)             = delete;
    semaphore& operator=(const semaphore&)  = delete;

private:
    std::atomic_int m_counter;  //!< Semaphore counter.
};

//! Naive binary semaphore, acting like spinlock of some kind.
//! \details Suitable for targets without OS included and without WFI/WFE
//! enabled. Useful during bring-up of new platforms. Main requirement -
//! atomic support.
//! Can take only two values - 0 or 1.
class binary_semaphore
{
public:
    //! Constructs semaphore.
    constexpr binary_semaphore()
        :m_flag{0} { }

    //! Signals semaphore, sets counter to 1.
    //! \details Can be called from ISR.
    void signal();

    //! Waits for semaphore, sets counter to 0.
    void wait();

    //! Tries to wait for semaphore signal.
    //! \details Will wait for semaphore signal with provided timeout.
    //! If no parameter given then it will immediately exit if there is no
    //! pending signals.
    //! \param[in] ms Milliseconds to wait.
    //! \retval true Semaphore signal consumed, counter decreased.
    //! \retval false Nothing to consume.
    bool try_wait(std::chrono::milliseconds ms = std::chrono::milliseconds(0));

    binary_semaphore(const binary_semaphore&)             = delete;
    binary_semaphore& operator=(const binary_semaphore&)  = delete;

private:
    std::atomic_bool m_flag;    //!< Binary flag.
};

} // namespace ecl

#endif // LIB_THREAD_NO_OS_SEMAPHORE_
