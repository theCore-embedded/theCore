#include <ecl/thread/thread.hpp>
#include <cassert>

ecl::native_thread::native_thread()
    :m_thread{}
    ,m_default{true}
{

}

ecl::native_thread::native_thread(int (*fn)(void *ctx), void *data)
    :m_thread{}
    ,m_default{false}
{
    m_thread = std::thread(fn, data);
}

ecl::native_thread::native_thread(native_thread &&other)
{
    this->m_thread = std::move(other.m_thread);
    this->m_default = false;
    other.m_default = true;
}

ecl::native_thread::~native_thread()
{
    if (!m_default) {
        // Should never happen
        ecl_assert_msg(m_thread.joinable(), "Thread object destroyed while not joinable");
        m_thread.detach();
    }
}

ecl::err ecl::native_thread::join()
{
    if (!m_thread.joinable()) {
        return ecl::err::perm;
    }

    m_thread.join();
    m_default = true;
    return ecl::err::ok;
}
