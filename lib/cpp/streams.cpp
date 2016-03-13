#include <ecl/iostream.hpp>

namespace ecl {

typename istream< console_driver >::device_type console_device;

// TODO: avoid this somehow.
// See https://isocpp.org/wiki/faq/ctors#static-init-order
typename istream< console_driver >::device_type *cin_device = &console_device;
typename istream< console_driver >::device_type *cout_device = &console_device;
typename istream< console_driver >::device_type *cerr_device = &console_device;

ecl::istream< console_driver >  cin{cin_device};
ecl::ostream< console_driver >  cout{cout_device};
ecl::ostream< console_driver >  cerr{cout_device};
}
