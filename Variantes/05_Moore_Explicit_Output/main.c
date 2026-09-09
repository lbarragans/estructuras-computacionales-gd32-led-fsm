#include <stdint.h>
#include "gd32vw55x.h"
#include "systimer.h"

#define LED_GPIO_PORT       GPIOC
#define LED_GPIO_PIN        GPIO_PIN_13
#define LED_GPIO_CLOCK      RCU_GPIOC
#define SLOW_HALF_MS        500U
#define FAST_HALF_MS        150U
#define PAUSE_MS            2000U
#define SLOW_PULSES         3U
#define FAST_PULSES         5U

static uint8_t led_on_flag = 0U;

static void led_init(void)
{
    rcu_periph_clock_enable(LED_GPIO_CLOCK);
    gpio_mode_set(LED_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_GPIO_PIN);
    gpio_output_options_set(
        LED_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, LED_GPIO_PIN
    );
    gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
    led_on_flag = 0U;
}

static void led_set(uint8_t on)
{
    if (on != 0U) {
        gpio_bit_reset(LED_GPIO_PORT, LED_GPIO_PIN);
        led_on_flag = 1U;
    } else {
        gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
        led_on_flag = 0U;
    }
}

static void led_toggle(void)
{
    led_set((uint8_t)(led_on_flag == 0U));
}

typedef enum {
    MOORE_SLOW_ON = 0,
    MOORE_SLOW_OFF,
    MOORE_FAST_ON,
    MOORE_FAST_OFF,
    MOORE_PAUSE
} moore_state_t;

volatile moore_state_t g_state = MOORE_SLOW_ON;
volatile uint32_t g_background_iterations = 0U;

static uint32_t entered_ms = 0U;
static uint8_t slow_done = 0U;
static uint8_t fast_done = 0U;

static void apply_output(moore_state_t state)
{
    switch (state) {
    case MOORE_SLOW_ON:
    case MOORE_FAST_ON:
        led_set(1U);
        break;
    default:
        led_set(0U);
        break;
    }
}

static void enter_state(moore_state_t next, uint32_t now)
{
    g_state = next;
    entered_ms = now;
    apply_output(next);
}

static void fsm_update(void)
{
    const uint32_t now = systimer_millis();

    switch (g_state) {
    case MOORE_SLOW_ON:
        if ((uint32_t)(now - entered_ms) >= SLOW_HALF_MS) {
            enter_state(MOORE_SLOW_OFF, now);
        }
        break;

    case MOORE_SLOW_OFF:
        if ((uint32_t)(now - entered_ms) >= SLOW_HALF_MS) {
            slow_done++;
            if (slow_done >= SLOW_PULSES) {
                slow_done = 0U;
                enter_state(MOORE_FAST_ON, now);
            } else {
                enter_state(MOORE_SLOW_ON, now);
            }
        }
        break;

    case MOORE_FAST_ON:
        if ((uint32_t)(now - entered_ms) >= FAST_HALF_MS) {
            enter_state(MOORE_FAST_OFF, now);
        }
        break;

    case MOORE_FAST_OFF:
        if ((uint32_t)(now - entered_ms) >= FAST_HALF_MS) {
            fast_done++;
            if (fast_done >= FAST_PULSES) {
                fast_done = 0U;
                enter_state(MOORE_PAUSE, now);
            } else {
                enter_state(MOORE_FAST_ON, now);
            }
        }
        break;

    case MOORE_PAUSE:
        if ((uint32_t)(now - entered_ms) >= PAUSE_MS) {
            enter_state(MOORE_SLOW_ON, now);
        }
        break;

    default:
        slow_done = 0U;
        fast_done = 0U;
        enter_state(MOORE_PAUSE, now);
        break;
    }
}

int main(void)
{
    led_init();
    systimer_init_1ms();
    entered_ms = systimer_millis();
    apply_output(g_state);

    for (;;) {
        fsm_update();
        g_background_iterations++;
    }
}
