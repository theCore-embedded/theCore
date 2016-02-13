#include <ecl/thread/thread.hpp>
#include <cassert>

ecl::exposed_thread::exposed_thread(void (*fn)(void *ctx), void *data)
    :m_thread{}
{
    m_thread = std::thread(fn, data);
}

ecl::exposed_thread::exposed_thread(exposed_thread &&other)
{
    this->m_thread = std::move(other.m_thread);
}

ecl::exposed_thread::~exposed_thread()
{
    assert(m_thread.joinable());
    m_thread.detach();
}

void ecl::exposed_thread::join()
{
    assert(m_thread.joinable());
    m_thread.join();
}
