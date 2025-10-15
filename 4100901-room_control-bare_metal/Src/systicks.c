#include "systicks.h"
#include "gpios.h"

volatile uint32_t ms_counter = 0;

void init_systick(void)
{
    SYST->RVR = 4000000 / 1000 - 1;   // 4 MHz → 1 ms
    SYST->CVR = 0;
    SYST->CSR = (1 << 0) | (1 << 1) | (1 << 2);  // ENABLE, TICKINT, CLKSOURCE
}

void SysTick_Handler(void)
{
    ms_counter++;

    static uint32_t hb = 0;
    if (++hb >= 500) {        // Toggle cada 500 ms
        GPIOA->ODR ^= (1U << 5);  // LD2 (PA5) -> parpadeo de latido
        hb = 0;
    }
}

void delay_ms(uint32_t ms)
{
    uint32_t start = ms_counter;
    while ((ms_counter - start) < ms)
        __asm volatile("wfi");
}
