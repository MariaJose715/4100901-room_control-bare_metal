// systick.h
#ifndef SYSTICKS_H
#define SYSTICKS_H

#include <stdint.h>

typedef struct{
    volatile uint32_t CSR;    // Control and Status Register
    volatile uint32_t RVR;    // Reload Value Register
    volatile uint32_t CVR;    // Current Value Register
    volatile uint32_t CALIB;  // Calibration Register
} SysTick_TypeDef;

/* Short macro `SYST` used throughout the codebase */
#define SYST_BASE   0xE000E010U
#define SYST        ((SysTick_TypeDef *) SYST_BASE)

void init_systick(void);
void delay_ms(uint32_t ms);
extern volatile uint32_t ms_counter;

#endif // SYSTICKS_H
