#ifndef LIB_THREAD_FREERTOS_SEMAPHORE_HPP_
#define LIB_THREAD_FREERTOS_SEMAPHORE_HPP_

#include <FreeRTOS.h>
#include <semphr.h>

#include <ecl/err.hpp>

#include <atomic>

namespace ecl
{

//! \brief FreeRTOS-based semaphore.
//! This semaphore is specific. It can only be raisen or cleared.
//! Default state is cleared, so calling wait() would block until someone
//! will call signal()
//!
class binary_semaphore
{
public:
    binary_semaphore();
    ~binary_semaphore();

    //!
    //! \brief Signals a semaphore.
    //! Can be called from ISR
    //!
    void signal();

    //!
    //! \brief Waits a semaphore.
    //! Default state is cleared, so calling wait() would block until someone
    //! will call signal()
    //!
    void wait();

    //!
    //! \brief Tries to wait on semaphore without lock.
    //! \retval false  Someone already waits a semaphore.
    //! \retval true   Semaphore counter decremented.
    //!
    bool try_wait();

    binary_semaphore(const binary_semaphore&)             = delete;
    binary_semaphore& operator=(const binary_semaphore&)  = delete;

private:
    SemaphoreHandle_t m_semaphore;
};


//! \brief FreeRTOS-based semaphore.
//!
class semaphore
{
public:
    semaphore();
    ~semaphore();

    //!
    //! \brief Signals a semaphore.
    //! Can be called from ISR.
    //!
    void signal();

    //!
    //! \brief Waits a semaphore
    //! Cannot be called from ISR.
    //!
    void wait();

    //!
    //! \brief Tries to wait on semaphore without lock.
    //! \retval false  Wait will result in lock.
    //! \retval true   Semaphore counter decremented.
    //!
    bool try_wait();

    semaphore(const semaphore&)             = delete;
    semaphore& operator=(const semaphore&)  = delete;

private:
    binary_semaphore m_semaphore;
    std::atomic_int  m_cnt;
};

}

#endif // LIB_THREAD_FREERTOS_SEMAPHORE_HPP_
