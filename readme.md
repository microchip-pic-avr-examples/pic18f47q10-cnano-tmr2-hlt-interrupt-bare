<div id="readme" class="Box-body readme blob js-code-block-container">
 <article class="markdown-body entry-content p-3 p-md-6" itemprop="text"><p><a href="https://www.microchip.com" rel="nofollow"><img src="images/MicrochipLogo.png" alt="MCHP" style="max-width:100%;"></a></p>

# PIC18F47Q10 Configure TMR4 as HLT to generate an interrupt (like a WDT without reset)

## Objective:
The PIC18F47Q10 features timers with Hardware Limit Timer (HLT) and one 10-bit ADCC module.
In this demo, uses TMR4 peripheral as a Hardware Limit Timer in order to generate an interrupt and stop TMR2 that also stops the ADCC auto-conversion.

## Resources:
- Technical Brief Link [(linkTBD)](http://www.microchip.com/)
- MPLAB® X IDE 5.30 or newer [(microchip.com/mplab/mplab-x-ide)](http://www.microchip.com/mplab/mplab-x-ide)
- MPLAB® XC8 2.10 or newer compiler [(microchip.com/mplab/compilers)](http://www.microchip.com/mplab/compilers)
- PIC18F47Q10 Curiosity Nano [(DM182029)](https://www.microchip.com/Developmenttools/ProductDetails/DM182029)
- Curiosity Nano Base for Click Boards™ [(AC164162)](https://www.microchip.com/Developmenttools/ProductDetails/AC164162)
- POT click board™ [(MIKROE-3402)](https://www.mikroe.com/pot-click)
- [PIC18F47Q10 datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/40002043D.pdf) for more information or specifications.

## Hardware Configuration:

The PIC18F47Q10 Curiosity Nano Development Board [(DM182029)](https://www.microchip.com/Developmenttools/ProductDetails/DM182029) is used as the test platform along with the Curiosity Nano Base for Click Boards™ [(AC164162)](https://www.microchip.com/Developmenttools/ProductDetails/AC164162) and the POT click board™ [(MIKROE-3402)](https://www.mikroe.com/pot-click).

The following configurations must be made for this project:
- RA0 pin - Configured as analog input 
- RE0 (LED0) pin - Configured as digital output
- RC7 pin - Configured as digital input

## Demo:
Run the code, LED0 will blink with `Timer2Period` (100ms), if potentiometer value is below a `DesiredThreshold` and will be light ON constantly if above that value. Also if the ADCC read value is above `MaxThreshold` and RC7 pin is pulled to GND for more than `Timer4Period` (500ms), TMR4 will stop TMR2 and LED0 will blink with 500ms period as long as RC7 is tied to GND.

<img src="images/HWsetup-HLT.gif" alt="Hardware Setup"/>

