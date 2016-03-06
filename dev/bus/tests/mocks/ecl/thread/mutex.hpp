#ifndef BUS_MOCKS_MUTEX_HPP_
#define BUS_MOCKS_MUTEX_HPP_


#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <ecl/err.hpp>


namespace ecl
{

class mutex
{
public:
    void lock()
    {
        mock("mutex").actualCall("lock");
    }

    void unlock()
    {
        mock("mutex").actualCall("unlock");
    }

    bool try_lock()
    {
        return mock("mutex").actualCall("try_lock").returnIntValue();
    }
};

}


#endif // BUS_MOCKS_MUTEX_HPP_
