/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include "portable.h"

#define MAX_TIME 0xFFFFFFFF

static uint32 m_msTicks = 0;

CY_ISR(PERIODIC_TIMER_ISR) 
{
    m_msTicks++;
    isr_1_ClearPending();
    PeriodicTimer_ClearInterrupt(PeriodicTimer_INTR_MASK_TC);
}

void Timer_Init(void)
{
    PeriodicTimer_Start();
    isr_1_StartEx(PERIODIC_TIMER_ISR);
}

TIME Timer_ElapsedTime( TIME nOldTime )
{
    TIME nCalcTime;
    if ( nOldTime > m_msTicks )                             // clock rolled over
    {
        nCalcTime = ( MAX_TIME - nOldTime ) + m_msTicks + 1;
    }
    else                                                    // clock has not rolled over
    {
        nCalcTime = m_msTicks - nOldTime;
    }
    return( nCalcTime );
}


/* [] END OF FILE */
