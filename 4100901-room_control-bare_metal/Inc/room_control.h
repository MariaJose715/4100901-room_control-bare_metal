#ifndef ROOM_CONTROL_H
#define ROOM_CONTROL_H

#include <stdint.h>

#define LED_TIMEOUT_MS 3000  // Tiempo para apagar LED después de presionar botón
#define PWM_INITIAL_DUTY 0   // PWM apagado al inicio

void room_control_on_button_press(void);
void room_control_on_uart_receive(char received_char);
void room_control_app_init(void);
void room_control_update(void);

#endif // ROOM_CONTROL_H
