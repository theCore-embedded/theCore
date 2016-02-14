#include <ecl/err.hpp>

using namespace ecl;

const char* ecl::err_to_str(err error)
{
    switch (error) {
    case err::ok:
        return "OK";
    case err::perm:
        return "Operation not permitted";
    case err::busy:
        return "Device or resource busy";
    case err::srch:
        return "No such process";
    case err::generic:
        return "Generic error occur";
    default:
        return "Unknown error code";
    }
}

