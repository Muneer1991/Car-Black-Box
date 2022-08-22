/* 
 * File:   main.h
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include <string.h>
#include "clcd.h"
#include "adc.h"
#include "ds1307.h"
#include "i2c.h"
#include "car_black_box.h"
#include "digital_keypad.h"
#include "at24c04.h"
#include "timers.h"
#include "uart.h"

#define DASH_BOARD_FLAG     0x01
#define LOGIN_FLAG          0x02


#define CURSOR_POS          0xC6    //line2 6th position
#define MENU                0x0A
#define DASH_BOARD          0x0B

#endif	/* MAIN_H */

