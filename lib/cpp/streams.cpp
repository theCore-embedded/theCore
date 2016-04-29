//!
//! \file
//! \brief Main iostream module.
//! Nifty counter idiom is used here.
//! See [wiki article about that](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Nifty_Counter)
//!

#include <ecl/iostream.hpp>

#include <new>         // placement new
#include <type_traits> // aligned_storage

namespace ecl {

// Useful aliases
using cin_type  = istream< console_driver >;
using cout_type = ostream< console_driver >;
using cerr_type = ostream< console_driver >;

//! Tracks initialization and destruction.
static int nifty_counter;

//! Memory for the console device driver
static typename std::aligned_storage< sizeof(console_driver), alignof(console_driver) >::type
  console_obj_buf;

//! Memory for the cin object
static typename std::aligned_storage< sizeof(cin_type), alignof(cin_type) >::type
  cin_obj_buf;

//! Memory for the cout object
static typename std::aligned_storage< sizeof(cout_type), alignof(cout_type) >::type
  cout_obj_buf;

//! Memory for the cerr object
static typename std::aligned_storage< sizeof(cerr_type), alignof(cerr_type) >::type
  cerr_obj_buf;

// TODO: think about extending of console devices to cover
// a case when different drivers for cin, count and cerr streams must be used.
typename istream< console_driver >::device_type &console_device
= reinterpret_cast< console_driver& >(console_obj_buf);

cin_type  &cin  = reinterpret_cast< cin_type& >(cin_obj_buf);
cout_type &cout = reinterpret_cast< cout_type& >(cout_obj_buf);
cerr_type &cerr = reinterpret_cast< cerr_type& >(cerr_obj_buf);

iostream_initializer::iostream_initializer()
{
    if (!nifty_counter++) {
        // Driver must be ready before streams will use it.
        new (&console_obj_buf) console_driver{};

        console_device.init();

        new (&cin_obj_buf)   cin_type{&console_device};
        new (&cout_obj_buf)  cout_type{&console_device};
        new (&cerr_obj_buf)  cerr_type{&console_device};

        // Streams could be used direclty only after kernel_main() call
        // in sys.cpp module.
        // This is unwanted limitation.
        // TODO: resolve it.
    }
}

iostream_initializer::~iostream_initializer()
{
    if (!--nifty_counter) {
        cerr.~cerr_type();
        cout.~cout_type();
        cin.~cin_type();
        console_device.~console_driver();
    }
}

}
