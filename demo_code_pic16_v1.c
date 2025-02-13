/*
 * File:   newmain.c
 * Author: jonat
 *
 * Created on February 10, 2025, 9:46 PM
 */

// CONFIG1
#pragma config FEXTOSC = HS     // External Oscillator mode selection bits (HS (crystal oscillator) above 4MHz; PFM set to high power)
#pragma config RSTOSC = EXT1X   // Power-up default value for COSC bits (EXTOSC operating per FEXTOSC bits)
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; i/o or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (FSCM timer enabled)

// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR pin is Master Clear function)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config LPBOREN = OFF    // Low-Power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = ON       // Brown-out reset enable bits (Brown-out Reset Enabled, SBOREN bit is ignored)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices)
#pragma config ZCD = OFF        // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR.)
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit can be cleared and set only once in software)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

// CONFIG3
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = ON        // WDT operating mode (WDT enabled regardless of sleep; SWDTEN ignored)
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4
#pragma config WRT = OFF        // UserNVM self-write protection bits (Write protection off)
#pragma config SCANE = available// Scanner Enable bit (Scanner module is available for use)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/Vpp pin function is MCLR.)

// CONFIG5
#pragma config CP = OFF         // UserNVM Program memory code protection bit (Program Memory code protection disabled)
#pragma config CPD = OFF        // DataNVM code protection bit (Data EEPROM code protection disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>         // for sprintf
#include <stdint.h>        // include stdint header


#define _XTAL_FREQ  4000000
#define LED RD0
#define BR  1200
#define Pulse_pin   RB4
void init_UART(void){
    
    TRISC6=0;//TX
    TRISC7=1;//RX
    RC6PPS=0x10;
    SPBRG=((_XTAL_FREQ/16)/BR)-1;
    BRGH=1;
    TX1STAbits.SYNC = 0;
    
    SPEN=1;
    TXEN=1;

    CREN=1;
    
    TX9=0;
    RX9=0;
    
    
    
}

void send_data(char to_send){
    while(!TXIF);
    TXREG=to_send;

}

void Send_String (char* st_pt){

    while(*st_pt)
        send_data(*st_pt++);
}

void Timer_config(void){
    
    T0CON0bits.T0EN = 0;        // Disable Timer0 for configuration
    T0CON0bits.T016BIT = 1;     // Set Timer0 to 16-bit mode
    T0CON0bits.T0OUTPS = 0b000; // No postscaler (1:1)

    T0CON1bits.T0CS = 0b010;    // Clock source = Fosc/4 (1 MHz)
    T0CON1bits.T0ASYNC = 1;     // Enable asynchronous mode
    T0CON1bits.T0CKPS = 0b0000; // Prescaler = 1:1 (1 MHz clock ? 1 tick = 1 µs)
    PIR0bits.TMR0IF=0;


}

char *itoa(int value) 
 {
     static char buffer[12];        // using 12 bytes to hold int32 dtype
     int original = value;        // save original value
 
     int c = sizeof(buffer)-1;
 
     buffer[c] = 0;                // write null char at the end   
 
     if (value < 0)                 // if it's negative, note that and take the absolute value
         value = -value;
     
     do                             // write least significant digit of value that's left
     {
         buffer[--c] = (value % 10) + '0';    
         value /= 10;
     } while (value);
 
     if (original < 0) 
         buffer[--c] = '-';
 
     return &buffer[c];
 }

void main(void) {
init_UART();
Timer_config();
    TRISD0=0;
    TRISC1=0;
    TRISB5=1;
    TRISB4=0;

    ANSELB=0;
    INLVLB5=0;
    
    int distance=0;
    int a=0;

    LED=1;
    while(1){
        T0CON0bits.T0EN=0;

        TMR0H=0;
        TMR0L=0;
        T0CON1bits.T0CKPS=0b0000;
        //T0CON0bits.T0EN=1;

        LED=1;


       // LED=1;
       // __delay_ms(1000);
       // LED=0;
        //__delay_ms(1000);
        Pulse_pin=1;
        __delay_us(10);
        Pulse_pin=0;
        while(!PORTBbits.RB5);
        T0CON0bits.T0EN=1;
        while(PORTBbits.RB5);
        T0CON0bits.T0EN=0;
        LED=1;

        distance=(TMR0L | TMR0H << 8);
        distance=distance/59;

        Send_String("aa");
        Send_String(itoa(distance));
        Send_String("xx\r\n");

        __delay_ms(1000);

    }
    
    
}
