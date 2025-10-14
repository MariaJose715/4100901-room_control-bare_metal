#include "rccs.h"

void rcc_init(void)
{
    RCC->AHB2ENR |= (1 << 0);                      // Habilita reloj GPIOA
    RCC->AHB2ENR |= (1 << 2);                      // Habilita reloj GPIOC
}
void rcc_syscfg_clock_enable(void) {
    RCC->APB2ENR |= (1U << 0); // Habilitar reloj SYSCFG (bit 0)
}
void rcc_tim3_clock_enable(void)
{
    // TIM3 clock enable on APB1ENR1 bit assumed to be 1
    RCC->APB1ENR1 |= (1U << 1);
}