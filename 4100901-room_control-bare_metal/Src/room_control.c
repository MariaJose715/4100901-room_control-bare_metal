#include "room_control.h"
#include "gpios.h"
#include "systicks.h"
#include "uart.h"
#include "tim.h"

extern volatile uint32_t ms_counter;

// Estados de la sala
typedef enum {
    ROOM_IDLE,
    ROOM_OCCUPIED
} room_state_t;

static room_state_t current_state = ROOM_IDLE;
static uint32_t last_action_time = 0;

// --- Inicialización general ---
void room_control_app_init(void)
{
    current_state = ROOM_IDLE;
    last_action_time = 0;

    tim3_ch1_pwm_init(PWM_FREQUENCY);
    tim3_ch1_pwm_set_duty_cycle(0);

    clear_gpio(GPIOA, 5); // LED apagado
    uart_send_string("Room Control inicializado\r\n");
}

// --- Evento: Botón presionado ---
void room_control_on_button_press(void)
{
    if (current_state == ROOM_IDLE)
    {
        current_state = ROOM_OCCUPIED;
        set_gpio(GPIOA, 5);
        tim3_ch1_pwm_set_duty_cycle(100);
        uart_send_string("Estado: OCCUPIED\r\n");
    }
    else
    {
        current_state = ROOM_IDLE;
        clear_gpio(GPIOA, 5);
        tim3_ch1_pwm_set_duty_cycle(0);
        uart_send_string("Estado: IDLE\r\n");
    }

    last_action_time = ms_counter;
}

// --- Evento: Carácter recibido por UART ---
void room_control_on_uart_receive(char received_char)
{
    switch (received_char)
    {
        case 'h': case 'H':
            tim3_ch1_pwm_set_duty_cycle(100);
            set_gpio(GPIOA, 5);
            current_state = ROOM_OCCUPIED;
            uart_send_string("PWM = 100%\r\n");
            break;

        case 'l': case 'L':
            tim3_ch1_pwm_set_duty_cycle(0);
            clear_gpio(GPIOA, 5);
            current_state = ROOM_IDLE;
            uart_send_string("PWM = 0%\r\n");
            break;

        case 'o': case 'O':
            current_state = ROOM_OCCUPIED;
            set_gpio(GPIOA, 5);
            tim3_ch1_pwm_set_duty_cycle(100);
            uart_send_string("Sala ocupada\r\n");
            break;

        case 'i': case 'I':
            current_state = ROOM_IDLE;
            clear_gpio(GPIOA, 5);
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("Sala vacía\r\n");
            break;

        case '1':
            tim3_ch1_pwm_set_duty_cycle(10);
            set_gpio(GPIOA, 5);
            uart_send_string("PWM = 10%\r\n");
            break;

        case '5':
            tim3_ch1_pwm_set_duty_cycle(50);
            set_gpio(GPIOA, 5);
            uart_send_string("PWM = 50%\r\n");
            break;

        case '9':
            tim3_ch1_pwm_set_duty_cycle(90);
            set_gpio(GPIOA, 5);
            uart_send_string("PWM = 90%\r\n");
            break;

        default:
            uart_send(received_char); // eco
            break;
    }

    last_action_time = ms_counter;
}

// --- Actualización periódica ---
void room_control_update(void)
{
    if (current_state == ROOM_OCCUPIED &&
        (ms_counter - last_action_time >= LED_TIMEOUT_MS))
    {
        current_state = ROOM_IDLE;
        clear_gpio(GPIOA, 5);
        tim3_ch1_pwm_set_duty_cycle(0);
        uart_send_string("Timeout -> Estado: IDLE\r\n");
    }
}
