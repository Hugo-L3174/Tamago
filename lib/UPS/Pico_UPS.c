#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "Pico_UPS.h"


/*!
 *   @brief  Class that stores state and functions for interacting with INA219
 *  current/power monitor IC
 */
// void INA219 {
// // public:
//   INA219(uint8_t addr = INA219_ADDRESS);
  // void begin();
  // void setCalibration_32V_2A();
  // float getBusVoltage_V();
  // float getShuntVoltage_mV();
  // float getCurrent_mA();
  // float getPower_mW();
  // void powerSave(bool on);
  // void wireWriteRegister(uint8_t reg, uint16_t value);
  // void wireReadRegister(uint8_t reg, uint16_t *value);
// };

// /*!
//  *  @brief  Instantiates a new INA219 class
//  *  @param addr the I2C address the device can be found on. Default is 0x40
//  */
// void INA219(uint8_t addr) {
//   ina219_i2caddr = addr;
//   ina219_currentDivider_mA = 0;
//   ina219_powerMultiplier_mW = 0.0f;
// }

/*!
 *  @brief  Sends a single command byte over I2C
 *  @param  reg
 *          register address
 *  @param  value
 *          value to write
 */
void wireWriteRegister(uint8_t reg, uint16_t value) {
	
	uint8_t tmpi[3];
	tmpi[0] = reg;
	tmpi[1] = (value >> 8) & 0xFF;
	tmpi[2] = value & 0xFF;
	
	i2c_write_blocking(i2c1, INA219_ADDRESS, tmpi, 3, true); // true to keep master control of bus
}

/*!
 *  @brief  Reads a 16 bit values over I2C
 *  @param  reg
 *          register address
 *  @param  *value
 *          read value
 */
void wireReadRegister(uint8_t reg, uint16_t *value) {

	uint8_t tmpi[2];

	i2c_write_blocking(i2c1, INA219_ADDRESS, &reg, 1, true); // true to keep master control of bus
  i2c_read_blocking(i2c1, INA219_ADDRESS, tmpi, 2, true);
	*value = (((uint16_t)tmpi[0] << 8) | (uint16_t)tmpi[1]);
}

/*!
 *  @brief  Configures to INA219 to be able to measure up to 32V and 2A
 *          of current.  Each unit of current corresponds to 100uA, and
 *          each unit of power corresponds to 2mW. Counter overflow
 *          occurs at 3.2A.
 *  @note   These calculations assume a 0.1 ohm resistor is present
 */
void setCalibration_32V_2A() {
  // By default we use a pretty huge range for the input voltage,
  // which probably isn't the most appropriate choice for system
  // that don't use a lot of power.  But all of the calculations
  // are shown below if you want to change the settings.  You will
  // also need to change any relevant register settings, such as
  // setting the VBUS_MAX to 16V instead of 32V, etc.

  // VBUS_MAX = 32V             (Assumes 32V, can also be set to 16V)
  // VSHUNT_MAX = 0.32          (Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
  // RSHUNT = 0.1               (Resistor value in ohms)

  // 1. Determine max possible current
  // MaxPossible_I = VSHUNT_MAX / RSHUNT
  // MaxPossible_I = 3.2A

  // 2. Determine max expected current
  // MaxExpected_I = 2.0A

  // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
  // MinimumLSB = MaxExpected_I/32767
  // MinimumLSB = 0.000061              (61uA per bit)
  // MaximumLSB = MaxExpected_I/4096
  // MaximumLSB = 0,000488              (488uA per bit)

  // 4. Choose an LSB between the min and max values
  //    (Preferrably a roundish number close to MinLSB)
  // CurrentLSB = 0.0001 (100uA per bit)

  // 5. Compute the calibration register
  // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
  // Cal = 4096 (0x1000)

  ina219_calValue = 4096;

  // 6. Calculate the power LSB
  // PowerLSB = 20 * CurrentLSB
  // PowerLSB = 0.002 (2mW per bit)

  // 7. Compute the maximum current and shunt voltage values before overflow
  //
  // Max_Current = Current_LSB * 32767
  // Max_Current = 3.2767A before overflow
  //
  // If Max_Current > Max_Possible_I then
  //    Max_Current_Before_Overflow = MaxPossible_I
  // Else
  //    Max_Current_Before_Overflow = Max_Current
  // End If
  //
  // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
  // Max_ShuntVoltage = 0.32V
  //
  // If Max_ShuntVoltage >= VSHUNT_MAX
  //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
  // Else
  //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
  // End If

  // 8. Compute the Maximum Power
  // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
  // MaximumPower = 3.2 * 32V
  // MaximumPower = 102.4W

  // Set multipliers to convert raw current/power values
  ina219_currentDivider_mA = 1.0; // Current LSB = 100uA per bit (1000/100 = 10)
  ina219_powerMultiplier_mW = 20; // Power LSB = 1mW per bit (2/1)

  // Set Calibration register to 'Cal' calculated above
  wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

  // Set Config register to take into account the settings above
  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                    INA219_CONFIG_GAIN_8_320MV | INA219_CONFIG_BADCRES_12BIT |
                    INA219_CONFIG_SADCRES_12BIT_32S_17MS |
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
  wireWriteRegister(INA219_REG_CONFIG, config);
}

