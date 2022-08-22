#include "main.h"

unsigned char clock_reg[3];
unsigned char time[7];  // "HHMMSS"
unsigned char log[11];
signed char pos = -1, event_count = -1;
unsigned char key, key2, attempt = 3;
unsigned char *menu[] = {"VIEW LOG", "DOWNLOAD LOG", "CLEAR LOG", "SET TIME", "CHANGE PSWRD"};
unsigned char password[5];
extern unsigned char seconds;
extern unsigned char *gear[];
extern signed char gr;
extern unsigned char event[3];
extern unsigned char speed;

static void get_time(void)
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
    
    //BCD Format 
    //clock_reg[0] = HH
    //clock_reg[1] = MM
    //clock_reg[2] = SS
     
    // HH -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';

    // MM 
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';

    // SS
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    
    time[6] = '\0';
}

static void display_time(void)
{
    
    get_time();
    
    clcd_putch(time[0], LINE2(0)); // HH:MM:SS
    clcd_putch(time[1], LINE2(1)); // HH:MM:SS
    clcd_putch(':', LINE2(2)); // HH:MM:SS
    clcd_putch(time[2], LINE2(3)); // HH:MM:SS
    clcd_putch(time[3], LINE2(4)); // HH:MM:SS
    clcd_putch(':', LINE2(5)); // HH:MM:SS
    clcd_putch(time[4], LINE2(6)); // HH:MM:SS
    clcd_putch(time[5], LINE2(7)); // HH:MM:SS
}

void display_dash_board(unsigned char *event, unsigned char speed)
{
    clcd_print("TIME     EVT SPD",LINE1(0));
    get_time();
    display_time();
    clcd_print(event, LINE2(9));
    clcd_putch((speed/10) + '0', LINE2(13));
    clcd_putch((speed % 10) + '0', LINE2(14));
}

void log_event(unsigned char *log)
{
    unsigned char addr;
    pos++;
    
    if(pos > 9)
    {
        pos = 0;
    }
    
    addr = pos * 10 + 5;    //10 --> 10 logs ; 5 --> password
    
    eeprom_at24c04_str_write(addr, log);
    
    if(event_count < 9)
    {
        event_count++;
    }

}
void log_car_event(unsigned char *event, unsigned char speed)
{
    get_time();
    
    strcpy(log, time);
    
    strncpy(log, time, 6);
    strncpy(&log[6], event, 2);
    log[8] = speed/10 + '0';
    log[9] = (speed % 10) + '0';
    log[10] = '\0';  
    
    log_event(log);
}

void clcd_clear(void)
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
}

char view_log(void)
{
    signed char index = 0;
    clcd_clear();
    while(1)
    {
        
        key2 = read_digital_keypad(LEVEL);
        key = read_digital_keypad(STATE);
        
        seconds = 0;
        while(key2 == SW5)
        {
            key2 = read_digital_keypad(LEVEL);
            if(seconds > 2)
            {
                clcd_clear();
                return DASH_BOARD;
            }
        }
        while(key2 == SW4)
        {
            key2 = read_digital_keypad(LEVEL);
            if(seconds > 2)
            {
                clcd_clear();
                return MENU;
            }
        }
        
        if(event_count != -1)
        {
            clcd_print("LOGS",LINE1(6));
            clcd_putch(index + '0', LINE2(0));
            
            /* read the time logs */
            clcd_putch(eeprom_at24c04_random_read((index * 10) + 0x05), LINE2(2)); // HH:MM:SS
            clcd_putch(eeprom_at24c04_random_read((index * 10) + 0x06), LINE2(3)); // HH:MM:SS
            clcd_putch(':', LINE2(4)); // HH:MM:SS
            clcd_putch(eeprom_at24c04_random_read((index * 10) + 0x07), LINE2(5)); // HH:MM:SS
            clcd_putch(eeprom_at24c04_random_read((index * 10) + 0x08), LINE2(6)); // HH:MM:SS
            clcd_putch(':', LINE2(7)); // HH:MM:SS
            clcd_putch(eeprom_at24c04_random_read((index * 10) + 0x09), LINE2(8)); // HH:MM:SS
            clcd_putch(eeprom_at24c04_random_read((index * 10) + 0x0A), LINE2(9)); // HH:MM:SS

            /* read the event logs */
            clcd_putch(eeprom_at24c04_random_read((index * 10) + 0x0B), LINE2(11));
            clcd_putch(eeprom_at24c04_random_read((index * 10) + 0x0C), LINE2(12));

            /* read the speed logs */
            clcd_putch(eeprom_at24c04_random_read((index * 10) + 0x0D), LINE2(14));
            clcd_putch(eeprom_at24c04_random_read((index * 10) + 0x0E), LINE2(15));
        }
        else
        {
            clcd_clear();
            clcd_print("NO LOGS TO SHOW",LINE1(0));
            clcd_print(" CREATE EVENTS",LINE2(0));
            __delay_ms(2000);
            clcd_clear();
            return MENU;
        }
        

        if(key == SW4)
        {
            if(index <= event_count)
            {
                if(++index == event_count+1)
                {
                    index = 0;
                }
            }
        }
        else if(key == SW5)
        {
            if(--index == -1 && event_count == 9)
            {
                index = 9;
            }
            else if(index == -1)
            {
                index = event_count;
            }
        }    
        else if(key == SW1)
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
    }    
}

