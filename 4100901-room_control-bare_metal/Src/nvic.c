#include "nvic.h"
#include "gpios.h"
#include "room_control.h"
#include "uart.h"
#include "systicks.h"

extern volatile uint32_t ms_counter;

// ---------------------------------------------------------------------
// Habilita EXTI13 (botón B1 en PC13)
// ---------------------------------------------------------------------
void nvic_exti_pc13_button_enable(void)
{
    // 1. Habilitar reloj SYSCFG (para configurar EXTI)
    RCC->APB2ENR |= (1U << 0);  // SYSCFG clock enable

    // 2. PC13 -> EXTI13: seleccionar puerto C en EXTICR4 (línea 13)
    SYSCFG->EXTICR[3] &= ~(0xFU << 4);
    SYSCFG->EXTICR[3] |=  (0x2U << 4);  // 0b0010 = Port C

    // 3. Habilitar interrupción línea 13
    EXTI->IMR1  |=  (1U << 13);
    // Flanco descendente (botón activo-bajo)
    EXTI->FTSR1 |=  (1U << 13);
    // Limpia flag de pendiente
    EXTI->PR1    =  (1U << 13);

    // 4. Habilitar IRQ EXTI15_10 en NVIC
    NVIC->ISER[1] = (1U << (EXTI15_10_IRQn - 32));
}

// ---------------------------------------------------------------------
// Habilita interrupción RX en USART2 (UART PA2/PA3)
// ---------------------------------------------------------------------
void nvic_usart2_irq_enable(void)
{
    // Habilita interrupción RX (RXNEIE)
    USART2->CR1 |= (1U << 5);

    // Habilita IRQ USART2 en NVIC
    NVIC->ISER[1] = (1U << (USART2_IRQn - 32));
}

// ---------------------------------------------------------------------
// ISR: Botón B1 (PC13) con anti-rebote (debounce)
// ---------------------------------------------------------------------
void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR1 & (1U << 13))  // ¿Pendiente en línea 13?
    {
        static uint32_t last_press_time = 0;
        uint32_t now = ms_counter;

        // Anti-rebote: ignorar pulsaciones dentro de 200 ms
        if ((now - last_press_time) > 200)
        {
            room_control_on_button_press();
            last_press_time = now;
        }

        // Limpiar flag de interrupción (write 1 to clear)
        EXTI->PR1 = (1U << 13);
    }
}

// ---------------------------------------------------------------------
// ISR: Recepción UART (USART2 RX)
// ---------------------------------------------------------------------
void USART2_IRQHandler(void)
{
    if (USART2->ISR & (1U << 5))  // RXNE flag
    {
        char received_char = (char)(USART2->RDR & 0xFF);  // leer limpia RXNE
        room_control_on_uart_receive(received_char);
    }
}
