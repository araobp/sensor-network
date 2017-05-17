/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using MPLAB(c) Code Configurator

  @Description:
    This header file provides implementations for pin APIs for all pins selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 4.15.1
        Device            :  PIC16F1829
        Version           :  1.01
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

    Microchip licenses to you the right to use, modify, copy and distribute
    Software only when embedded on a Microchip microcontroller or digital signal
    controller that is integrated into your product or third party product
    (pursuant to the sublicense terms in the accompanying license agreement).

    You should refer to the license agreement accompanying this Software for
    additional information regarding your rights and obligations.

    SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
    EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
    MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
    IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
    CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
    OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
    INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
    CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
    SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
    (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

*/


#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set channel_AN3 aliases
#define channel_AN3_TRIS               TRISAbits.TRISA4
#define channel_AN3_LAT                LATAbits.LATA4
#define channel_AN3_PORT               PORTAbits.RA4
#define channel_AN3_WPU                WPUAbits.WPUA4
#define channel_AN3_ANS                ANSELAbits.ANSA4
#define channel_AN3_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define channel_AN3_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define channel_AN3_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define channel_AN3_GetValue()           PORTAbits.RA4
#define channel_AN3_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define channel_AN3_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define channel_AN3_SetPullup()      do { WPUAbits.WPUA4 = 1; } while(0)
#define channel_AN3_ResetPullup()    do { WPUAbits.WPUA4 = 0; } while(0)
#define channel_AN3_SetAnalogMode()  do { ANSELAbits.ANSA4 = 1; } while(0)
#define channel_AN3_SetDigitalMode() do { ANSELAbits.ANSA4 = 0; } while(0)

// get/set RB4 procedures
#define RB4_SetHigh()    do { LATBbits.LATB4 = 1; } while(0)
#define RB4_SetLow()   do { LATBbits.LATB4 = 0; } while(0)
#define RB4_Toggle()   do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define RB4_GetValue()         PORTBbits.RB4
#define RB4_SetDigitalInput()   do { TRISBbits.TRISB4 = 1; } while(0)
#define RB4_SetDigitalOutput()  do { TRISBbits.TRISB4 = 0; } while(0)
#define RB4_SetPullup()     do { WPUBbits.WPUB4 = 1; } while(0)
#define RB4_ResetPullup()   do { WPUBbits.WPUB4 = 0; } while(0)
#define RB4_SetAnalogMode() do { ANSELBbits.ANSB4 = 1; } while(0)
#define RB4_SetDigitalMode()do { ANSELBbits.ANSB4 = 0; } while(0)

// get/set RB6 procedures
#define RB6_SetHigh()    do { LATBbits.LATB6 = 1; } while(0)
#define RB6_SetLow()   do { LATBbits.LATB6 = 0; } while(0)
#define RB6_Toggle()   do { LATBbits.LATB6 = ~LATBbits.LATB6; } while(0)
#define RB6_GetValue()         PORTBbits.RB6
#define RB6_SetDigitalInput()   do { TRISBbits.TRISB6 = 1; } while(0)
#define RB6_SetDigitalOutput()  do { TRISBbits.TRISB6 = 0; } while(0)
#define RB6_SetPullup()     do { WPUBbits.WPUB6 = 1; } while(0)
#define RB6_ResetPullup()   do { WPUBbits.WPUB6 = 0; } while(0)

// get/set channel_AN7 aliases
#define channel_AN7_TRIS               TRISCbits.TRISC3
#define channel_AN7_LAT                LATCbits.LATC3
#define channel_AN7_PORT               PORTCbits.RC3
#define channel_AN7_WPU                WPUCbits.WPUC3
#define channel_AN7_ANS                ANSELCbits.ANSC3
#define channel_AN7_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define channel_AN7_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define channel_AN7_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define channel_AN7_GetValue()           PORTCbits.RC3
#define channel_AN7_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define channel_AN7_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define channel_AN7_SetPullup()      do { WPUCbits.WPUC3 = 1; } while(0)
#define channel_AN7_ResetPullup()    do { WPUCbits.WPUC3 = 0; } while(0)
#define channel_AN7_SetAnalogMode()  do { ANSELCbits.ANSC3 = 1; } while(0)
#define channel_AN7_SetDigitalMode() do { ANSELCbits.ANSC3 = 0; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()    do { LATCbits.LATC4 = 1; } while(0)
#define RC4_SetLow()   do { LATCbits.LATC4 = 0; } while(0)
#define RC4_Toggle()   do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()         PORTCbits.RC4
#define RC4_SetDigitalInput()   do { TRISCbits.TRISC4 = 1; } while(0)
#define RC4_SetDigitalOutput()  do { TRISCbits.TRISC4 = 0; } while(0)
#define RC4_SetPullup()     do { WPUCbits.WPUC4 = 1; } while(0)
#define RC4_ResetPullup()   do { WPUCbits.WPUC4 = 0; } while(0)

// get/set RC5 procedures
#define RC5_SetHigh()    do { LATCbits.LATC5 = 1; } while(0)
#define RC5_SetLow()   do { LATCbits.LATC5 = 0; } while(0)
#define RC5_Toggle()   do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define RC5_GetValue()         PORTCbits.RC5
#define RC5_SetDigitalInput()   do { TRISCbits.TRISC5 = 1; } while(0)
#define RC5_SetDigitalOutput()  do { TRISCbits.TRISC5 = 0; } while(0)
#define RC5_SetPullup()     do { WPUCbits.WPUC5 = 1; } while(0)
#define RC5_ResetPullup()   do { WPUCbits.WPUC5 = 0; } while(0)

// get/set channel_AN8 aliases
#define channel_AN8_TRIS               TRISCbits.TRISC6
#define channel_AN8_LAT                LATCbits.LATC6
#define channel_AN8_PORT               PORTCbits.RC6
#define channel_AN8_WPU                WPUCbits.WPUC6
#define channel_AN8_ANS                ANSELCbits.ANSC6
#define channel_AN8_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define channel_AN8_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define channel_AN8_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define channel_AN8_GetValue()           PORTCbits.RC6
#define channel_AN8_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define channel_AN8_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define channel_AN8_SetPullup()      do { WPUCbits.WPUC6 = 1; } while(0)
#define channel_AN8_ResetPullup()    do { WPUCbits.WPUC6 = 0; } while(0)
#define channel_AN8_SetAnalogMode()  do { ANSELCbits.ANSC6 = 1; } while(0)
#define channel_AN8_SetDigitalMode() do { ANSELCbits.ANSC6 = 0; } while(0)

// get/set IO_RC7 aliases
#define IO_RC7_TRIS               TRISCbits.TRISC7
#define IO_RC7_LAT                LATCbits.LATC7
#define IO_RC7_PORT               PORTCbits.RC7
#define IO_RC7_WPU                WPUCbits.WPUC7
#define IO_RC7_ANS                ANSELCbits.ANSC7
#define IO_RC7_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define IO_RC7_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define IO_RC7_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define IO_RC7_GetValue()           PORTCbits.RC7
#define IO_RC7_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define IO_RC7_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define IO_RC7_SetPullup()      do { WPUCbits.WPUC7 = 1; } while(0)
#define IO_RC7_ResetPullup()    do { WPUCbits.WPUC7 = 0; } while(0)
#define IO_RC7_SetAnalogMode()  do { ANSELCbits.ANSC7 = 1; } while(0)
#define IO_RC7_SetDigitalMode() do { ANSELCbits.ANSC7 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/