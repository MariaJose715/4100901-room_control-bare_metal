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

    uart_send_string("Room Control inicializado\r\n");
}

// --- Evento: Botón presionado ---
void room_control_on_button_press(void)
{
    if (current_state == ROOM_IDLE)
    {
        current_state = ROOM_OCCUPIED;
        tim3_ch1_pwm_set_duty_cycle(100);
        uart_send_string("Estado: OCCUPIED\r\n");
    }
    else
    {
        current_state = ROOM_IDLE;
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
            current_state = ROOM_OCCUPIED;
            uart_send_string("PWM = 100%\r\n");
            break;

        case '1': 
            tim3_ch1_pwm_set_duty_cycle(10);
             current_state = ROOM_OCCUPIED; 
             uart_send_string("PWM = 10%\r\n"); 
             break;

        case '2':
            tim3_ch1_pwm_set_duty_cycle(20);
            current_state = ROOM_OCCUPIED;
            uart_send_string("PWM = 20%\r\n");
            break;
        case '3':
            tim3_ch1_pwm_set_duty_cycle(30);
            current_state = ROOM_OCCUPIED;
            uart_send_string("PWM = 30%\r\n");
            break;
        case '4':
            tim3_ch1_pwm_set_duty_cycle(40);
            current_state = ROOM_OCCUPIED;
            uart_send_string("PWM = 40%\r\n");
            break;
        case '5':
            tim3_ch1_pwm_set_duty_cycle(50);
            current_state = ROOM_OCCUPIED;
            uart_send_string("PWM = 50%\r\n");
            break;
        case '6':
            tim3_ch1_pwm_set_duty_cycle(60);
            current_state = ROOM_OCCUPIED;
            uart_send_string("PWM = 60%\r\n");
            break;
        case '7':
            tim3_ch1_pwm_set_duty_cycle(70);
            current_state = ROOM_OCCUPIED;
            uart_send_string("PWM = 70%\r\n");
            break;
        case '8':
            tim3_ch1_pwm_set_duty_cycle(80);
            current_state = ROOM_OCCUPIED;
            uart_send_string("PWM = 80%\r\n");
            break;
        case '9':
            tim3_ch1_pwm_set_duty_cycle(90);
            current_state = ROOM_OCCUPIED;
            uart_send_string("PWM = 90%\r\n");
            break;

        case 'l': case 'L':
            tim3_ch1_pwm_set_duty_cycle(0);
            current_state = ROOM_IDLE;
            uart_send_string("PWM = 0%\r\n");
            break;

        case 'o': case 'O':
            current_state = ROOM_OCCUPIED;
            tim3_ch1_pwm_set_duty_cycle(100);
            uart_send_string("La sala está ocupada (bombilla encendida)\r\n");
            break;

        case 'i': case 'I':
            current_state = ROOM_IDLE;
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("La sala está vacía (bombilla apagada)\r\n");
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
        tim3_ch1_pwm_set_duty_cycle(0);
        uart_send_string("Timeout -> Estado: IDLE\r\n");
    }
}
