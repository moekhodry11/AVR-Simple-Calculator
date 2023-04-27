/****************************************************************/
/***************  Name   : Mohamed Khodary **********************/
/***************  Date   : 27/04/2023      **********************/
/***************  SWC    : Simple Calc     **********************/
/*************** Version : 1.1             **********************/
/****************************************************************/

/* Lib layer */
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "util/delay.h"
#define F_CPU 8000000UL

#include <stdlib.h>
#include <stdio.h>

/* MCAL */
#include "DIO_interface.h"

/* HAL */
#include "LCD_interface.h"
#include "KPD_interface.h"

// Declare variables for storing input and calculations
u8 buffer[16] = { 0 };
u8 flagClear = 0;	//to clear LCD after every process
u8 buffer_pos = 0;
s16 num1 = 0, num2 = 0, result = 0;
u8 op = '\0';

void numberToString(u16 num, u8 *str);
u16 stringToNumber(u8 *str);

int main(void) {
	DIO_voidInit();
	LCD_voidInit();
	u8 key;
	while (1) {

		KPD_u8GetKeyState(&key);
		if (key != KPD_u8_KEY_NOT_PRESSED
				&& (key == '1' || key == '2' || key == '3' || key == '4'
						|| key == '5' || key == '6' || key == '7' || key == '8'
						|| key == '9' || key == '0')) {
			if (flagClear == 1) {
				LCD_voidSendCmnd(LCD_CLEAR);
				flagClear = 0;
			}
			LCD_voidSendCmnd(LCD_CLEAR);
			buffer[buffer_pos++] = key;
			LCD_voidSendChar(key);
		} else if (key == '+' || key == '-' || key == '*' || key == '/') {
			// Store first number and operator
			LCD_voidSendCmnd(LCD_CLEAR);
			LCD_voidSendChar(key);
			num1 = stringToNumber(buffer);
			memset(buffer, 0, sizeof(buffer));	//clear buffer
			op = key;
			buffer_pos = 0;
		}

		else if (key == '=') {
			//LCD_voidSendCmnd(LCD_CLEAR);//clear LCD to print output
			flagClear = 1;	//flag is up after evry process
			// Store the second number and perform the calculation
			num2 = stringToNumber(buffer);
			memset(buffer, 0, sizeof(buffer));
			switch (op) {
			case '+':
				result = num1 + num2;
				break;
			case '-':
				result = num1 - num2;
				break;
			case '*':
				result = num1 * num2;
				break;
			case '/':
				result = num1 / num2;
				break;
			}
			LCD_voidSendCmnd(LCD_CLEAR);
			//LCD_voidSendChar('=');
			if (result < 0) {
				result *= -1;
				LCD_voidSendChar('-');
			}
			// Display result on LCD
			u8 result_str[16];
			numberToString(result, result_str);
			LCD_voidSendString(result_str);
			buffer_pos = 0;
			num1 = 0, num2 = 0, result = 0;
		}
		// Handle C input
		else if (key == 'C') {
			memset(buffer, 0, sizeof(buffer));
			buffer_pos = 0;
			LCD_voidSendCmnd(LCD_CLEAR);
			;
		}

	};

	return 0;
}

u16 stringToNumber(u8 *str) {
	u16 result = 0;
	u16 sign = 1;
	if (*str == '-') {
		sign = -1;
		str++;
	}
	while (*str != '\0') {
		if (*str < '0' || *str > '9') {
			// invalid character
			return 0;
		}
		result = result * 10 + (*str - '0');
		str++;
	}
	return sign * result;
}

void numberToString(u16 num, u8 *str) {
	u16 i = 0;
	if (num < 0) {
		*str++ = '-';
		num *= -1;
	}
	do {
		str[i++] = (num % 10) + '0';
		num /= 10;
	} while (num);
	str[i] = '\0';
	// Reverse the string
	for (u16 j = 0; j < i / 2; j++) {
		u8 temp = str[j];
		str[j] = str[i - j - 1];
		str[i - j - 1] = temp;
	}
}
