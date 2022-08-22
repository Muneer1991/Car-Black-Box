#include <xc.h>
#include "digital_keypad.h"

/* Function to initialize keypad */
void init_digital_keypad(void)
{
    /* Set Keypad Port as input */
    KEYPAD_PORT_DDR = KEYPAD_PORT_DDR | INPUT_LINES;
}

/* Function to read keypad */
unsigned char read_digital_keypad(unsigned char mode)
{
    /* Flag used for edge trigger */
    static unsigned char once = 1;
    
    /* Condition for level detection */
    if (mode == LEVEL_DETECTION)
    {
        /* return the port bits to key */
        return KEYPAD_PORT & INPUT_LINES;
    }
    /* Condition for edge triggering */
    else
    {
        /* Condition when switch is pressed */
        if (((KEYPAD_PORT & INPUT_LINES) != ALL_RELEASED) && once)
        {
            once = 0;
            
            /* return the port bits to key */
            return KEYPAD_PORT & INPUT_LINES;
        }
        /* Condition when switch is not pressed */
        else if ((KEYPAD_PORT & INPUT_LINES) == ALL_RELEASED)
        {
            once = 1;
        }
    }
    
    /* If no conditions are true the return no buttons are pressed */
    return ALL_RELEASED;
}
