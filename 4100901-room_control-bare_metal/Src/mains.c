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

    // --- 2. Configuración de GPIOs ---
    init_gpio(GPIOA, 5, 0x01, 0x00, 0x01, 0x00, 0x00);   // LED PA5 salida push-pull
    init_gpio(GPIOC, 13, 0x00, 0x00, 0x01, 0x01, 0x00);  // Botón PC13 entrada con pull-up

    // --- 3. Inicialización de SysTick y UART ---
    init_systick();     // genera interrupciones cada 1 ms
    init_gpio_uart();   // configura PA2/PA3 (AF7)
    init_uart();        // 115200 8N1

    // --- 4. Inicialización de la aplicación ROOM_CONTROL ---
    room_control_app_init();

    // --- 5. Habilitación de interrupciones ---
    nvic_exti_pc13_button_enable();  // interrupción del botón B1
    nvic_usart2_irq_enable();        // interrupción de recepción UART

    uart_send_string("Sistema Inicializado!\r\n");

    // --- 6. Bucle principal ---
    while (1)
    {
        room_control_update();  // control periódico de la sala
        __asm volatile("wfi");  // espera a interrupción para ahorro de energía
    }
}