/*!
 *  @brief  Set power save mode according to parameters
 *  @param  on
 *          boolean value
 */
void powerSave(bool on) {
  uint16_t current;
  wireReadRegister(INA219_REG_CONFIG, &current);
  uint8_t next;
  if (on) {
    next = current | INA219_CONFIG_MODE_POWERDOWN; 
  } else {
    next = current & ~INA219_CONFIG_MODE_POWERDOWN; 
  }
  wireWriteRegister(INA219_REG_CONFIG, next);
}





/*!
 *  @brief  Setups the HW (defaults to 32V and 2A for calibration values)
 *  @param theWire the TwoWire object to use
 */
//void INA219::begin(TwoWire *theWire) {
void begin() {
  //_i2c = theWire;
  i2c_init(i2c1, 400 * 1000);
  gpio_set_function(6,GPIO_FUNC_I2C);
  gpio_set_function(7,GPIO_FUNC_I2C);
  gpio_pull_up(6);
  gpio_pull_up(7);
  setCalibration_32V_2A();
}

/*!
 *  @brief  Gets the shunt voltage in mV (so +-327mV)
 *  @return the shunt voltage converted to millivolts
 */
float getShuntVoltage_mV() {
  uint16_t value;
  wireReadRegister(INA219_REG_SHUNTVOLTAGE, &value);
  return (int16_t)value * 0.01;
}

/*!
 *  @brief  Gets the shunt voltage in volts
 *  @return the bus voltage converted to volts
 */
float getBusVoltage_V() {
	
  uint16_t value;
  wireReadRegister(INA219_REG_BUSVOLTAGE, &value);
  // Shift to the right 3 to drop CNVR and OVF and multiply by LSB
  return (int16_t)((value >> 3) * 4) * 0.001;
}

/*!
 *  @brief  Gets the current value in mA, taking into account the
 *          config settings and current LSB
 *  @return the current reading convereted to milliamps
 */
float getCurrent_mA() {
  uint16_t value;

  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

  // Now we can safely read the CURRENT register!
  wireReadRegister(INA219_REG_CURRENT, &value);
  float valueDec = (int16_t)value;
  valueDec /= ina219_currentDivider_mA;
  return valueDec;
}

/*!
 *  @brief  Gets the power value in mW, taking into account the
 *          config settings and current LSB
 *  @return power reading converted to milliwatts
 */
float getPower_mW() {
	
  uint16_t value;

  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

  // Now we can safely read the POWER register!
  wireReadRegister(INA219_REG_POWER, &value);
  
  float valueDec = (int16_t)value;
  valueDec *= ina219_powerMultiplier_mW;
  return valueDec;
}



// int main() {
// 	float bus_voltage = 0;
// 	float shunt_voltage = 0;
// 	float power = 0;
// 	float current = 0;
// 	float P=0;
// 	uint16_t value;
// 	// INA219 ina(INA219_ADDRESS);
	
//   const uint LED_PIN = PICO_DEFAULT_LED_PIN;
//   gpio_init(LED_PIN);
// 	stdio_init_all();
//   gpio_set_dir(LED_PIN, GPIO_OUT);
// 	begin();

//   while (true) {

//     bus_voltage = getBusVoltage_V();         // voltage on V- (load side)
//     current = getCurrent_mA()/1000;               // current in mA
//     P = (bus_voltage -3)/1.2*100;
//     if(P<0)
//     {
//       P=0;
//     }
//     else if (P>100)
//     {
//       P=100;
//     }
        
//     printf("Voltage:  %6.3f V\r\n",bus_voltage);
//     printf("Current:  %6.3f A\r\n",current);
//     printf("Percent:  %6.1f %%\r\n",P);
//     printf("\r\n");
  
//     gpio_put(LED_PIN, 1);
//     sleep_ms(1000);
//     gpio_put(LED_PIN, 0);
//     sleep_ms(1000);
//   }
// }
