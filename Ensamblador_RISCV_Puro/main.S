.section .text
    .align 2

    /*
     * Ejercicio 04
     * Maquina de estados no bloqueante en RISC-V puro.
     *
     * La tabla de vectores/startup del SDK sigue siendo infraestructura.
     * La logica de aplicacion no contiene codigo C.
     */

    /* RCU y GPIOC */
    .equ RCU_AHB1EN,          0x40023830

    .equ GPIOC_CTL,           0x40020800
    .equ GPIOC_OMODE,         0x40020804
    .equ GPIOC_OSPD,          0x40020808
    .equ GPIOC_PUD,           0x4002080C
    .equ GPIOC_BOP,           0x40020818
    .equ GPIOC_BC,            0x40020828
    .equ GPIOC_TG,            0x4002082C

    .equ GPIOC_CLOCK_BIT,     0x00000004
    .equ PC13_BIT,            0x00002000
    .equ PC13_FIELD_MASK,     0x0C000000
    .equ PC13_OUTPUT_MODE,    0x04000000
    .equ PC13_SPEED_10MHZ,    0x04000000

    /* SysTimer */
    .equ SYSTIMER_BASE,       0xD1000000
    .equ SYSTIMER_CMP_LO,     0xD1000008
    .equ SYSTIMER_CMP_HI,     0xD100000C
    .equ SYSTIMER_MTIMECTL,   0xD1000FF8
    .equ MTIMECTL_CMPCLREN,   0x00000002

    /* ECLIC, IRQ 7 */
    .equ ECLIC_INTIP7,        0xD200101C
    .equ ECLIC_INTIE7,        0xD200101D
    .equ ECLIC_INTATTR7,      0xD200101E
    .equ ECLIC_INTCTRL7,      0xD200101F

    .equ ECLIC_POS_EDGE,      0x02

    /* La biblioteca de firmware usada por este proyecto configura 160 MHz. */
    .equ SYSTEM_CORE_CLOCK_HZ, 160000000
    .equ SYSTIMER_COMPARE_1MS, 40000
    .equ SLOW_HALF_MS,         500
    .equ FAST_HALF_MS,         150
    .equ PAUSE_MS,             2000

    .global main
    .type main, @function

main:
    /* --------------------------------------------------------- */
    /* 1. Habilitar reloj GPIOC.                                 */
    /* --------------------------------------------------------- */
    li      t0, RCU_AHB1EN
    lw      t1, 0(t0)
    ori     t1, t1, GPIOC_CLOCK_BIT
    sw      t1, 0(t0)
    fence   iorw, iorw

    /* --------------------------------------------------------- */
    /* 2. Configurar PC13 como salida push-pull, 10 MHz, no pull.*/
    /* --------------------------------------------------------- */
    li      t0, GPIOC_CTL
    lw      t1, 0(t0)
    li      t2, PC13_FIELD_MASK
    not     t2, t2
    and     t1, t1, t2
    li      t2, PC13_OUTPUT_MODE
    or      t1, t1, t2
    sw      t1, 0(t0)

    li      t0, GPIOC_OMODE
    lw      t1, 0(t0)
    li      t2, PC13_BIT
    not     t2, t2
    and     t1, t1, t2
    sw      t1, 0(t0)

    li      t0, GPIOC_OSPD
    lw      t1, 0(t0)
    li      t2, PC13_FIELD_MASK
    not     t2, t2
    and     t1, t1, t2
    li      t2, PC13_SPEED_10MHZ
    or      t1, t1, t2
    sw      t1, 0(t0)

    li      t0, GPIOC_PUD
    lw      t1, 0(t0)
    li      t2, PC13_FIELD_MASK
    not     t2, t2
    and     t1, t1, t2
    sw      t1, 0(t0)

    /* LED activo en bajo: iniciar apagado. */
    li      t0, GPIOC_BOP
    li      t1, PC13_BIT
    sw      t1, 0(t0)

    /* --------------------------------------------------------- */
    /* 3. Configurar SysTimer para periodo de 1 ms.              */
    /*    160 MHz / 4000 = 40000                                */
    /* --------------------------------------------------------- */

    li      t0, SYSTIMER_MTIMECTL
    li      t1, MTIMECTL_CMPCLREN
    sw      t1, 0(t0)

    /* Escritura segura del compare de 64 bits en RV32. */
    li      t0, SYSTIMER_CMP_LO
    li      t2, -1
    sw      t2, 0(t0)

    li      t0, SYSTIMER_CMP_HI
    sw      zero, 0(t0)

    li      t0, SYSTIMER_CMP_LO
    li      t1, SYSTIMER_COMPARE_1MS
    sw      t1, 0(t0)

    /* --------------------------------------------------------- */
    /* 4. Configurar ECLIC IRQ 7.                               */
    /* --------------------------------------------------------- */

    /* Clear pending. */
    li      t0, ECLIC_INTIP7
    sb      zero, 0(t0)

    /* Rising edge, non-vector attribute. */
    li      t0, ECLIC_INTATTR7
    li      t1, ECLIC_POS_EDGE
    sb      t1, 0(t0)

    /* Level/prioridad = 0, como la referencia. */
    li      t0, ECLIC_INTCTRL7
    sb      zero, 0(t0)

    /* Enable IRQ 7. */
    li      t0, ECLIC_INTIE7
    li      t1, 1
    sb      t1, 0(t0)

    /* Habilitar interrupciones globales M-mode: mstatus.MIE = 1. */
    li      t0, 0x8
    csrs    mstatus, t0

    /* --------------------------------------------------------- */
    /* 5. Estado inicial: lento, cero pulsos, LED apagado.      */
    /* --------------------------------------------------------- */
    la      s0, system_milliseconds
    lw      s1, 0(s0)
    li      s2, 0                  /* 0=lento, 1=rapido, 2=pausa */
    li      s3, 0                  /* pulsos completos */
    li      s4, 0                  /* LED encendido */