char download_log(void)
{
    
    if(event_count != -1)
    {
        uart_puts("LOGS:\n");
        uart_puts("# TIME    EVENT  SPEED\n");
        for(int i = 0; i <= event_count; i++)
        {
            uart_putchar(i+'0');
            uart_putchar(' ');
            uart_putchar(eeprom_at24c04_random_read((i * 10) + 0x05)); // HH:MM:SS
            uart_putchar(eeprom_at24c04_random_read((i * 10) + 0x06)); // HH:MM:SS
            uart_putchar(':'); 
            uart_putchar(eeprom_at24c04_random_read((i * 10) + 0x07)); // HH:MM:SS
            uart_putchar(eeprom_at24c04_random_read((i * 10) + 0x08)); // HH:MM:SS
            uart_putchar(':'); 
            uart_putchar(eeprom_at24c04_random_read((i * 10) + 0x09)); // HH:MM:SS
            uart_putchar(eeprom_at24c04_random_read((i * 10) + 0x0A)); // HH:MM:SS
            uart_puts("    ");
            uart_putchar(eeprom_at24c04_random_read((i * 10) + 0x0B));
            uart_putchar(eeprom_at24c04_random_read((i * 10) + 0x0C));
            uart_puts("  ");
            uart_putchar(eeprom_at24c04_random_read((i * 10) + 0x0D));
            uart_putchar(eeprom_at24c04_random_read((i * 10) + 0x0E));
            uart_putchar('\n');
        }
    }
    else
    {
        uart_puts("No logs available. Please Create few events\n");
    }
    __delay_ms(500);
    return MENU;
}

char clear_log(void)
{
    event_count = -1;
    clcd_clear();
    clcd_print("LOGS CLEARED",LINE1(0));
    __delay_ms(2000);
    clcd_clear();
    return MENU;
}

