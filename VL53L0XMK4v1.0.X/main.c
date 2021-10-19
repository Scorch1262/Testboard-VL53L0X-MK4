#define FCY 16000000UL 
#include <assert.h>
#include <stdbool.h>
#include "mcc_generated_files/system.h"
#include <stdio.h>
#include <stdlib.h>
#include <libpic30.h>
#include <xc.h>
#include <string.h>
#include "mcc_generated_files/i2c1.h"
#include "vl53l0x.h"

#if defined(__dsPIC33E__)
	#include <p33exxxx.h>
#elif defined(__dsPIC33F__)
	#include <p33fxxxx.h>
#elif defined(__dsPIC30F__)
	#include <p30fxxxx.h>
#elif defined(__PIC24E__)
	#include <p24exxxx.h>
#elif defined(__PIC24H__)
	#include <p24hxxxx.h>
#elif defined(__PIC24F__) || defined(__PIC24FK__)
	#include <p24fxxxx.h>
#endif

float range_mm;                                     // definiert "range_mm"
float range_cm;                                     // definiert "range_cm"
    
/*
                         Main application
 */
int main(void){                                         // "main"
    SYSTEM_Initialize();                                // ruft "SYSTEM_Initialize" auf
    VL53L0X_Init();                                 // initialisierung des Sensors
    __delay_ms(500);                                // warte 500ms
    
    while (1){                                          // Endloschleife
        range_mm = VL53L0X_Millimeter_GetValue();   // schreibt ergebnis von "VL53L0X_Millimeter_GetValue" in "range_mm"
        range_cm = range_mm / 10;                   // rechnet Wert für "range_cm" aus
        printf("Distanz = %.0fmm \t %.1fcm\r\n",range_mm,range_cm); // schreibt "Distanz = %.0fmm \t %.1fcm\r\n" an UART  
        __delay_ms(500);                            // warte 500ms
    }                                                   // 
}                                                       // 
    
