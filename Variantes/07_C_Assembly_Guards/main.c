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

#include "fsm_guards.h"

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
    FSM_FAST = 1,
    FSM_PAUSE = 2
} fsm_state_t;

volatile fsm_state_t g_state = FSM_SLOW;
volatile uint32_t g_guard_result = 0U;
volatile uint32_t g_background_iterations = 0U;

static uint32_t last_event_ms = 0U;
static uint8_t completed_pulses = 0U;

static void enter_state(fsm_state_t next, uint32_t now)
{
    g_state = next;
    completed_pulses = 0U;
    last_event_ms = now;

    if (next == FSM_PAUSE) {
        led_set(0U);
    }
}

static void fsm_update(void)
{
    const uint32_t now = systimer_millis();
    const uint32_t elapsed = (uint32_t)(now - last_event_ms);

    if (g_state == FSM_SLOW || g_state == FSM_FAST) {
        const uint32_t half_ms =
            (g_state == FSM_SLOW) ? SLOW_HALF_MS : FAST_HALF_MS;

        if (elapsed < half_ms) {
            return;
        }

        last_event_ms = now;
        led_toggle();

        if (led_on_flag == 0U) {
            completed_pulses++;
        }

        g_guard_result = fsm_transition_ready_asm(
            (uint32_t)g_state, completed_pulses, 0U
        );

        if (g_guard_result != 0U) {
            enter_state(
                (g_state == FSM_SLOW) ? FSM_FAST : FSM_PAUSE,
                now
            );
        }
        return;
    }

    if (g_state == FSM_PAUSE) {
        g_guard_result = fsm_transition_ready_asm(
            (uint32_t)g_state, 0U, elapsed
        );

        if (g_guard_result != 0U) {
            enter_state(FSM_SLOW, now);
        }
        return;
    }

    led_set(0U);
    enter_state(FSM_SLOW, now);
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
