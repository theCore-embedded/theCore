#include "target.hpp"

#include <ecl/iostream.hpp>
#include <ecl/thread/utils.hpp>
#include <ecl/thread/semaphore.hpp>

#include <platform/exti_manager.hpp>

using ecl::exti_manager;
using ecl::cout;
using ecl::endl;
using ecl::semaphore;

extern "C"
void board_init()
{
    gpio_init();
}

void user_button_handler(void *ctx)
{
    semaphore *s = reinterpret_cast<semaphore*>(ctx);
    s->signal();
}

int main()
{
    ecl::cout << "Hello, Embedded World!" << ecl::endl;

    semaphore s;
    exti_manager::handler h;

    h.set_ctx(&s);
    h.set_cb(user_button_handler);

    exti_manager::subscribe<usr_btn>(h, exti_manager::trigger::rising);

    while (1) {
        s.wait();
        ecl::cout << "Button pressed!" << ecl::endl;
        exti_manager::unmask(h);
    }
}
