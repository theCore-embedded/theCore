#include <ecl/iostream.hpp>

namespace ecl {

// TODO: think about extending of console devices to cover
// case when there are different drivers for cin, count and cerr streams.
typename istream< console_driver >::device_type console_device;

// TODO: avoid this somehow.
// See https://isocpp.org/wiki/faq/ctors#static-init-order

istream< console_driver > cin{&console_device};
ostream< console_driver > cout{&console_device};
ostream< console_driver > cerr{&console_device};
}
