#ifndef LIB_THREAD_HOST_THREAD_HPP_
#define LIB_THREAD_HOST_THREAD_HPP_

#include "semaphore.hpp"

#include <pthread.h>
#include <string>
#include <ecl/err.hpp>


namespace ecl
{

// TODO
class native_thread
{
public:
    using routine = ecl::err (*)(void *);

    native_thread();

    native_thread(native_thread &&other);
    ~native_thread();

    native_thread& operator=(native_thread &) = delete;

    // Sets new stack size
    // Asserts if stack size == 0
    // Return err::busy if thread is started
    ecl::err set_stack_size(size_t size);

    // Sets name
    // err::srch if thread is in detached state
    // err::generic if something bad happens
    ecl::err set_name(const char *name);

    // Writes thread name to buf, at most 'size' bytes including null-character
    // returns length of expected name (excluding null-character).
    // Retval < 0 if some error occur
    ssize_t get_name(char *buf, size_t size);

    // Sets routine and its context
    // Asserts if fn == null
    // Return error if thread already started
    ecl::err set_routine(routine fn, void *arg);

    // Starts thread
    // Hits assert if routine wasn't set
    // Returns error if something bad happen
    // Returns err::busy if already started
    ecl::err start();

    // Error if not joinable because:
    //  - thread is not started
    //  - thread is in detached state
    //  - thread was already joined
    ecl::err join(ecl::err &retcode);
    // Same as above, used when return value from thread is not needed
    ecl::err join();

    // Error if cannot be detached because:
    // - thread is not started
    // - thread was already detached
    // - thread was already joined
    ecl::err detach();

private:
    struct runner_arg
    {
        ecl::semaphore  start_flag;
        routine         start_routine;
        void            *routine_arg;
    };

    static void* thread_runner(void *ctx);

    enum class state
    {
        initial,
        started,
        detached,
        //joined,
    };

    pthread_t       m_thread;
    size_t          m_stack;
    std::string     m_name;
    state           m_state;
    routine         m_fn;
    void            *m_arg;
//    bool        m_default;   // Thread is in default state
};

}

#endif // LIB_THREAD_HOST_THREAD_HPP_
