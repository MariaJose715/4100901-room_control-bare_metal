// --- Ejemplo de botón y LED LD2 en STM32F476RGTx -----------------------------
    .section .text
    .syntax unified
    .thumb

    .global main
    .global init_led
    .global init_button
    .global init_systick
    .global SysTick_Handler

// --- Definiciones de registros para LD2 y Botón (Ver RM0351) -----------------
    .equ RCC_BASE,       0x40021000         @ Base de RCC
    .equ RCC_AHB2ENR,    RCC_BASE + 0x4C    @ Enable GPIOA clock (AHB2ENR)
    .equ RCC_AHB2ENR_GPIOC, (1 << 2)      @ Bit para GPIOC. controla qué periféricos de la AHB2 bus tienen habilitado su reloj
    .equ GPIOA_BASE,     0x48000000         @ Base de GPIOA
    .equ GPIOA_MODER,    GPIOA_BASE + 0x00  @ Mode register
    .equ GPIOA_ODR,      GPIOA_BASE + 0x14  @ Output data register
    .equ LD2_PIN,        5   
// --- Definiciones de registros para botón B1 (PC13) --------------------------
    .equ GPIOC_BASE,     0x48000800         @ Base de GPIOC
    .equ GPIOC_MODER,    GPIOC_BASE + 0x00  @ Mode register
    .equ GPIOC_IDR,      GPIOC_BASE + 0x10  @ Input data register
    .equ BUTTON_PIN,     13                 @ Pin del botón B1 (PC13)

// --- Definiciones de registros para SysTick (Ver PM0214) ---------------------
    .equ SYST_CSR,       0xE000E010         @ Control and status
    .equ SYST_RVR,       0xE000E014         @ Reload value register
    .equ SYST_CVR,       0xE000E018         @ Current value register
    .equ HSI_FREQ,       4000000            @ Reloj interno por defecto (4 MHz)

// --- Programa principal ------------------------------------------------------
main:
    bl init_led
    bl init_button
    bl init_systick

main_loop:
    // Espera a que el botón sea presionado (PC13 = 0)
wait_press: @ 
    movw  r0, #:lower16:GPIOC_IDR @Construye la dirección del registro
    movt  r0, #:upper16:GPIOC_IDR
    ldr   r1, [r0]
    tst   r1, #(1 << BUTTON_PIN)    @ ¿PC13 está en alto?hace un AND con el bit 13 (PC13). bit = 1 → botón NO presionado
    bne   wait_press                @ Si está en alto, sigue esperando

    // Encender LED LD2 (PA5)
    movw  r0, #:lower16:GPIOA_ODR
    movt  r0, #:upper16:GPIOA_ODR
    ldr   r1, [r0]
    orr   r1, r1, #(1 << LD2_PIN) @pone en 1 el bit 5 (LED ON).
    str   r1, [r0]

    // Esperar 3 segundos usando SysTick
    movs  r2, #3                     @ 3 ciclos de 1 segundo
wait_3s:
    wfi                              @ Espera interrupción SysTick
    subs  r2, r2, #1
    bne   wait_3s

    // Apagar LED LD2 (PA5)
    movw  r0, #:lower16:GPIOA_ODR
    movt  r0, #:upper16:GPIOA_ODR
    ldr   r1, [r0]
    bic   r1, r1, #(1 << LD2_PIN)
    str   r1, [r0]

    // Espera a que el botón sea liberado (PC13 = 1)@
wait_release:
    movw  r0, #:lower16:GPIOC_IDR
    movt  r0, #:upper16:GPIOC_IDR
    ldr   r1, [r0]
    tst   r1, #(1 << BUTTON_PIN)
    beq   wait_release @Si PC13 sigue en 0 (presionado), repite el bucle.

    b main_loop
              
// --- Inicialización de GPIOA PA5 para el LED LD2 -----------------------------
init_led:
    movw  r0, #:lower16:RCC_AHB2ENR
    movt  r0, #:upper16:RCC_AHB2ENR
    ldr   r1, [r0]
    orr   r1, r1, #(1 << 0)                @ Habilita reloj GPIOA
    str   r1, [r0]

    movw  r0, #:lower16:GPIOA_MODER
    movt  r0, #:upper16:GPIOA_MODER
    ldr   r1, [r0]
    bic   r1, r1, #(0b11 << (LD2_PIN * 2)) @ Limpia bits MODER5
    orr   r1, r1, #(0b01 << (LD2_PIN * 2)) @ PA5 como salida
    str   r1, [r0]
    bx    lr

// --- Inicialización de GPIOC PC13 como entrada (Botón B1) --------------------
init_button:
    movw  r0, #:lower16:RCC_AHB2ENR
    movt  r0, #:upper16:RCC_AHB2ENR
    ldr   r1, [r0]
    orr   r1, r1, #RCC_AHB2ENR_GPIOC   @ Habilita reloj del puerto GPIOC
    str   r1, [r0]

    movw  r0, #:lower16:GPIOC_MODER
    movt  r0, #:upper16:GPIOC_MODER
    ldr   r1, [r0]
    bic   r1, r1, #(0b11 << (BUTTON_PIN * 2)) @ PC13 como entrada (00)
    str   r1, [r0]
    bx    lr

// --- Inicialización de Systick para 1 s --------------------------------------
init_systick:
    movw  r0, #:lower16:SYST_RVR
    movt  r0, #:upper16:SYST_RVR
    movw  r1, #:lower16:HSI_FREQ
    movt  r1, #:upper16:HSI_FREQ
    subs  r1, r1, #1                       @ reload = 4 000 000 - 1
    str   r1, [r0]

    movw  r0, #:lower16:SYST_CSR
    movt  r0, #:upper16:SYST_CSR
    movs  r1, #(1 << 0)|(1 << 1)|(1 << 2)  @ ENABLE=1, TICKINT=1, CLKSOURCE=1
    str   r1, [r0]
    bx    lr

// --- Manejador de la interrupción SysTick ------------------------------------
    .thumb_func                            @ Ensure Thumb bit
SysTick_Handler:

    bx    lr                               @ Solo sirve para despertar con wfi
