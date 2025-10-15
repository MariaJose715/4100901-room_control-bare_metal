#include "rccs.h"
#include "gpios.h"
#include "systicks.h"
#include "uart.h"
#include "tim.h"
#include "nvic.h"
#include "room_control.h"

extern volatile uint32_t ms_counter;

int main(void)
{
    // --- 1. Inicialización de relojes base ---
    rcc_init();

    // --- 2. GPIOs ---
    init_gpio(GPIOA, 5, 0x01, 0x00, 0x01, 0x00, 0x00);   // LD2 (heartbeat)
    init_gpio(GPIOC, 13, 0x00, 0x00, 0x01, 0x01, 0x00);  // Botón B1 (entrada con pull-up)

    // --- 3. SysTick + UART ---
    init_systick();     // genera interrupciones de 1 ms (también parpadea LED2)
    init_gpio_uart();   // PA2/PA3 AF7
    init_uart();        // 115200 8N1

    // --- 4. Inicializar aplicación ROOM CONTROL ---
    room_control_app_init(); // Configura PWM en PA6 y estado inicial

    // --- 5. Habilitar interrupciones ---
    nvic_exti_pc13_button_enable();  // Botón
    nvic_usart2_irq_enable();        // UART RX

    uart_send_string("Sistema Inicializado!\r\n");

    // --- 6. Bucle principal ---
    while (1)
    {
        room_control_update();  // lógica de timeout y control PWM
        __asm volatile("wfi");  // modo bajo consumo hasta próxima interrupción
    }
}
