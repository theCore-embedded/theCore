/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ECL_LIB_UTILS_ERR_HPP_
#define ECL_LIB_UTILS_ERR_HPP_

namespace ecl
{

// Rudely taken from http://en.cppreference.com/w/cpp/error/errno_macros
enum class err : int
{
    ok               =   0,    // Successfully completed
    tobig            =  -1,    // Argument list too long
    acces            =  -2,    // Permission denied
    addrinuse        =  -3,    // Address in use
    addrnotavail     =  -4,    // Address not available
    afnosupport      =  -5,    // Address family not supported
    again            =  -6,    // Resource unavailable
    already          =  -7,    // Connection already in progress
    badf             =  -8,    // Bad file descriptor
    badmsg           =  -9,    // Bad message
    busy             = -10,    // Device or resource busy
    canceled         = -11,    // Operation canceled
    child            = -12,    // No child processes
    connaborted      = -13,    // Connection aborted
    connrefused      = -14,    // Connection refused
    connreset        = -15,    // Connection reset
    deadlk           = -16,    // Resource deadlock would occur
    destaddrreq      = -17,    // Destination address required
    dom              = -18,    // Mathematics argument out of domain of function
    exist            = -19,    // File exists
    fault            = -20,    // Bad address
    fbig             = -21,    // File too large
    hostunreach      = -22,    // Host is unreachable
    idrm             = -23,    // Identifier removed
    ilseq            = -24,    // Illegal byte sequence
    inprogress       = -25,    // Operation in progress
    intr             = -26,    // Interrupted function
    inval            = -27,    // Invalid argument
    io               = -28,    // I/O error
    isconn           = -29,    // Socket is connected
    isdir            = -30,    // Is a directory
    loop             = -31,    // Too many levels of symbolic links
    mfile            = -32,    // File descriptor value too large
    mlink            = -33,    // Too many links
    msgsize          = -34,    // Message too large
    nametoolong      = -35,    // Filename too long
    netdown          = -36,    // Network is down
    netreset         = -37,    // Connection aborted by network
    netunreach       = -38,    // Network unreachable
    nfile            = -39,    // Too many files open in system
    nobufs           = -40,    // No buffer space available
    nodata           = -41,    // No message is available on the STREAM head read queue
    nodev            = -42,    // No such device
    noent            = -43,    // No such file or directory
    noexec           = -44,    // Executable file format error
    nolck            = -45,    // No locks available
    nolink           = -46,    // Link has been severed
    nomem            = -47,    // Not enough space
    nomsg            = -48,    // No message of the desired type
    noprotoopt       = -49,    // Protocol not available
    nospc            = -50,    // No space left on device
    nosr             = -51,    // No STREAM resources
    nostr            = -52,    // Not a STREAM
    nosys            = -53,    // Function not supported
    notconn          = -54,    // The socket is not connected
    notdir           = -55,    // Not a directory
    notempty         = -56,    // Directory not empty
    notrecoverable   = -57,    // State not recoverable
    notsock          = -58,    // Not a socket
    notsup           = -59,    // Not supported
    notty            = -60,    // Inappropriate I/O control operation
    nxio             = -61,    // No such device or address
    opnotsupp        = -62,    // Operation not supported on socket
    overflow         = -63,    // Value too large to be stored in data type
    ownerdead        = -64,    // Previous owner died
    perm             = -65,    // Operation not permitted
    pipe             = -66,    // Broken pipe
    proto            = -67,    // Protocol error
    protonosupport   = -68,    // Protocol not supported
    prototype        = -69,    // Protocol wrong type for socket
    range            = -70,    // Result too large
    rofs             = -71,    // Read-only file system
    spipe            = -72,    // Invalid seek
    srch             = -73,    // No such process
    time             = -74,    // Stream ioctl() timeout
    timedout         = -75,    // Connection timed out
    txtbsy           = -76,    // Text file busy
    wouldblock       = -77,    // Operation would block
    xdev             = -78,    // Cross-device link
    generic          = -79,    // Generic error
};

static inline bool is_error(err error)
{
    return error != err::ok;
}

static inline bool is_ok(err error)
{
    return !is_error(error);
}

const char* err_to_str(err error);

} // namespace ecl

#ifdef CORE_TESTS_ENABLED
#include <CppUTest/SimpleString.h>

static inline SimpleString StringFrom(ecl::err err)
{
    return SimpleString{ecl::err_to_str(err)};
}
#endif // CORE_TESTS_ENABLED

#endif // ECL_LIB_UTILS_ERR_HPP_
