#include "main.h"

unsigned char seconds;

void __interrupt() isr(void)
{
    /* Static variables for counting the interrupts */
    static unsigned int count1 = 0, count2 = 0, count3 = 0;
    
    /* Condition for timer0 interrupt */
    if (TMR0IF == 1)
    {
        /* TMR0 Register value + 6 (offset count to get 250 ticks) + 1 Instruction Cycle */
        /* In-place addition requires 1 instruction cycle */
        TMR0 += 7;
        
        /* When interrupt flag is SET for 625 times toggle the LED1 */
        if (++count1 == 1250)
        {
            count1 = 0;
            if(++seconds > 20)
            {
                seconds = 0;
            }
        }
        
        /* RESET the flag bit */
        TMR0IF = 0;
    }
}