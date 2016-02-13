#include <ecl/err.hpp>

using namespace ecl;

const char* ecl::err_to_str(err error)
{
    switch (error) {
    case err::ok:
        return "OK";
    case err::perm:
        return "Operation not permitted";
    default:
        return "Unknown error code";
    }
}

