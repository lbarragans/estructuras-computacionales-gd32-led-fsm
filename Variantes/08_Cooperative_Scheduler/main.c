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

typedef void (*task_fn_t)(uint32_t now);

typedef struct {
    task_fn_t fn;
    uint32_t period_ms;
    uint32_t last_run_ms;
} task_t;

volatile fsm_state_t g_state = FSM_SLOW;
volatile uint32_t g_background_runs = 0U;
volatile uint32_t g_diagnostics_runs = 0U;

static uint32_t fsm_last_event_ms = 0U;
static uint8_t completed_pulses = 0U;

static void fsm_task(uint32_t now)
{
    if (g_state == FSM_PAUSE) {
        if ((uint32_t)(now - fsm_last_event_ms) >= PAUSE_MS) {
            g_state = FSM_SLOW;
            fsm_last_event_ms = now;
        }
        return;
    }

    const uint32_t half_ms =
        (g_state == FSM_SLOW) ? SLOW_HALF_MS : FAST_HALF_MS;
    const uint8_t target =
        (g_state == FSM_SLOW) ? SLOW_PULSES : FAST_PULSES;

    if ((uint32_t)(now - fsm_last_event_ms) < half_ms) {
        return;
    }

    fsm_last_event_ms = now;
    led_toggle();

    if (led_on_flag == 0U) {
        completed_pulses++;
    }

    if (completed_pulses >= target) {
        completed_pulses = 0U;

        if (g_state == FSM_SLOW) {
            g_state = FSM_FAST;
        } else {
            g_state = FSM_PAUSE;
            led_set(0U);
        }
    }
}

static void background_task(uint32_t now)
{
    (void)now;
    g_background_runs++;
}

static void diagnostics_task(uint32_t now)
{
    (void)now;
    g_diagnostics_runs++;
}

static task_t tasks[] = {
    {fsm_task,         1U,   0U},
    {background_task, 10U,  0U},
    {diagnostics_task, 100U, 0U}
};

static void scheduler_run(uint32_t now)
{
    const uint32_t count = sizeof(tasks) / sizeof(tasks[0]);

    for (uint32_t i = 0U; i < count; ++i) {
        if ((uint32_t)(now - tasks[i].last_run_ms) >= tasks[i].period_ms) {
            tasks[i].last_run_ms = now;
            tasks[i].fn(now);
        }
    }
}

int main(void)
{
    led_init();
    systimer_init_1ms();

    const uint32_t now = systimer_millis();
    fsm_last_event_ms = now;

    for (uint32_t i = 0U; i < (sizeof(tasks) / sizeof(tasks[0])); ++i) {
        tasks[i].last_run_ms = now;
    }

    for (;;) {
        scheduler_run(systimer_millis());
    }
}
