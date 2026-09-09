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
    FSM_SLOW = 0,
    FSM_FAST,
    FSM_PAUSE
} fsm_state_t;

typedef enum {
    FSM_EVENT_NONE = 0,
    FSM_EVENT_HALF_PERIOD,
    FSM_EVENT_PAUSE_DONE
} fsm_event_t;

volatile fsm_state_t g_state = FSM_SLOW;
volatile uint32_t g_background_iterations = 0U;

static uint32_t last_event_ms = 0U;
static uint8_t completed_pulses = 0U;

static fsm_event_t generate_event(uint32_t now)
{
    uint32_t required_ms = PAUSE_MS;

    if (g_state == FSM_SLOW) {
        required_ms = SLOW_HALF_MS;
    } else if (g_state == FSM_FAST) {
        required_ms = FAST_HALF_MS;
    }

    if ((uint32_t)(now - last_event_ms) < required_ms) {
        return FSM_EVENT_NONE;
    }

    last_event_ms = now;

    if (g_state == FSM_PAUSE) {
        return FSM_EVENT_PAUSE_DONE;
    }

    return FSM_EVENT_HALF_PERIOD;
}

static void dispatch_event(fsm_event_t event)
{
    if (event == FSM_EVENT_NONE) {
        return;
    }

    switch (g_state) {
    case FSM_SLOW:
        if (event == FSM_EVENT_HALF_PERIOD) {
            led_toggle();
            if (led_on_flag == 0U) {
                completed_pulses++;
                if (completed_pulses >= SLOW_PULSES) {
                    completed_pulses = 0U;
                    g_state = FSM_FAST;
                }
            }
        }
        break;

    case FSM_FAST:
        if (event == FSM_EVENT_HALF_PERIOD) {
            led_toggle();
            if (led_on_flag == 0U) {
                completed_pulses++;
                if (completed_pulses >= FAST_PULSES) {
                    completed_pulses = 0U;
                    led_set(0U);
                    g_state = FSM_PAUSE;
                }
            }
        }
        break;

    case FSM_PAUSE:
        if (event == FSM_EVENT_PAUSE_DONE) {
            g_state = FSM_SLOW;
        }
        break;

    default:
        led_set(0U);
        completed_pulses = 0U;
        g_state = FSM_SLOW;
        break;
    }
}

int main(void)
{
    led_init();
    systimer_init_1ms();
    last_event_ms = systimer_millis();

    for (;;) {
        const uint32_t now = systimer_millis();
        const fsm_event_t event = generate_event(now);
        dispatch_event(event);
        g_background_iterations++;
    }
}