char set_time(void)
{
    unsigned char hour,minute,second;
    
    char cursor_pos = 4;
    char time_index = 0;
    clcd_clear();
    
    get_time();
    
    clcd_print("HH:MM:SS",LINE1(4));
        
    clcd_putch(time[0], LINE2(4)); // HH:MM:SS
    clcd_putch(time[1], LINE2(5)); // HH:MM:SS
    clcd_putch(':', LINE2(6)); // HH:MM:SS
    clcd_putch(time[2], LINE2(7)); // HH:MM:SS
    clcd_putch(time[3], LINE2(8)); // HH:MM:SS
    clcd_putch(':', LINE2(9)); // HH:MM:SS
    clcd_putch(time[4], LINE2(10)); // HH:MM:SS
    clcd_putch(time[5], LINE2(11)); // HH:MM:SS
    
    clcd_write(0xC4, INST_MODE);
    clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
    __delay_us(100);
        
    while(1)
    {
        key2 = read_digital_keypad(LEVEL);
        key = read_digital_keypad(STATE);
        __delay_us(700);
        
        seconds = 0;
            
        while(key2 == SW4)
        {
            key2 = read_digital_keypad(LEVEL);
            if(seconds > 2)
            {
                hour = (((time[0] - '0') & 0x03) << 4) | (((time[1] - '0') & 0x0F));
                minute = (((time[2] - '0') & 0x07) << 4) | (((time[3] - '0') & 0x0F));
                second = (((time[4] - '0') & 0x07) << 4) | (((time[5] - '0') & 0x0F));
                
                write_ds1307(HOUR_ADDR,hour);  
                write_ds1307(MIN_ADDR, minute); 
                write_ds1307(SEC_ADDR, second);
                
                clcd_clear();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                return MENU;
            }
        }
            
        if(key == SW4)
        {
            if(time_index == 0)
            {
                if(time[0] < '2')
                {
                    clcd_putch(++time[0], LINE2(4)); // HH:MM:SS                
                }
                else
                {
                    time[0] = '0';
                    clcd_putch('0', LINE2(4)); // HH:MM:SS
                }
            }
            else if(time_index == 1)
            {
                if(((time[0] == '1' || time[0] == '0') && time[1] < '9') || (time[0] == '2' && time[1] < '4'))
                {
                    clcd_putch(++time[1], LINE2(5)); // HH:MM:SS                
                }
                else if(((time[0] == '1' || time[0] == '0') && time[1] == '9') || (time[0] == '2' && time[1] >= '4'))
                {
                    time[1] = '0';
                    clcd_putch('0', LINE2(5)); // HH:MM:SS
                }
            }
            else if(time_index == 2 || time_index == 4)
            {
                if(time[2] < '5' && cursor_pos == 7)
                {
                    clcd_putch(++time[2], LINE2(7)); // HH:MM:SS                
                }
                else if(time[2] == '5' && cursor_pos == 7)
                {
                    time[2] = '0';
                    clcd_putch('0', LINE2(7)); // HH:MM:SS
                }
                else if(time[4] < '5' && cursor_pos == 10)
                {
                    clcd_putch(++time[4], LINE2(10)); // HH:MM:SS                
                }
                else if(time[4] == '5' && cursor_pos == 10)
                {
                    time[4] = '0';
                    clcd_putch('0', LINE2(10)); // HH:MM:SS
                }
            }
            else if(time_index == 3 || time_index == 5)
            {
                if(time[3] < '9' && cursor_pos == 8)
                {
                    clcd_putch(++time[3], LINE2(8)); // HH:MM:SS                
                }
                else if(time[3] == '9' && cursor_pos == 8)
                {
                    time[3] = '0';
                    clcd_putch('0', LINE2(8)); // HH:MM:SS
                }
                else if(time[5] < '9' && cursor_pos == 11)
                {
                    clcd_putch(++time[5], LINE2(11)); // HH:MM:SS                
                }
                else if(time[5] == '9' && cursor_pos == 11)
                {
                    time[5] = '0';
                    clcd_putch('0', LINE2(11)); // HH:MM:SS
                }
            }
            clcd_write(0xC0 + cursor_pos, INST_MODE);
            
        }
        else if(key == SW5)
        {
            cursor_pos++;
            time_index++;
            if(cursor_pos == 6 || cursor_pos == 9)
            {
                cursor_pos++;
            }
            else if(cursor_pos == 12)
            {
                cursor_pos = 4;
            }
            if(time_index == 6)
            {
                time_index = 0;
            }
            clcd_write(0xC0 + cursor_pos, INST_MODE);
        }
        
    }
    
}

char change_password(void)
{
    
    unsigned char new_password[5], c_p = 6, ind = 0;
    clcd_clear();
    clcd_print(" NEW PASSWORD", LINE1(0));
    clcd_write(0xC6, INST_MODE);
    clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
    __delay_us(100);
    
    while(1)
    {
        for(int i = 0; i++ <=300;);
        key = read_digital_keypad(STATE);
        
        
        if(key == SW4)
        {
            clcd_putch('*', LINE2(c_p++));
            new_password[ind++] = '1';
        }
        else if(key == SW5)
        {
            clcd_putch('*', LINE2(c_p++));
            new_password[ind++] = '0';
        }
        if(ind == 4)
        {
            new_password[ind] = '\0';
            eeprom_at24c04_str_write(0x00, new_password);
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            clcd_clear();
            clcd_print("PASSWORD DONE", LINE1(0));
            __delay_ms(1000);
            clcd_clear();
            return DASH_BOARD;
        }
    }
}

