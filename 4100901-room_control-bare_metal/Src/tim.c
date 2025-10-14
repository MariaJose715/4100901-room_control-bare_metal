#include "tim.h"
#include "rccs.h"
#include "gpios.h"

void tim3_ch1_pwm_init(uint32_t pwm_freq_hz)
{
    // PA6 como Alternate Function (AF2)
    gpio_setup_pin(GPIOA, 6, GPIO_MODE_AF, 2);

    // Habilitar reloj TIM3
    rcc_tim3_clock_enable();

    // Configuración de temporizador
    TIM3->PSC = 100 - 1; // Prescaler -> 4MHz / 100 = 40kHz
    TIM3->ARR = (TIM_PCLK_FREQ_HZ / 100 / pwm_freq_hz) - 1;

    // PWM mode 1 en CH1 + preload
    TIM3->CCMR1 &= ~((7U << 4) | (1U << 3));
    TIM3->CCMR1 |= ((6U << 4) | (1U << 3));

    TIM3->CCR1 = 0;
    TIM3->CCER |= (1U << 0);  // habilita salida CH1

    TIM3->CR1 |= (1U << 7);   // ARPE
    TIM3->EGR = 1U;           // update event
    TIM3->CR1 |= 1U;          // habilita contador
}

void tim3_ch1_pwm_set_duty_cycle(uint8_t duty_cycle_percent)
{
    if (duty_cycle_percent > 100)
        duty_cycle_percent = 100;

    uint32_t arr = TIM3->ARR + 1;
    TIM3->CCR1 = (arr * duty_cycle_percent) / 100U;
}
