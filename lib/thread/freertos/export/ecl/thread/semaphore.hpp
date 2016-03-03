#ifndef LIB_THREAD_FREERTOS_SEMAPHORE_HPP_
#define LIB_THREAD_FREERTOS_SEMAPHORE_HPP_

#include <FreeRTOS.h>
#include <semphr.h>

namespace ecl
{

//!
//! \brief FreeRTOS-based semaphore.
//! \TODO: notes about FreeRTOS semaphore limitation
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

    semaphore(const semaphore&)             = delete;
    semaphore& operator=(const semaphore&)  = delete;

private:
    SemaphoreHandle_t m_semaphore;
};


//!
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

    binary_semaphore(const semaphore&)             = delete;
    binary_semaphore& operator=(const semaphore&)  = delete;

private:
    SemaphoreHandle_t m_semaphore;
};

}

#endif // LIB_THREAD_FREERTOS_SEMAPHORE_HPP_