void display_menu(void)
{
    signed char menu_index = 0;
    char ret = 0;
    char (*fptr[])(void) = {view_log,download_log,clear_log,set_time,change_password};
    
    while(1)
    {
        
        key2 = read_digital_keypad(LEVEL);
        key = read_digital_keypad(STATE);

        seconds = 0;
        while(key2 == SW4)
        {
            key2 = read_digital_keypad(LEVEL);
            
            if(seconds > 2)
            {
                ret = fptr[menu_index]();
                seconds = 0;
                
                if(ret == DASH_BOARD)
                {
                    key2 = ALL_RELEASED;
                    __delay_ms(500);
                    return;
                }
                if(ret == MENU)
                {
                    __delay_ms(500);
                    key = ALL_RELEASED;
                }
            }
        }
        while(key2 == SW5)
        {
            key2 = read_digital_keypad(LEVEL);
            
            if(seconds > 2)
            {
                clcd_clear();
                return;
            }
        }
        
        if(menu_index != 4)
        {
            
            clcd_putch('*', LINE1(0));
            clcd_print(menu[menu_index], LINE1(2));
            clcd_print(menu[menu_index+1], LINE2(2));
        }
        else if(menu_index == 4)
        {
            clcd_putch(' ', LINE1(0));
            clcd_print("SET TIME", LINE1(2));
            clcd_print("CHANGE PSWRD", LINE2(2));
            clcd_putch('*', LINE2(0));
        }

        if(key == SW4)
        {
            if(++menu_index < 4)
            {
                clcd_clear();
                seconds = 0;                
            }
            else
            {
                menu_index = 4;
            }
        }
        else if(key == SW5)
        {
            if(--menu_index > 0)
            {
                clcd_clear();
                seconds = 0;                
            }
            else
            {
                clcd_clear();
                menu_index = 0;
            }            
        }    
    }
}
void display_login(void)
{
    unsigned char password_enter[5], cursor_position = 6;
    signed char index = 0;
    
    for(int i = 0; i < 4; i++)
    {
        password[i] = eeprom_at24c04_random_read(0x00+i);
    }
    password[4] = '\0';
        
    seconds = 0;
    
    while(seconds <= 4)
    {
        for(int i = 0; i++ <=300;);
        key = read_digital_keypad(STATE);
        
        
        if(key == SW4)
        {
            clcd_putch('*', LINE2(cursor_position++));
            password_enter[index++] = '1';
            seconds = 0;
        }
        else if(key == SW5)
        {
            clcd_putch('*', LINE2(cursor_position++));
            password_enter[index++] = '0';
            seconds = 0;
        }
        if(index == 4)
        {
            password_enter[index] = '\0';

            if(strcmp(password_enter,password) == 0)
            {
                clcd_clear();                
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                display_menu();
                key2 = ALL_RELEASED;
                return;
            }
            else
            {
                clcd_clear();
                
                if(attempt == 0)
                {
                    clcd_clear();                
                    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                    __delay_us(100);
                    seconds = 0;
                    while(seconds < 10)
                    {
                        clcd_print("LOCKED FOR:", LINE1(0));
                        clcd_putch('9' - seconds, LINE1(11));
                        clcd_print("sec", LINE1(13));
                    }
                    attempt = 3;
                    clcd_clear();
                    continue;
                }
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                attempt--;
                clcd_print(" WRONG PASSWORD",LINE1(0));
                clcd_putch(attempt + '0', LINE2(0));
                clcd_print(" ATTEMPTS LEFT",LINE2(1));
                seconds = 0;
                while(seconds <= 2);
                seconds = 0;
                index = 0;
                cursor_position = 6;
                clcd_clear();
                clcd_print(" ENTER PASSWORD", LINE1(0));
                clcd_write(CURSOR_POS, INST_MODE);
                clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
                __delay_us(100);
            }
        }
    }
    clcd_clear();
    index = 0;
    cursor_position = 6;
    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
    __delay_us(100);
}
