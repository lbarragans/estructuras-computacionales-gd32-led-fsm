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
    FSM_SLOW_BLINK = 0,
    FSM_FAST_BLINK,
    FSM_PAUSE
} fsm_state_t;

volatile fsm_state_t g_state = FSM_SLOW_BLINK;
volatile uint32_t g_background_iterations = 0U;
volatile uint32_t g_completed_cycles = 0U;
volatile uint8_t g_completed_pulses = 0U;

static uint32_t last_event_ms = 0U;

static void enter_state(fsm_state_t next, uint32_t now)
{
    g_state = next;
    g_completed_pulses = 0U;
    last_event_ms = now;

    if (next == FSM_PAUSE) {
        led_set(0U);
    }
}

static void update_blink(
    uint32_t now,
    uint32_t half_period,
    uint8_t required_pulses,
    fsm_state_t next
)
{
    if ((uint32_t)(now - last_event_ms) < half_period) {
        return;
    }

    last_event_ms = now;
    led_toggle();

    if (led_on_flag == 0U) {
        g_completed_pulses++;
        if (g_completed_pulses >= required_pulses) {
            enter_state(next, now);
        }
    }
}

static void fsm_update(void)
{
    const uint32_t now = systimer_millis();

    switch (g_state) {
    case FSM_SLOW_BLINK:
        update_blink(now, SLOW_HALF_MS, SLOW_PULSES, FSM_FAST_BLINK);
        break;
    case FSM_FAST_BLINK:
        update_blink(now, FAST_HALF_MS, FAST_PULSES, FSM_PAUSE);
        break;
    case FSM_PAUSE:
        if ((uint32_t)(now - last_event_ms) >= PAUSE_MS) {
            g_completed_cycles++;
            enter_state(FSM_SLOW_BLINK, now);
        }
        break;
    default:
        led_set(0U);
        enter_state(FSM_SLOW_BLINK, now);
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
