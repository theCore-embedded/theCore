#ifdef CONFIG_USE_BYPASS_CONSOLE
namespace ecl
{
extern void bypass_console_init();
} // namespace ecl
#endif // CONFIG_USE_CONSOLE

// Required by ARM ARCH startup code
extern "C" void SystemInit()
{
    // TODO: implement
}

extern "C" void platform_init()
{
    // TODO: implement

#ifdef CONFIG_USE_BYPASS_CONSOLE
    ecl::bypass_console_init();
#endif
}
