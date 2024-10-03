#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


#define BTN 1
#define BTN_STATUS !((PINB >> BTN) & 0x01)

enum Status
{
    RUNNING,
    POWER_OFF,
};


void power_off()
{
    cli();                               // Disable Interrupts before next commands
    wdt_disable();                       // Disable watch dog timer to save power
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode power down
    sleep_enable();
    sleep_bod_disable(); // Disable Brown out detector
    sei();               // Enable interrupts
    sleep_cpu();
    sleep_disable();
}

Status status;  // Device status


void setup() {
  PORTB |= (1 << BTN);  // Enable PULL_UP resistor
  GIMSK |= (1 << PCIE); // Enable Pin Change Interrupts
  PCMSK |= (1 << BTN);  // Use PCINTn as interrupt pin
  sei();                // Enable interrupts

  status = RUNNING; // Start ON/OFF when power is connected

  DDRB |= (1 << DDB0); //Set pin 0 to output for the LED
}

void loop() {

    if (status == RUNNING)
    {
        /* main code here */

        PORTB |= (1 << PB0); // LED Pin 0 ON

        /* -------------- */
    }
    else
    {
        PORTB &= ~(1 << PB0); // LED Pin 0 OFF

        power_off();
    }
}



ISR(PCINT0_vect)
{
    if (BTN_STATUS) //If button is down change status
    {
        if (status == RUNNING)
            status = POWER_OFF;
        else
        {
            status = RUNNING;
            /* initialize the device here (timers etc..)*/
        }
    }
}
