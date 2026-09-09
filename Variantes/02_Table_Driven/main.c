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
    FSM_PAUSE,
    FSM_STATE_COUNT
} fsm_state_t;

typedef struct {
    uint32_t interval_ms;
    uint8_t pulses;
    fsm_state_t next;
    uint8_t is_blink;
} state_config_t;

static const state_config_t state_table[FSM_STATE_COUNT] = {
    [FSM_SLOW]  = {SLOW_HALF_MS, SLOW_PULSES, FSM_FAST, 1U},
    [FSM_FAST]  = {FAST_HALF_MS, FAST_PULSES, FSM_PAUSE, 1U},
    [FSM_PAUSE] = {PAUSE_MS,     0U,          FSM_SLOW, 0U}
};

volatile fsm_state_t g_state = FSM_SLOW;
volatile uint32_t g_background_iterations = 0U;
volatile uint32_t g_completed_cycles = 0U;

static uint32_t last_event_ms = 0U;
static uint8_t completed_pulses = 0U;

static void enter_state(fsm_state_t state, uint32_t now)
{
    g_state = state;
    completed_pulses = 0U;
    last_event_ms = now;

    if (state == FSM_PAUSE) {
        led_set(0U);
    }
}

static void fsm_update(void)
{
    const uint32_t now = systimer_millis();

    if ((uint32_t)g_state >= (uint32_t)FSM_STATE_COUNT) {
        led_set(0U);
        enter_state(FSM_SLOW, now);
        return;
    }

    const state_config_t *cfg = &state_table[g_state];

    if ((uint32_t)(now - last_event_ms) < cfg->interval_ms) {
        return;
    }

    last_event_ms = now;

    if (cfg->is_blink != 0U) {
        led_toggle();

        if (led_on_flag == 0U) {
            completed_pulses++;
            if (completed_pulses >= cfg->pulses) {
                enter_state(cfg->next, now);
            }
        }
    } else {
        g_completed_cycles++;
        enter_state(cfg->next, now);
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
