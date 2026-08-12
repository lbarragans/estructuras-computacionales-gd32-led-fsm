#include <stdint.h>

#include "gd32vw55x.h"
#include "systimer.h"

#define LED_GPIO_PORT            GPIOC
#define LED_GPIO_PIN             GPIO_PIN_13
#define LED_GPIO_CLOCK           RCU_GPIOC

#define SLOW_HALF_PERIOD_MS      500U
#define FAST_HALF_PERIOD_MS      150U
#define PAUSE_DURATION_MS        2000U
#define SLOW_PULSE_COUNT         3U
#define FAST_PULSE_COUNT         5U

typedef enum {
    FSM_SLOW_BLINK = 0,
    FSM_FAST_BLINK,
    FSM_PAUSE
} fsm_state_t;

volatile fsm_state_t g_fsm_state = FSM_SLOW_BLINK;
volatile uint32_t g_transition_count = 0U;
volatile uint32_t g_completed_cycles = 0U;
volatile uint32_t g_led_toggle_count = 0U;
volatile uint32_t g_background_iterations = 0U;
volatile uint8_t g_completed_pulses = 0U;
volatile uint8_t g_led_is_on = 0U;

static uint32_t last_event_ms = 0U;

static void led_init(void)
{
    rcu_periph_clock_enable(LED_GPIO_CLOCK);

    gpio_mode_set(
        LED_GPIO_PORT,
        GPIO_MODE_OUTPUT,
        GPIO_PUPD_NONE,
        LED_GPIO_PIN
    );

    gpio_output_options_set(
        LED_GPIO_PORT,
        GPIO_OTYPE_PP,
        GPIO_OSPEED_10MHZ,
        LED_GPIO_PIN
    );

    /* En esta placa el LED de PC13 es activo en nivel bajo. */
    gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
    g_led_is_on = 0U;
}

static void led_set(uint8_t turn_on)
{
    if (turn_on != 0U) {
        gpio_bit_reset(LED_GPIO_PORT, LED_GPIO_PIN);
        g_led_is_on = 1U;
    } else {
        gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
        g_led_is_on = 0U;
    }
}

static void led_toggle(void)
{
    led_set((uint8_t)(g_led_is_on == 0U));
    g_led_toggle_count++;
}

static void enter_state(fsm_state_t next_state, uint32_t now_ms)
{
    g_fsm_state = next_state;
    g_completed_pulses = 0U;
    last_event_ms = now_ms;
    g_transition_count++;

    if (next_state == FSM_PAUSE) {
        led_set(0U);
    }
}

static void update_blink_state(
    uint32_t now_ms,
    uint32_t half_period_ms,
    uint8_t required_pulses,
    fsm_state_t next_state
)
{
    if ((uint32_t)(now_ms - last_event_ms) < half_period_ms) {
        return;
    }

    last_event_ms = now_ms;
    led_toggle();

    /* Un pulso se completa cuando el LED vuelve al estado apagado. */
    if (g_led_is_on == 0U) {
        g_completed_pulses++;

        if (g_completed_pulses >= required_pulses) {
            enter_state(next_state, now_ms);
        }
    }
}

static void fsm_update(void)
{
    uint32_t now_ms = systimer_millis();

    switch (g_fsm_state) {
    case FSM_SLOW_BLINK:
        update_blink_state(
            now_ms,
            SLOW_HALF_PERIOD_MS,
            SLOW_PULSE_COUNT,
            FSM_FAST_BLINK
        );
        break;

    case FSM_FAST_BLINK:
        update_blink_state(
            now_ms,
            FAST_HALF_PERIOD_MS,
            FAST_PULSE_COUNT,
            FSM_PAUSE
        );
        break;

    case FSM_PAUSE:
        if ((uint32_t)(now_ms - last_event_ms) >= PAUSE_DURATION_MS) {
            g_completed_cycles++;
            enter_state(FSM_SLOW_BLINK, now_ms);
        }
        break;

    default:
        /* Recuperacion determinista si el estado se corrompe. */
        led_set(0U);
        enter_state(FSM_SLOW_BLINK, now_ms);
        break;
    }
}

static void background_task_update(void)
{
    /* Prueba visible de que la temporizacion no bloquea la CPU. */
    g_background_iterations++;
}

int main(void)
{
    led_init();
    systimer_init_1ms();
    last_event_ms = systimer_millis();

    while (1) {
        fsm_update();
        background_task_update();
    }
}
