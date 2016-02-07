#ifndef LIB_THREAD_HOST_SEMAPHORE_HPP_
#define LIB_THREAD_HOST_SEMAPHORE_HPP_

namespace ecl
{

class semaphore
{
public:
    constexpr semaphore() { }

    void signal() {}
    void wait() {}

    semaphore(const semaphore&)             = delete;
    semaphore& operator=(const semaphore&)  = delete;

private:
};

}

#endif
