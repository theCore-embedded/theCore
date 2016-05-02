//! \file
//! \brief Collection of kernel stubs.
//! This module suppplies the dummy kernel main routine in case if kernel
//! isn't present or it's main routine shouldn't be called when initiliazing
//! the Core.

extern int early_main();

// Do nothing if kernel isn't used
void kernel_main()
{
    early_main();
}
