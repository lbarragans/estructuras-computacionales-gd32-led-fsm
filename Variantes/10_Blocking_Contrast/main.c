#include <stdint.h>
#include "gd32vw55x.h"

#define LED_GPIO_PORT  GPIOC
#define LED_GPIO_PIN   GPIO_PIN_13
#define LED_GPIO_CLOCK RCU_GPIOC

volatile uint32_t g_background_iterations = 0U;

static void busy_wait_ms(uint32_t milliseconds)
{
    while (milliseconds-- > 0U) {
        volatile uint32_t cycles = 16000U;
        while (cycles-- > 0U) {
            __asm volatile ("nop");
        }
    }
}

static void led_init(void)
{
    rcu_periph_clock_enable(LED_GPIO_CLOCK);
    gpio_mode_set(LED_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_GPIO_PIN);
    gpio_output_options_set(
        LED_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, LED_GPIO_PIN
    );
    gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
}

static void pulse(uint32_t on_ms, uint32_t off_ms)
{
    gpio_bit_reset(LED_GPIO_PORT, LED_GPIO_PIN);
    busy_wait_ms(on_ms);

    gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
    busy_wait_ms(off_ms);
}

int main(void)
{
    led_init();

    for (;;) {
        for (uint32_t i = 0U; i < 3U; ++i) {
            pulse(500U, 500U);
        }

        for (uint32_t i = 0U; i < 5U; ++i) {
            pulse(150U, 150U);
        }

        gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
        busy_wait_ms(2000U);

        /*
         * Solo se incrementa una vez por ciclo porque la CPU estuvo bloqueada
         * durante todas las esperas.
         */
        g_background_iterations++;
    }
}
