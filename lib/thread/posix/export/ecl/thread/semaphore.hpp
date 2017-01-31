#ifndef LIB_THREAD_HOST_SEMAPHORE_HPP_
#define LIB_THREAD_HOST_SEMAPHORE_HPP_

#include <condition_variable>
#include <mutex>
#include <atomic>
#include <ecl/err.hpp>

//------------------------------------------------------------------------------

namespace ecl
{

//! Counting semaphore.
//! \details Implemented for host platform.
class semaphore
{
public:
    //! Constructs semaphore.
    semaphore();

    //! Signals semaphore, rises counter.
    void signal();

    //! Waits for semaphore, decreases counter.
    //! \details Not callable from ISR. Blocks utill signal() is called in
    //! different context.
    void wait();

    //! Tries to wait for semaphore signal.
    //! \details Will wait for semaphore signal with provided timeout.
    //! If no parameter given then it will immediatelly exit if there is no
    //! pending signals.
    //! \param[in] ms Milliseconds to wait.
    //! \retval true Semaphore signal consumed, counter decreased.
    //! \retval false Nothing to consume.
    bool try_wait(std::chrono::milliseconds ms = std::chrono::milliseconds::min());

    semaphore(const semaphore&)             = delete;
    semaphore& operator=(const semaphore&)  = delete;

private:
    std::mutex                  m_mutex;    //!< Mutex to lock for conditional.
    std::condition_variable     m_cond;     //!< Conditional.
    std::atomic_int             m_cnt;      //!< Atomic counter.
};

//------------------------------------------------------------------------------

//! Binary semaphore.
//! \details Can take only two values - 0 or 1.
class binary_semaphore
{
public:
    //! Constructs semaphore.
    binary_semaphore();

    //! Signals semaphore, sets counter to 1.
    //! \details Can be called from ISR.
    void signal();

    //! Waits for semaphore, sets counter to 0.
    void wait();

    //! Tries to wait for semaphore signal.
    //! \details Will wait for semaphore signal with provided timeout.
    //! If no parameter given then it will immediatelly exit if there is no
    //! pending signals.
    //! \param[in] ms Milliseconds to wait.
    //! \retval true Semaphore signal consumed, counter decreased.
    //! \retval false Nothing to consume.
    bool try_wait(std::chrono::milliseconds ms = std::chrono::milliseconds::min());

    binary_semaphore(const binary_semaphore&)             = delete;
    binary_semaphore& operator=(const binary_semaphore&)  = delete;

private:
    std::mutex                  m_mutex;    //!< Mutex to lock for conditional.
    std::condition_variable     m_cond;     //!< Conditional.
    std::atomic_bool            m_flag;     //!< Atomic counter.
};

} // namespace ecl

#endif // LIB_THREAD_HOST_SEMAPHORE_HPP_
