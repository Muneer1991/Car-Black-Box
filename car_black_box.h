/* 
 * File:   car_black_box.h
 * Author: muneer
 *
 * Created on August 16, 2022, 9:29 PM
 */

#ifndef CAR_BLACK_BOX_H
#define	CAR_BLACK_BOX_H

void display_dash_board(unsigned char *event, unsigned char speed);
void log_car_event(unsigned char *event, unsigned char speed);
void clcd_clear(void);
void display_login(void);

#endif	/* CAR_BLACK_BOX_H */

