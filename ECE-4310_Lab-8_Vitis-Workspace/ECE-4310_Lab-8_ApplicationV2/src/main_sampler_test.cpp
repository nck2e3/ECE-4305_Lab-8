/**
 * @file main_sampler_test.cpp
 *
 * @brief Basic test of Nexys4 DDR MMIO cores
 *
 * @version v1.0: Initial release
 */

#include "chu_init.h"
#include "gpio_cores.h"
#include "xadc_core.h"
#include "sseg_core.h"
#include "spi_core.h"
#include "i2c_core.h"
#include "ps2_core.h"
#include "ddfs_core.h"
#include "adsr_core.h"

#define READ_CHANNEL 1

/**
 * Converts an ADC voltage value to a displayable format on a seven-segment display.
 *
 * This function takes the ADC voltage reading and formats it for display on the
 * seven-segment display in the format "x.x".
 *
 * @param sseg Pointer to the SsegCore instance.
 * @param adc_voltage The ADC voltage value to be displayed.
 */
void adc_voltage_to_sseg(SsegCore *sseg, double adc_voltage)
{
    //Turn off unneeded SSeg displays...
    for (int i = 3; i > 0; --i)
        sseg->write_1ptn(0xff, i); //Active LOW...

    sseg->set_dp(2*2*2*2*2*2*2);  // Set decimal point between the sevenrg and sixth digits...

    //Determine value for the whole portion of adc_voltage...
    int upper_disp_val = (int)(adc_voltage / 100.0); 
    uint8_t converted_upper_disp_val = sseg->h2s(upper_disp_val);
    sseg->write_1ptn(converted_upper_disp_val, 7);

    //Determine value for the first decimal place of adc_voltage...
    int first_frac_val = ((int)(adc_voltage / 10)) % 10; //Ones place...
    uint8_t converted_first_frac_val = sseg->h2s(first_frac_val);
    sseg->write_1ptn(converted_first_frac_val, 6);

    //Determine value for the second decimal place of adc_voltage...
    int second_frac_val = ((int)adc_voltage) % 10; //Tenths place...
    uint8_t converted_second_frac_val = sseg->h2s(second_frac_val);
    sseg->write_1ptn(converted_second_frac_val, 5);

    //Determine value for the third decimal place of adc_voltage...
    int third_frac_val = (int)(adc_voltage * 10) % 10; //Hundreths place...
    uint8_t converted_third_frac_val = sseg->h2s(third_frac_val);
    sseg->write_1ptn(converted_third_frac_val, 4);  //Display position for third decimal place...
}

/**
 * Reads FPGA internal voltage and temperature values and controls the LED display.
 *
 * This function periodically reads the ADC raw values and displays them
 * on the LED, along with printing sensor and analog channel values to the console.
 *
 * @param adc_p Pointer to the XadcCore instance for ADC readings.
 * @param led_p Pointer to the GpoCore instance for controlling LEDs.
 */
void adc_check(XadcCore *adc_p, GpoCore *led_p) {
   double reading;
   int i;
   uint16_t raw;

   for (i = 0; i < 5; i++) {
      //Display 12-bit channel 0 reading on LED...
      raw = adc_p->read_raw(0);
      raw = raw >> 4;
      led_p->write(raw);

      //Display on-chip sensor and analog channel values in the console...
      uart.disp("Analog channel/voltage: ");
      uart.disp(2);
      uart.disp(" / ");
      reading = adc_p->read_adc_in(2);
      uart.disp(reading, 2);
      uart.disp("\n\r");
   }
}

//Instantiate global objects for LED, ADC, and SSEG cores...
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));

/**
 * Main program that starts the LED chase using ADC values to control speed.
 *
 * This program continuously reads analog channel voltages, maps the voltage
 * to a delay for controlling LED chase speed, and displays voltage and delay
 * information to the UART console.
 *
 * @return int Program exit status (0 for successful completion).
 */
int main() {
   int pos = 0;              //LED position (0 is rightmost, 15 is leftmost)...
   int direction = -1;       //-1 for left, 1 for right...
   int delay;
   int min_delay = 50;      //Minimum delay for the fastest speed...
   int max_delay = 1000;     //Maximum delay for the slowest speed...

   while (1) {
      //Read voltage from analog channel, assumed to be in the range 0-1V...
      double reading = adc.read_adc_in(READ_CHANNEL);

      //Map voltage (0-1V) to the delay range...
      delay = max_delay - (int)((reading) * (max_delay - min_delay));

      //Ensure delay does not go below the minimum delay threshold...
      if (delay < min_delay) {
         delay = min_delay;
      }

      //Light up the current LED in the chase pattern...
      led.write(1 << pos);

      //Delay based on the voltage-controlled speed...
      sleep_ms(delay);

      //Turn off the LED before moving to the next position...
      led.write(0);

      //Adjust position and direction for the chase...
      if (pos == 0) {
         direction = 1;  //Change direction to right (increment)...
      } else if (pos == 15) {
         direction = -1; //Change direction to left (decrement)...
      }

      pos += direction;  //Move LED in the current direction...

      //Output voltage and speed information to UART after each cycle...
      uart.disp("Analog channel/voltage: ");
      uart.disp(READ_CHANNEL);
      uart.disp(" / ");
      uart.disp(reading, 3);  //Display voltage reading with 3 decimal places...
      uart.disp(" | Delay (ms): ");
      uart.disp(delay);
      uart.disp("\n\r");

      //Update the seven-segment display with the current reading...
      adc_voltage_to_sseg(&sseg, reading * 100); //Multuply by 100 to normalize...
   }
   return 0;
}
