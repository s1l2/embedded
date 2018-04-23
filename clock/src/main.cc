#include "drivers/stm32/gpio_bus.h"
#include "drivers/stm32/i2c_bus.h"
#include "drivers/stm32/rtc.h"
#include "drivers/stm32/as1115_display.h"
#include "drivers/stm32/status_led.h"
#include "drivers/stm32/pec11_renc.h"

#include "ui.h"

using namespace clock;
using namespace stm32;

RTClock rtc;

GPIOBus gpiob(GPIOBus::Id::B);
GPIOBus gpiod(GPIOBus::Id::D);
GPIOPin scl_pin(gpiob, 6);
GPIOPin sda_pin(gpiob, 9);

I2CBus i2c(I2CBus::Id::ONE, scl_pin, sda_pin);
AS1115Display display(i2c, 5);

GPIOPin ok_led(gpiod, 15);
GPIOPin error_led(gpiod, 14);
GPIOPin activity_led(gpiod, 13);

StatusLed status_led(ok_led, error_led, activity_led);

GPIOPin encoder_clockwise(gpiod, 2);
GPIOPin encoder_counter_clockwise(gpiod, 3);
GPIOPin encoder_button(gpiod, 6);

Pec11RotaryEncoder encoder(encoder_clockwise,
                           encoder_counter_clockwise,
                           encoder_button);

UI ui(&display, &encoder);

static bool set_time() {
    RTClock::Time time;

    time.hour = 12;
    time.minute = 0;
    time.second = 0;
    time.am_pm = RTClock::AM_PM::AM;
    return rtc.SetTime(&time);
}

static void Init() {
    gpiob.Init();
    gpiod.Init();
    status_led.Init();
    status_led.SetError(!rtc.Init());
    i2c.Init();
    display.Init();
    encoder.Init();

    status_led.SetError(!set_time());
}

static void update_time() {
    RTClock::Time time;

    rtc.GetTime(&time);
    ui.Clear();
    ui.SetHour(time.hour);
    ui.SetMinute(time.minute);
    ui.Update();
}

static inline void delay() {
    for (int i = 0; i < 1280000; i++);
}

int main() {
    Init();

    status_led.SetOk(true);
    while (true) {
        update_time();
    }
}

extern "C" {
void __cxa_pure_virtual() {
}
}
