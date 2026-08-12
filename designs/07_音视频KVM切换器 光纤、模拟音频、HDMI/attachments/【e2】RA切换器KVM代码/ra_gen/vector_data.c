/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [1] = ctsu_write_isr, /* CTSU WRITE (Write request interrupt) */
            [2] = ctsu_read_isr, /* CTSU READ (Measurement data transfer request interrupt) */
            [3] = ctsu_end_isr, /* CTSU END (Measurement end interrupt) */
        };
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            [1] = BSP_PRV_IELS_ENUM(EVENT_CTSU_WRITE), /* CTSU WRITE (Write request interrupt) */
            [2] = BSP_PRV_IELS_ENUM(EVENT_CTSU_READ), /* CTSU READ (Measurement data transfer request interrupt) */
            [3] = BSP_PRV_IELS_ENUM(EVENT_CTSU_END), /* CTSU END (Measurement end interrupt) */
        };
        #endif
