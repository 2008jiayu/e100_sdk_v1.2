#include <stdint.h>

#ifndef __INTRINSICS_H_
#define __INTRINSICS_H_

#ifndef __ASSEMBLY__

#define __ISB() __asm__ __volatile__ ("isb" : : : "memory")
#define __DSB() __asm__ __volatile__ ("dsb" : : : "memory")
#define __DMB() __asm__ __volatile__ ("dmb" : : : "memory")
#define __WFI() __asm__ __volatile__ ("wfi" : : : "memory")
#define __SEV() __asm__ __volatile__ ("sev" : : : "memory")
#define __WFE() __asm__ __volatile__ ("wfe" : : : "memory")

#ifndef __thumb__
static inline uint32_t __get_CPSR(void)
{
        register uint32_t temp = 0;
        __asm__ __volatile__ ("mrs      %0, CPSR" : "+r" (temp): : "memory", "cc" );
        return temp;
}

#define __set_CPSR(save_cpsr)                           \
        do {                                            \
                __asm__ __volatile__ (                  \
                                "msr     CPSR_cxsf, %0" \
                                :                       \
                                : "r"(save_cpsr)        \
                                : "memory", "cc" );     \
        } while (0)
#endif

typedef uint32_t __istate_t;

/* Enable interrupts */
#define __enable_interrupt()                            \
        do {                                            \
                __asm__ __volatile__ (                  \
                                "cpsie    if"           \
                                :                       \
                                :                       \
                                : "memory", "cc");      \
        } while (0)

/* Disable interrupts */
#define __disable_interrupt()                           \
        do {                                            \
                __asm__ __volatile__ (                  \
                                "cpsid    if"           \
                                :                       \
                                :                       \
                                : "memory", "cc" );     \
        } while (0)

/* Enable IRQ */
#define __enable_irq()                                  \
        do {                                            \
                __asm__ __volatile__ (                  \
                                "cpsie    i"            \
                                :                       \
                                :                       \
                                : "memory", "cc" );     \
        } while (0)

/* Disable IRQ */
#define __disable_irq()                                 \
        do {                                            \
                __asm__ __volatile__ (                  \
                                "cpsid    i"            \
                                :                       \
                                :                       \
                                : "memory", "cc" );     \
        } while (0)

/* Enable FIQ */
#define __enable_fiq()                                  \
        do {                                            \
                __asm__ __volatile__ (                  \
                                "cpsie    f"            \
                                :                       \
                                :                       \
                                : "memory", "cc" );     \
        } while (0)

/* Disable FIQ */
#define __disable_fiq()                                 \
        do {                                            \
                __asm__ __volatile__ (                  \
                                "cpsid    f"            \
                                :                       \
                                :                       \
                                : "memory", "cc" );     \
        } while (0)

#define __get_interrupt_state           __get_CPSR

/* Only restore interrupt state */
#define __set_interrupt_state(save_cpsr)                        \
        do {                                                    \
                register uint32_t temp = 0;                     \
                                                                \
                __asm__ __volatile__ (                          \
                                "mrs     %0, cpsr\n\t"          \
                                "bic     %0, %0, #0xc0\n\t"     \
                                "and     %1, %1, #0xc0\n\t"     \
                                "orr     %0, %0, %1\n\t"        \
                                "msr     cpsr_c, %0\n\t"        \
                                : "+r" (temp), "+r" (save_cpsr) \
                                :                \
                                : "memory", "cc"                \
                                );                              \
        } while (0)
#endif

#endif /* __INTRINSICS_H_ */
