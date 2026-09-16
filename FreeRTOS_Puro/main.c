#include <stdint.h>

#include "gd32vw55x.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define LED_GPIO_PORT  GPIOC
#define LED_GPIO_PIN   GPIO_PIN_13
#define LED_GPIO_CLOCK RCU_GPIOC

typedef enum {
    FSM_SLOW = 0,
    FSM_FAST,
    FSM_PAUSE
} fsm_state_t;

typedef enum {
    EVT_HALF_PERIOD = 0,
    EVT_PAUSE_DONE
} fsm_event_t;

static QueueHandle_t event_queue;
static volatile fsm_state_t g_state = FSM_SLOW;
static uint8_t led_on_flag = 0U;
static uint8_t completed_pulses = 0U;

static void led_init(void)
{
    rcu_periph_clock_enable(LED_GPIO_CLOCK);
    gpio_mode_set(LED_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_GPIO_PIN);
    gpio_output_options_set(
        LED_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, LED_GPIO_PIN
    );
    gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
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

/*
 * Productor pedagogico:
 * consulta el estado y publica el evento temporal correspondiente.
 * Una integracion real puede reemplazarlo por software timers.
 */
static void event_producer_task(void *argument)
{
    (void)argument;

    for (;;) {
        fsm_event_t event;
        TickType_t delay_ticks;

        if (g_state == FSM_SLOW) {
            event = EVT_HALF_PERIOD;
            delay_ticks = pdMS_TO_TICKS(500U);
        } else if (g_state == FSM_FAST) {
            event = EVT_HALF_PERIOD;
            delay_ticks = pdMS_TO_TICKS(150U);
        } else {
            event = EVT_PAUSE_DONE;
            delay_ticks = pdMS_TO_TICKS(2000U);
        }

        vTaskDelay(delay_ticks);
        (void)xQueueSend(event_queue, &event, portMAX_DELAY);
    }
}

static void fsm_controller_task(void *argument)
{
    (void)argument;
    fsm_event_t event;

    for (;;) {
        if (xQueueReceive(event_queue, &event, portMAX_DELAY) != pdPASS) {
            continue;
        }

        switch (g_state) {
        case FSM_SLOW:
            if (event == EVT_HALF_PERIOD) {
                led_toggle();
                if (led_on_flag == 0U && ++completed_pulses >= 3U) {
                    completed_pulses = 0U;
                    g_state = FSM_FAST;
                }
            }
            break;

        case FSM_FAST:
            if (event == EVT_HALF_PERIOD) {
                led_toggle();
                if (led_on_flag == 0U && ++completed_pulses >= 5U) {
                    completed_pulses = 0U;
                    led_set(0U);
                    g_state = FSM_PAUSE;
                }
            }
            break;

        case FSM_PAUSE:
            if (event == EVT_PAUSE_DONE) {
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
}

int main(void)
{
    led_init();

    event_queue = xQueueCreate(8U, sizeof(fsm_event_t));
    if (event_queue == NULL) {
        for (;;) {
        }
    }

    (void)xTaskCreate(
        event_producer_task, "EventProducer",
        configMINIMAL_STACK_SIZE, NULL,
        tskIDLE_PRIORITY + 1U, NULL
    );

    (void)xTaskCreate(
        fsm_controller_task, "FSM",
        configMINIMAL_STACK_SIZE, NULL,
        tskIDLE_PRIORITY + 2U, NULL
    );

    vTaskStartScheduler();

    for (;;) {
    }
}
