/**
    (c) 2020 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#pragma config WDTE = OFF               /* WDT operating mode->WDT Disabled */ 
#pragma config LVP = ON                 /* Low voltage programming enabled, RE3 pin is MCLR */ 

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define Timer2Period        0x2F        /* TMR2 Period is 100ms */
#define Timer4Period        0xF1        /* TMR4 Period is 500ms */
#define DesiredThreshold    300         /* Desired threshold value */
#define MaxThreshold        500         /* Maximum threshold value */
#define AnalogChannel       0x00        /* Use ANA0 as input for ADCC */

volatile uint16_t adcVal;

static void CLK_Initialize(void);
static void PPS_Initialize(void);
static void PORT_Initialize(void);
static void ADCC_Initialize(void);
static void TMR2_Initialize(void);
static void TMR4_Initialize(void);
static void INTERRUPT_Initialize(void);
static uint16_t ADC_ReadValue(uint8_t);
static void ADCC_Interrupt(void);
static void TMR4_Interrupt(void);

static void CLK_Initialize(void)
{
    /* set HFINTOSC Oscillator */  
    OSCCON1 = 0x60;
    /* set HFFRQ to 1 MHz */
    OSCFRQ = 0x00;
}

static void PPS_Initialize(void)
{
    /* Set RC7 as input for TMR4 (T4IN) */
    T4INPPS = 0x17;
}

static void PORT_Initialize(void)
{
    /* Set RC7 pin as digital */
    ANSELC = 0x7F;
    /* Set RE0 pin as output */
    TRISE = 0x06;
    /* Enable weak pull-up on pin RC7 */
    WPUC = 0x80;
}

static void ADCC_Initialize(void)
{
    /* ADACT Auto-Conversion Trigger Source is TMR2 */ 
    ADACT = 0x04;
    /* ADGO stop; ADFM right; ADON enabled; ADCONT disabled; ADCS FRC */
    ADCON0 = 0x94;
    /* Clear the ADCC interrupt flag */
    PIR1bits.ADIF = 0;
    /* Enabling ADCC interrupt flag */
    PIE1bits.ADIE = 1;
}

static void TMR2_Initialize(void)
{
    /* TMR2 Clock source, LFINTOSC (00100) has 31 kHz */
    T2CLKCON = 0x04;
    /* T2PSYNC Not Synchronized; T2MODE Starts at T2ON = 1 and TMR2_ers = 0; T2CKPOL Rising Edge */
    T2HLT = 0x02; 
    /* TMR2 external reset is TMR4_postscaled */ 
    T2RST = 0x02;
    /* TMR2 ON on; T2 CKPS Prescaler 1:64; T2 OUTPS Postscaler 1:1 
       Minimum timer period is 31 kHz/64 = 2.064516 ms  */
    T2CON = 0xE0;
    /* Set TMR2 period, PR2 to 100ms */
    T2PR = Timer2Period;
    /* Clear the TMR2 interrupt flag */
    PIR4bits.TMR2IF = 0;
}

static void TMR4_Initialize(void)
{
    /* TMR4 Clock source, LFINTOSC (00100) has 31 kHz */
    T4CLKCON = 0x04;
    /* T4PSYNC Synchronized; T4MODE Resets at TMR4_ers = 1; T4CKPOL Rising Edge */
    T4HLT = 0x87;
    /* TMR4 External reset signal by T4INPPS pin */
    T4RST = 0;
    /* TMR4 ON on; T4 CKPS Prescaler 1:64; T4 OUTPS Postscaler 1:1 
       Minimum timer period is 31 kHz/64 = 2.064516 ms  */ 
    T4CON = 0xE0;
    /* Set TMR4 period, PR4 to 500ms */
    T4PR = Timer4Period;
    /* Clear the TMR4 interrupt flag */
    PIR4bits.TMR4IF = 0;
    /* Enabling TMR4 interrupt flag */
    PIE4bits.TMR4IE = 1;
}

static void INTERRUPT_Initialize(void)
{
    INTCONbits.GIE  = 1;          /* Enable Global Interrupts */
    INTCONbits.PEIE = 1;          /* Enable Peripheral Interrupts */ 
}

static uint16_t ADCC_ReadValue(uint8_t channel)
{   
    ADPCH = channel;     /* Set the input channel for ADCC */
    /* TMR2 is trigger source for auto-conversion for ADCC */
    return ((uint16_t)((ADRESH << 8) + ADRESL));
}

static void __interrupt() INTERRUPT_manager (void)
{
    /* Interrupt handler */
    if (INTCONbits.PEIE == 1)
    {
        if (PIE4bits.TMR4IE == 1 && PIR4bits.TMR4IF == 1)
        {
            TMR4_Interrupt();
        } 
        else if (PIE1bits.ADIE == 1 && PIR1bits.ADIF == 1)
        {
            ADCC_Interrupt();
        } 
    }
}

static void ADCC_Interrupt(void)
{
    /* Clear the ADCC interrupt flag */
    PIR1bits.ADIF = 0;
    
    if (adcVal < DesiredThreshold)
    {
        /* Toggle LED0 at the Timer2Period frequency */
         LATEbits.LATE0 = ~LATEbits.LATE0;
    }
    /* Get the conversion result from ADCC AnalogChannel */
    adcVal = ADCC_ReadValue(AnalogChannel);
}

static void TMR4_Interrupt(void)
{
    /* Clear the TMR4 interrupt flag */
    PIR4bits.TMR4IF = 0;

    /* HLT trigger condition: if adcVal > MaxThreshold and pin RC7 is pulled-down */
    if (adcVal > MaxThreshold)
    {
        /* Toggle LED0 at the Timer4Period frequency */
        LATEbits.LATE0 = ~LATEbits.LATE0;
        /* HLT will stop TMR2 that also stops ADCC */
        /* Stop the Timer by writing to TMRxON bit */
        T2CONbits.TMR2ON = 0;
    }
}

void main(void)
{
    /* Initialize the device */
    CLK_Initialize();             /* Oscillator Initialize function */
    PPS_Initialize();             /* Peripheral select Initialize function */
    PORT_Initialize();            /* Port Initialize function */
    ADCC_Initialize();            /* ADCC Initialize function */
    TMR2_Initialize();            /* TMR2 Initialize function */
    TMR4_Initialize();            /* TMR4 Initialize function */
    INTERRUPT_Initialize();       /* Interrupt Initialize function */

    while (1)
    {
        if ((adcVal > DesiredThreshold)&&(adcVal < MaxThreshold))
        {
            /* turn LED0 ON by writing pin RE0 to low */
            LATEbits.LATE0 = 0;
        }
    }
}

/**
 End of File
*/