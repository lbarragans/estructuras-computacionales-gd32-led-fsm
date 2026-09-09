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
    TOP_BLINKING = 0,
    TOP_PAUSED
} top_state_t;

typedef enum {
    BLINK_SLOW = 0,
    BLINK_FAST
} blink_state_t;

volatile top_state_t g_top_state = TOP_BLINKING;
volatile blink_state_t g_blink_state = BLINK_SLOW;
volatile uint32_t g_background_iterations = 0U;

static uint32_t last_event_ms = 0U;
static uint8_t completed_pulses = 0U;

static void enter_pause(uint32_t now)
{
    g_top_state = TOP_PAUSED;
    completed_pulses = 0U;
    last_event_ms = now;
    led_set(0U);
}

static void blinking_superstate_update(uint32_t now)
{
    const uint32_t half_ms =
        (g_blink_state == BLINK_SLOW) ? SLOW_HALF_MS : FAST_HALF_MS;
    const uint8_t target =
        (g_blink_state == BLINK_SLOW) ? SLOW_PULSES : FAST_PULSES;

    if ((uint32_t)(now - last_event_ms) < half_ms) {
        return;
    }

    last_event_ms = now;
    led_toggle();

    if (led_on_flag != 0U) {
        return;
    }

    completed_pulses++;

    if (completed_pulses < target) {
        return;
    }

    completed_pulses = 0U;

    if (g_blink_state == BLINK_SLOW) {
        g_blink_state = BLINK_FAST;
    } else {
        g_blink_state = BLINK_SLOW;
        enter_pause(now);
    }
}

static void fsm_update(void)
{
    const uint32_t now = systimer_millis();

    switch (g_top_state) {
    case TOP_BLINKING:
        blinking_superstate_update(now);
        break;

    case TOP_PAUSED:
        if ((uint32_t)(now - last_event_ms) >= PAUSE_MS) {
            g_top_state = TOP_BLINKING;
            g_blink_state = BLINK_SLOW;
            last_event_ms = now;
        }
        break;

    default:
        led_set(0U);
        g_top_state = TOP_PAUSED;
        g_blink_state = BLINK_SLOW;
        last_event_ms = now;
        break;
    }
}

int main(void)
{
    led_init();
    systimer_init_1ms();
    last_event_ms = systimer_millis();

    for (;;) {
        fsm_update();
        g_background_iterations++;
    }
}
