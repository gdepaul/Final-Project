// ******************************************************************************************* //
// Greg DePaul
// Chris Carry
// Vincent Ippolito
// ECE 372
// Lab3

// ******************************************************************************************* //
// Include file for PIC24FJ64GA002 microcontroller. This include file defines
// MACROS for special function registers (SFR) and control bits within those
// registers.

#include "p24fj64ga002.h"
#include <stdio.h>
#include "lcd.h"

// ******************************************************************************************* //
// Configuration bits for CONFIG1 settings.
//
// Make sure "Configuration Bits set in code." option is checked in MPLAB.
//
// These settings are appropriate for debugging the PIC microcontroller. If you need to
// program the PIC for standalone operation, change the COE_ON option to COE_OFF.

_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF &
          BKBUG_ON & COE_OFF & ICS_PGx1 &
          FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS32768 )

// ******************************************************************************************* //
// Configuration bits for CONFIG2 settings.
// Make sure "Configuration Bits set in code." option is checked in MPLAB.

_CONFIG2( IESO_OFF & SOSCSEL_SOSC & WUTSEL_LEG & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF &
          IOL1WAY_OFF & I2C1SEL_PRI & POSCMOD_XT )

// ******************************************************************************************* //

// Definitions
   

// ******************************************************************************************* //

// Global Variables
volatile int ADC_value;
volatile char recordColors;
volatile char colorChange;
volatile char lastColor;

// ******************************************************************************************* //

// Function Definitions
void setMotors();

// ******************************************************************************************* //

int main(void)
{
    // Initialize Variables
    char value[8];


    //Initialize the LCD
    LCDInitialize();

    // Configure AD1PCFG register for configuring input pin as analog
    // This will configue the ADC for the potentiometer
    // This will use Pin (1)
    AD1PCFG &= 0xFFFE;
    AD1CON2 = 0;
    AD1CON3 = 0x0101;
    AD1CON1 = 0x20E4;
    AD1CHS = 0;
    AD1CSSL = 0;

    IFS0bits.AD1IF = 0;         // Clear A/D conversion interrupt.
    AD1CON1bits.ADON = 1;       // Turn on A/D

    // Initialize a switch for the momentary changes
    // Turn on SW1 with PORTB
    TRISBbits.TRISB5 = 1;
    CNEN2bits.CN27IE = 1;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;

    // Now we must configure the pins to be used for the PWM
    TMR2 = 0;
    T2CONbits.TCKPS1 = 1;
    T2CONbits.TCKPS0 = 1;
    T2CONbits.TON = 1;
    PR2 = 1842;                 // 1ms

    // Determines the duty cycle of the first motor
    // 18 if OCM is on
    // 0 if off
    // OCM for 1 signal
    RPOR1bits.RP2R = 18;         // Output Compare 1  -- 18 is for OC1 output
    OC1CONbits.OCTSEL = 0;      // Using Timer 2 for OC1
    OC1CONbits.OCM = 6;         // PWM mode
    OC1R = 1842;                // 1842/2 = 921... 50% Duty cycle
    OC1RS = 921;                // Duty 100%

    // OCM for 2 signal
    RPOR1bits.RP3R = 19;        // Output Compare 2 -- 19 is for OC2 output.
    OC2CONbits.OCTSEL = 0;      // Using Timer 2 for OC2
    OC2CONbits.OCM = 6;         // PWM mode
    OC2R = 1842;                // 1842/2 = 921... 50% Duty cycle
    OC2RS = 921;                // 100% Duty

    // Determines the duty cycle of the second motor
    // 18 if OCM is on
    // 0 if off
    // OCM for 1 signal
    RPOR4bits.RP8R = 18;        // Output Compare 1  -- 18 is for OC1 output

    // OCM for 2 signal
    RPOR4bits.RP9R = 19;        // Output Compare 2 -- 19 is for OC2 output.

    //setMotors();

    while(1)
    {

        if(IFS0bits.AD1IF == 1) { // Poll for a change in the ADC value
            IFS0bits.AD1IF = 0;
            ADC_value = ADC1BUF0;

            // Print the digital value of the
            //sprintf(value, "%6d", ADC_value);
            //LCDMoveCursor(0,0);
            //LCDPrintString(value);

            
            LCDMoveCursor(0,0);
            if(ADC_value < 100) {
                LCDClear();
            }
            else if(ADC_value < 200) {
                LCDClear();
                LCDPrintString("Black");
            }
            else if(ADC_value < 400) {
                LCDClear();
                LCDPrintString("Red");
            }
            else {
                LCDClear();
                LCDPrintString("White");
            }

            //setMotors();
        }
        
        /*
        if(changeState == 1 && PORTBbits.RB5 == 1) {
            LCDClear();
            switch(state) {
                case(Idle1):
                    state = Forward;
                    RPOR1bits.RP2R = 0;
                    RPOR1bits.RP3R = 0;
                    RPOR4bits.RP8R = 0;
                    RPOR4bits.RP9R = 0;
                    changeState = 0;
                    //LCDMoveCursor(1,0);
                    //LCDPrintString("Idle");
                    break;
                case(Forward):
                    state = Idle2;
                    RPOR1bits.RP2R = 0;
                    RPOR1bits.RP3R = 19;
                    RPOR4bits.RP8R = 0;
                    RPOR4bits.RP9R = 18;
                    changeState = 0;
                    //LCDMoveCursor(1,0);
                    //LCDPrintString("Forward");
                    break;
                case(Idle2):
                    state = Backward;
                    RPOR1bits.RP2R = 0;
                    RPOR1bits.RP3R = 0;
                    RPOR4bits.RP8R = 0;
                    RPOR4bits.RP9R = 0;
                    changeState = 0;
                    //LCDMoveCursor(1,0);
                    //LCDPrintString("Idle");
                    break;
                case(Backward):
                    state = Idle1;
                    RPOR1bits.RP2R = 18;
                    RPOR1bits.RP3R = 0;
                    RPOR4bits.RP8R = 19;
                    RPOR4bits.RP9R = 0;
                    changeState = 0;
                    //LCDMoveCursor(1,0);
                    //LCDPrintString("Backward");
                    break;
            }
            setMotors();

        }
        */
    }
    
    return 0;
    
}

// ******************************************************************************************* //

// If the switch is turned on 
void _ISR _CNInterrupt(void)
{
	// TODO: Clear interrupt flag
	IFS1bits.CNIF = 0;

	// TODO: Detect if *any* key of the keypad is *pressed*, and update scanKeypad
	// variable to indicate keypad scanning process must be executed.
}

// ******************************************************************************************* //

void setMotors() {

    
}

// ******************************************************************************************* //