.Lmain_loop:
    la      t3, g_background_iterations
    lw      t4, 0(t3)
    addi    t4, t4, 1
    sw      t4, 0(t3)

    lw      t0, 0(s0)
    sub     t1, t0, s1

    beqz    s2, .Lslow_period
    li      t2, 1
    beq     s2, t2, .Lfast_period
    li      t2, PAUSE_MS
    bltu    t1, t2, .Lmain_loop
    mv      s1, t0
    li      s2, 0
    li      s3, 0
    la      t3, g_completed_cycles
    lw      t4, 0(t3)
    addi    t4, t4, 1
    sw      t4, 0(t3)
    j       .Ltransition_done

.Lslow_period:
    li      t2, SLOW_HALF_MS
    j       .Lcheck_blink_time
.Lfast_period:
    li      t2, FAST_HALF_MS
.Lcheck_blink_time:
    bltu    t1, t2, .Lmain_loop
    mv      s1, t0
    li      t0, GPIOC_TG
    li      t1, PC13_BIT
    sw      t1, 0(t0)

    xori    s4, s4, 1
    la      t3, g_led_toggle_count
    lw      t4, 0(t3)
    addi    t4, t4, 1
    sw      t4, 0(t3)
    bnez    s4, .Lmain_loop

    addi    s3, s3, 1
    beqz    s2, .Lcheck_slow_count
    li      t2, 5
    bltu    s3, t2, .Lmain_loop
    li      s2, 2
    j       .Lenter_state
.Lcheck_slow_count:
    li      t2, 3
    bltu    s3, t2, .Lmain_loop
    li      s2, 1
.Lenter_state:
    li      s3, 0
.Ltransition_done:
    la      t3, g_transition_count
    lw      t4, 0(t3)
    addi    t4, t4, 1
    sw      t4, 0(t3)

    j       .Lmain_loop

    .size main, .-main


    /*
     * Handler llamado desde la infraestructura de interrupciones del startup.
     * El wrapper del startup administra el contexto de interrupcion.
     */
    .align 2
    .global eclic_mtip_handler
    .type eclic_mtip_handler, @function

eclic_mtip_handler:
    /* Limpiar pending IRQ 7. */
    li      t0, ECLIC_INTIP7
    sb      zero, 0(t0)

    /* system_milliseconds++ */
    la      t0, system_milliseconds
    lw      t1, 0(t0)
    addi    t1, t1, 1
    sw      t1, 0(t0)

    ret

    .size eclic_mtip_handler, .-eclic_mtip_handler


    .section .bss
    .align 2

    .global system_milliseconds
    .type system_milliseconds, @object

system_milliseconds:
    .word 0

    .size system_milliseconds, 4

    .global g_transition_count
    .global g_completed_cycles
    .global g_led_toggle_count
    .global g_background_iterations
g_transition_count:       .word 0
g_completed_cycles:       .word 0
g_led_toggle_count:       .word 0
g_background_iterations:  .word 0
