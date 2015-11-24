// Required for ARM architecture
extern "C" int  __aeabi_atexit(void* object, void (*destroyer)(void*), void* dso_handle)
{
    (void) object;
    (void) destroyer;
    (void) dso_handle;
    return 0;
}
