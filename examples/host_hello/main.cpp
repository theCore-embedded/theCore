#include <ecl/iostream.hpp>
#include <ecl/assert.h>

int main()
{
    ecl::cout << "Hello, Host World!" << ecl::endl;
    ecl_assert_msg(1 == 0, "This is an assert demonstration. Application will abort.");
    return 0;
}
