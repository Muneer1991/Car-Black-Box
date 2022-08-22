/*
 * File:   main.c
 * Author: Muneer Mohammad Ali
 * Description: Car Black Box (Project)
 *
 * Created on August 16, 2022, 9:25 PM
 */


#include "main.h"

#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT disabled) 


unsigned char *gear[] = {"GN", "GR", "G1", "G2", "G3", "G4", "G5"};
signed char gr = -1;
unsigned char event[3] = "ON";
unsigned char speed = 0;  

static void init_config(void) 
{
    init_i2c(100000);
    init_ds1307();
    
    init_clcd();
    init_adc();
    init_digital_keypad();    
    init_timer0();
    
    init_uart(9600);
    uart_puts("Project Started\n");
    
    /* Enabling peripheral interrupt */
    PEIE = 1;
    
    /* Enable all the Global Interrupts */
    GIE = 1;
}

void main(void) 
{
    init_config();
    
    
    unsigned char control_screen = DASH_BOARD_FLAG;
    unsigned char key;
    
    eeprom_at24c04_str_write(0x00, "1111");
    speed = read_adc()/10.333333;
    
    log_car_event(event, speed);
    
    while (1) 
    {
        speed = read_adc()/10.333333;
        
        key = read_digital_keypad(STATE);
        
        if(key == SW1)
        {
            strcpy(event,"C ");
            log_car_event(event, speed);
        }
        else if(key == SW2)
        {
            if(++gr == 7)
            {
                gr = 6;
            }
            strcpy(event,gear[gr]);
            log_car_event(event, speed);
            
        }
        else if(key == SW3)
        {
            if(gr == -1 || --gr == -1)
            {
                gr = 0;
            }
            strcpy(event,gear[gr]);
            log_car_event(event, speed);
        }
        else if((key == SW4 || key == SW5) && control_screen == DASH_BOARD_FLAG)
        {
            clcd_clear();
            clcd_print(" ENTER PASSWORD", LINE1(0));
            clcd_write(CURSOR_POS, INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            control_screen = LOGIN_FLAG;
        }
        
        if(control_screen == DASH_BOARD_FLAG)
        {
            display_dash_board(event, speed);
        }
        else if(control_screen == LOGIN_FLAG)
        {
            display_login();
            control_screen = DASH_BOARD_FLAG;
        }
    }
}
