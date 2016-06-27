/*************************************************** 
*  An Example of the little Useless box
***************************************************/

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

long delayHalfSeconds;
bool watchdogActivated = true;


// Define watchdog timer interrupt.
ISR(WDT_vect)
{
  // Set the watchdog activated flag.
  // Note that you shouldn't do much work inside an interrupt handler.
  watchdogActivated = true;
}

// Put the Arduino to sleep.
void sleep()
{
  // Set sleep to full power down.  Only external interrupts or 
  // the watchdog timer can wake the CPU!
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Turn off the peripherals while asleep.
  power_adc_disable();
  power_spi_disable();

  // Enable sleep and enter sleep mode.
  sleep_mode();

  // CPU is now asleep and program execution completely halts!
  // Once awake, execution will resume at this point.
  
  // When awake, disable sleep mode and turn on all devices.
  sleep_disable();
  power_all_enable();
}

void setup() {
  //Reset the watchdog flag
  watchdogActivated = false;

  //Set the output pinmode
  pinMode(12, OUTPUT);
  digitalWrite(12, 0);
  
  // This next section of code is timing critical, so interrupts are disabled.
  // See more details of how to change the watchdog in the ATmega328P datasheet
  // around page 50, Watchdog Timer.
  noInterrupts();
  
  // Set the watchdog reset bit in the MCU status register to 0.
  MCUSR &= ~(1<<WDRF);
  
  // Set WDCE and WDE bits in the watchdog control register.
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  // Set watchdog clock prescaler bits to a value of 0.5 seconds.
  WDTCSR = (1<<WDP0) | (1<<WDP2);
  
  // Enable watchdog as interrupt only (no reset).
  WDTCSR |= (1<<WDIE);
  
  // Enable interrupts again.
  interrupts();

  //Setup the Delay between power on and Energizing the coil of the switch
  randomSeed(analogRead(0));
  delayHalfSeconds = random(2, 20);       //Min 1 sec Max 10 sec
}

void loop() {
  if(!watchdogActivated){
    for(int i = 0; i < delayHalfSeconds; i++){
       sleep();    // Go to sleep!
    }
  }
  digitalWrite(12, 1);
  digitalWrite(13, 1); // For Debugging
  delay(300);
  digitalWrite(12, 0);
  digitalWrite(13, 0); //For Debugging
  delay(300);
}
