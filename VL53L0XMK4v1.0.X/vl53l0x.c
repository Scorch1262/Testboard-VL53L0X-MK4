#define FCY 16000000UL

#include "xc.h"
#include <assert.h>
#include <stdbool.h>
#include <libpic30.h>
#include <stdint.h>
#include "mcc_generated_files/system.h"
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

#define VL53L0X_ADDR    0x29

#define SYSRANGE_START                              0x00
#define SYSTEM_THRESH_HIGH                          0x0C
#define SYSTEM_THRESH_LOW                           0x0E
#define SYSTEM_SEQUENCE_CONFIG                      0x01
#define SYSTEM_RANGE_CONFIG                         0x09
#define SYSTEM_INTERMEASUREMENT_PERIOD              0x04
#define SYSTEM_INTERRUPT_CONFIG_GPIO                0x0A
#define GPIO_HV_MUX_ACTIVE_HIGH                     0x84
#define SYSTEM_INTERRUPT_CLEAR                      0x0B
#define RESULT_INTERRUPT_STATUS                     0x13
#define RESULT_RANGE_STATUS                         0x14
#define RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN       0xBC
#define RESULT_CORE_RANGING_TOTAL_EVENTS_RTN        0xC0
#define RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF       0xD0
#define RESULT_CORE_RANGING_TOTAL_EVENTS_REF        0xD4
#define RESULT_PEAK_SIGNAL_RATE_REF                 0xB6
#define ALGO_PART_TO_PART_RANGE_OFFSET_MM           0x28
#define I2C_SLAVE_DEVICE_ADDRESS                    0x8A
#define MSRC_CONFIG_CONTROL                         0x60
#define PRE_RANGE_CONFIG_MIN_SNR                    0x27
#define PRE_RANGE_CONFIG_VALID_PHASE_LOW            0x56
#define PRE_RANGE_CONFIG_VALID_PHASE_HIGH           0x57
#define PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT          0x64
#define FINAL_RANGE_CONFIG_MIN_SNR                  0x67
#define FINAL_RANGE_CONFIG_VALID_PHASE_LOW          0x47
#define FINAL_RANGE_CONFIG_VALID_PHASE_HIGH         0x48
#define FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT 0x44
#define PRE_RANGE_CONFIG_SIGMA_THRESH_HI            0x61
#define PRE_RANGE_CONFIG_SIGMA_THRESH_LO            0x62
#define PRE_RANGE_CONFIG_VCSEL_PERIOD               0x50
#define PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI          0x51
#define PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO          0x52
#define SYSTEM_HISTOGRAM_BIN                        0x81
#define HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT       0x33
#define HISTOGRAM_CONFIG_READOUT_CTRL               0x55
#define FINAL_RANGE_CONFIG_VCSEL_PERIOD             0x70
#define FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI        0x71
#define FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO        0x72
#define CROSSTALK_COMPENSATION_PEAK_RATE_MCPS       0x20
#define MSRC_CONFIG_TIMEOUT_MACROP                  0x46
#define SOFT_RESET_GO2_SOFT_RESET_N                 0xBF
#define IDENTIFICATION_MODEL_ID                     0xC0
#define IDENTIFICATION_REVISION_ID                  0xC2
#define OSC_CALIBRATE_VAL                           0xF8
#define GLOBAL_CONFIG_VCSEL_WIDTH                   0x32
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_0            0xB0
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_1            0xB1
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_2            0xB2
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_3            0xB3
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_4            0xB4
#define GLOBAL_CONFIG_SPAD_ENABLES_REF_5            0xB5
#define GLOBAL_CONFIG_REF_EN_START_SELECT           0xB6
#define DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD         0x4E
#define DYNAMIC_SPAD_REF_EN_START_OFFSET            0x4F
#define POWER_MANAGEMENT_GO1_POWER_FORCE            0x80
#define VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV           0x89
#define ALGO_PHASECAL_LIM                           0x30
#define ALGO_PHASECAL_CONFIG_TIMEOUT                0x30

I2C1_MESSAGE_STATUS status;                                             //

uint8_t Slave_Address;                                                  // definiert "Slave_Address"

uint8_t  writeBuffer[10] = {0x00, 0x00, 0x00};                          // definiert "writeBuffer"
uint8_t  readBuffer[10] = {0x00, 0x00, 0x00};                           // definiert "readBuffer"
uint16_t retryTimeOut1;                                                 // definiert "retryTimeOut1"
uint16_t retryTimeOut2;                                                 // definiert "retryTimeOut2"

void VL53L0X_Init(void){                                                        // "VL53L0X_Init"

    Slave_Address = VL53L0X_ADDR;                                               // schreibt "STTS22H_ADDR" in "Slave_Address"

	writeBuffer[0] = VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV;                         // schreibt "VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV;                         // schreibt "VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x88;                                                      // schreibt "0x88" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_CORE_RANGING_TOTAL_EVENTS_RTN;                      // schreibt "RESULT_CORE_RANGING_TOTAL_EVENTS_RTN" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = OSC_CALIBRATE_VAL;                                         // schreibt "OSC_CALIBRATE_VAL" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = SYSTEM_INTERMEASUREMENT_PERIOD;                            //
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 4, Slave_Address, &status);                 // liest von  I2C1 in 4 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = CROSSTALK_COMPENSATION_PEAK_RATE_MCPS;                     // schreibt "CROSSTALK_COMPENSATION_PEAK_RATE_MCPS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = ALGO_PART_TO_PART_RANGE_OFFSET_MM;                         // schreibt "ALGO_PART_TO_PART_RANGE_OFFSET_MM" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT;               // schreibt "FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT;                        // schreibt "PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT;                        // schreibt "PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_TIMEOUT_MACROP;                                // schreibt "MSRC_CONFIG_TIMEOUT_MACROP" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI;                        // schreibt "PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI;                        // schreibt "PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_VCSEL_PERIOD;                           // schreibt "FINAL_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI;                      // schreibt "FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x91;                                                      // schreibt "0x91" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = 0x44;                                                      //
    writeBuffer[1] = 0x00;                                                      //
    writeBuffer[2] = 0x20;                                                      //
	I2C1_MasterWrite(&writeBuffer[0], 3, Slave_Address, &status);               // schreibt 3 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_CONTROL;                                       // schreibt "MSRC_CONFIG_CONTROL" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_CONTROL;                                       // schreibt "MSRC_CONFIG_CONTROL" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_CONTROL;                                       // schreibt "MSRC_CONFIG_CONTROL" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_CONTROL;                                       // schreibt "MSRC_CONFIG_CONTROL" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_CONTROL;                                       // schreibt "MSRC_CONFIG_CONTROL" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_CONTROL;                                       // schreibt "MSRC_CONFIG_CONTROL" in "writeBuffer[0]"
	writeBuffer[1] = 0x02;                                                      // schreibt "0x02" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_CONTROL;                                       // schreibt "MSRC_CONFIG_CONTROL" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_CONTROL;                                       // schreibt "MSRC_CONFIG_CONTROL" in "writeBuffer[0]"
	writeBuffer[1] = 0x12;                                                      // schreibt "0x12" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = 0x44;                                                      //
    writeBuffer[1] = 0x00;                                                      //
    writeBuffer[2] = 0x20;                                                      //
	I2C1_MasterWrite(&writeBuffer[0], 3, Slave_Address, &status);               // schreibt 3 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = I2C_SLAVE_DEVICE_ADDRESS;                                  // schreibt "I2C_SLAVE_DEVICE_ADDRESS" in "writeBuffer[0]"
	writeBuffer[1] = 0x29;                                                      // schreibt "0x29" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x06;                                                      // schreibt "0x06" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x05;                                                      // schreibt "0x05" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x07;                                                      // schreibt "0x07" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_HISTOGRAM_BIN;                                      // schreibt "SYSTEM_HISTOGRAM_BIN" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x94;                                                      // schreibt "0x94" in "writeBuffer[0]"
	writeBuffer[1] = 0x02;                                                      // schreibt "0x02" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x90;                                                      // schreibt "0x90" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x94;                                                      // schreibt "0x94" in "writeBuffer[0]"
	writeBuffer[1] = 0x7B;                                                      // schreibt "0x7B" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x90;                                                      // schreibt "0x90" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x94;                                                      // schreibt "0x94" in "writeBuffer[0]"
	writeBuffer[1] = 0x77;                                                      // schreibt "0x77" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = 0x90;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 4, Slave_Address, &status);                 // liest von  I2C1 in 4 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x94;                                                      // schreibt "0x94" in "writeBuffer[0]"
	writeBuffer[1] = 0x78;                                                      // schreibt "0x78" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = 0x90;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 4, Slave_Address, &status);                 // liest von  I2C1 in 4 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x94;                                                      // schreibt "0x94" in "writeBuffer[0]"
	writeBuffer[1] = 0x79;                                                      // schreibt "0x79" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = 0x90;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 4, Slave_Address, &status);                 // liest von  I2C1 in 4 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x94;                                                      // schreibt "0x94" in "writeBuffer[0]"
	writeBuffer[1] = 0x7A;                                                      // schreibt "0x7A" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = 0x90;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 4, Slave_Address, &status);                 // liest von  I2C1 in 4 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_HISTOGRAM_BIN;                                      // schreibt "SYSTEM_HISTOGRAM_BIN" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x06;                                                      // schreibt "0x06" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_CORE_RANGING_TOTAL_EVENTS_RTN;                      // schreibt "RESULT_CORE_RANGING_TOTAL_EVENTS_RTN" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = IDENTIFICATION_REVISION_ID;                                // schreibt "IDENTIFICATION_REVISION_ID" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x06;                                                      // schreibt "0x06" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x05;                                                      // schreibt "0x05" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x07;                                                      // schreibt "0x07" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_HISTOGRAM_BIN;                                      // schreibt "SYSTEM_HISTOGRAM_BIN" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x94;                                                      // schreibt "0x94" in "writeBuffer[0]"
	writeBuffer[1] = 0x6B;                                                      // schreibt "0x6B" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
      writeBuffer[0] = 0x90;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 4, Slave_Address, &status);                 // liest von  I2C1 in 4 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x94;                                                      // schreibt "0x94" in "writeBuffer[0]"
	writeBuffer[1] = 0x24;                                                      // schreibt "0x24" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
      writeBuffer[0] = 0x90;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 4, Slave_Address, &status);                 // liest von  I2C1 in 4 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x94;                                                      // schreibt "0x94" in "writeBuffer[0]"
	writeBuffer[1] = 0x25;                                                      // schreibt "0x25" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = 0x90;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 4, Slave_Address, &status);                 // liest von  I2C1 in 4 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_HISTOGRAM_BIN;                                      // schreibt "SYSTEM_HISTOGRAM_BIN" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x06;                                                      // schreibt "0x06" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = DYNAMIC_SPAD_REF_EN_START_OFFSET;                          // schreibt "DYNAMIC_SPAD_REF_EN_START_OFFSET" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD;                       // schreibt "DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD" in "writeBuffer[0]"
	writeBuffer[1] = 0x2C;                                                      // schreibt "0x2C" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_PEAK_SIGNAL_RATE_REF;                               // schreibt "RESULT_PEAK_SIGNAL_RATE_REF" in "writeBuffer[0]"
	writeBuffer[1] = 0xB4;                                                      // schreibt "0xB4" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = GLOBAL_CONFIG_SPAD_ENABLES_REF_0;
    writeBuffer[1] = 0x00;
    writeBuffer[2] = 0xF0;
    writeBuffer[3] = 0xE7;
    writeBuffer[4] = 0x3F;
    writeBuffer[5] = 0x00;
    writeBuffer[6] = 0x00;
	I2C1_MasterWrite(&writeBuffer[0], 7, Slave_Address, &status);               // schreibt 7 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
      writeBuffer[0] = GLOBAL_CONFIG_SPAD_ENABLES_REF_0;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 6, Slave_Address, &status);                 // liest von  I2C1 in 6 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_RANGE_CONFIG;                                       // schreibt "SYSTEM_RANGE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x10;                                                      // schreibt "0x10" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x11;                                                      // schreibt "0x11" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x24;                                                      // schreibt "0x24" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x25;                                                      // schreibt "0x25" in "writeBuffer[0]"
	writeBuffer[1] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x75;                                                      // schreibt "0x75" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD;                       // schreibt "DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD" in "writeBuffer[0]"
	writeBuffer[1] = 0x2C;                                                      // schreibt "0x2C" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_VALID_PHASE_HIGH;                       // schreibt "FINAL_RANGE_CONFIG_VALID_PHASE_HIGH" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x30;                                                      // schreibt "0x30" in "writeBuffer[0]"
	writeBuffer[1] = 0x20;                                                      // schreibt "0x20" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x30;                                                      // schreibt "0x30" in "writeBuffer[0]"
	writeBuffer[1] = 0x09;                                                      // schreibt "0x09" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x54;                                                      // schreibt "0x54" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x31;                                                      // schreibt "0x31" in "writeBuffer[0]"
	writeBuffer[1] = 0x04;                                                      // schreibt "0x04" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = GLOBAL_CONFIG_VCSEL_WIDTH;                                 // schreibt "GLOBAL_CONFIG_VCSEL_WIDTH" in "writeBuffer[0]"
	writeBuffer[1] = 0x03;                                                      // schreibt "0x03" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x40;                                                      // schreibt "0x40" in "writeBuffer[0]"
	writeBuffer[1] = 0x83;                                                      // schreibt "0x83" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_TIMEOUT_MACROP;                                // schreibt "MSRC_CONFIG_TIMEOUT_MACROP" in "writeBuffer[0]"
	writeBuffer[1] = 0x25;                                                      // schreibt "0x25" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_CONTROL;                                       // schreibt "MSRC_CONFIG_CONTROL" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_MIN_SNR;                                  // schreibt "PRE_RANGE_CONFIG_MIN_SNR" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	writeBuffer[1] = 0x06;                                                      // schreibt "0x06" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI;                        // schreibt "PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO;                        // schreibt "PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO" in "writeBuffer[0]"
	writeBuffer[1] = 0x96;                                                      // schreibt "0x96" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VALID_PHASE_LOW;                          // schreibt "PRE_RANGE_CONFIG_VALID_PHASE_LOW" in "writeBuffer[0]"
	writeBuffer[1] = 0x08;                                                      // schreibt "0x08" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VALID_PHASE_HIGH;                         // schreibt "PRE_RANGE_CONFIG_VALID_PHASE_HIGH" in "writeBuffer[0]"
	writeBuffer[1] = 0x30;                                                      // schreibt "0x30" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_SIGMA_THRESH_HI;                          // schreibt "PRE_RANGE_CONFIG_SIGMA_THRESH_HI" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_SIGMA_THRESH_LO;                          // schreibt "PRE_RANGE_CONFIG_SIGMA_THRESH_LO" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT;                        // schreibt "PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x65;                                                      // schreibt "0x65" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x66;                                                      // schreibt "0x66" in "writeBuffer[0]"
	writeBuffer[1] = 0xA0;                                                      // schreibt "0xA0" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x22;                                                      // schreibt "0x22" in "writeBuffer[0]"
	writeBuffer[1] = 0x32;                                                      // schreibt "0x32" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_VALID_PHASE_LOW;                        // schreibt "FINAL_RANGE_CONFIG_VALID_PHASE_LOW" in "writeBuffer[0]"
	writeBuffer[1] = 0x14;                                                      // schreibt "0x14" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x49;                                                      // schreibt "0x49" in "writeBuffer[0]"
	writeBuffer[1] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x4A;                                                      // schreibt "0x4A" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x7A;                                                      // schreibt "0x7A" in "writeBuffer[0]"
	writeBuffer[1] = 0x0A;                                                      // schreibt "0x0A" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x7B;                                                      // schreibt "0x7B" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x78;                                                      // schreibt "0x78" in "writeBuffer[0]"
	writeBuffer[1] = 0x21;                                                      // schreibt "0x21" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x23;                                                      // schreibt "0x23" in "writeBuffer[0]"
	writeBuffer[1] = 0x34;                                                      // schreibt "0x34" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x42;                                                      // schreibt "0x42" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT;               // schreibt "FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT" in "writeBuffer[0]"
	writeBuffer[1] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x45;                                                      // schreibt "0x45" in "writeBuffer[0]"
	writeBuffer[1] = 0x26;                                                      // schreibt "0x26" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_TIMEOUT_MACROP;                                // schreibt "MSRC_CONFIG_TIMEOUT_MACROP" in "writeBuffer[0]"
	writeBuffer[1] = 0x05;                                                      // schreibt "0x05" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x40;                                                      // schreibt "0x40" in "writeBuffer[0]"
	writeBuffer[1] = 0x40;                                                      // schreibt "0x40" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_THRESH_LOW;                                         // schreibt "SYSTEM_THRESH_LOW" in "writeBuffer[0]"
	writeBuffer[1] = 0x06;                                                      // schreibt "0x06" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = CROSSTALK_COMPENSATION_PEAK_RATE_MCPS;                     // schreibt "CROSSTALK_COMPENSATION_PEAK_RATE_MCPS" in "writeBuffer[0]"
	writeBuffer[1] = 0x1A;                                                      // schreibt "0x1A" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x43;                                                      // schreibt "0x43" in "writeBuffer[0]"
	writeBuffer[1] = 0x40;                                                      // schreibt "0x40" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x34;                                                      // schreibt "0x34" in "writeBuffer[0]"
	writeBuffer[1] = 0x03;                                                      // schreibt "0x03" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x35;                                                      // schreibt "0x35" in "writeBuffer[0]"
	writeBuffer[1] = 0x44;                                                      // schreibt "0x44" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x31;                                                      // schreibt "0x31" in "writeBuffer[0]"
	writeBuffer[1] = 0x04;                                                      // schreibt "0x04" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x4B;                                                      // schreibt "0x4B" in "writeBuffer[0]"
	writeBuffer[1] = 0x09;                                                      // schreibt "0x09" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x4C;                                                      // schreibt "0x4C" in "writeBuffer[0]"
	writeBuffer[1] = 0x05;                                                      // schreibt "0x05" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x4D;                                                      // schreibt "0x4D" in "writeBuffer[0]"
	writeBuffer[1] = 0x04;                                                      // schreibt "0x04" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT;               // schreibt "FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x45;                                                      // schreibt "0x45" in "writeBuffer[0]"
	writeBuffer[1] = 0x20;                                                      // schreibt "0x20" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_VALID_PHASE_LOW;                        // schreibt "FINAL_RANGE_CONFIG_VALID_PHASE_LOW" in "writeBuffer[0]"
	writeBuffer[1] = 0x08;                                                      // schreibt "0x08" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_VALID_PHASE_HIGH;                       // schreibt "FINAL_RANGE_CONFIG_VALID_PHASE_HIGH" in "writeBuffer[0]"
	writeBuffer[1] = 0x28;                                                      // schreibt "0x28" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_MIN_SNR;                                // schreibt "FINAL_RANGE_CONFIG_MIN_SNR" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_VCSEL_PERIOD;                           // schreibt "FINAL_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	writeBuffer[1] = 0x04;                                                      // schreibt "0x04" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI;                      // schreibt "FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO;                      // schreibt "FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO" in "writeBuffer[0]"
	writeBuffer[1] = 0xFE;                                                      // schreibt "0xFE" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x76;                                                      // schreibt "0x76" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x77;                                                      // schreibt "0x77" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x0D;                                                      // schreibt "0x0D" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0xF8;                                                      // schreibt "0xF8" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x8E;                                                      // schreibt "0x8E" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CONFIG_GPIO;                              // schreibt "SYSTEM_INTERRUPT_CONFIG_GPIO" in "writeBuffer[0]"
	writeBuffer[1] = 0x04;                                                      // schreibt "0x04" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = GPIO_HV_MUX_ACTIVE_HIGH;                                   // schreibt "GPIO_HV_MUX_ACTIVE_HIGH" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = GPIO_HV_MUX_ACTIVE_HIGH;                                   // schreibt "GPIO_HV_MUX_ACTIVE_HIGH" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = GPIO_HV_MUX_ACTIVE_HIGH;                                   // schreibt "GPIO_HV_MUX_ACTIVE_HIGH" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = OSC_CALIBRATE_VAL;                                         // schreibt "OSC_CALIBRATE_VAL" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
      writeBuffer[0] = SYSTEM_INTERMEASUREMENT_PERIOD;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 4, Slave_Address, &status);                 // liest von  I2C1 in 4 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = CROSSTALK_COMPENSATION_PEAK_RATE_MCPS;                     // schreibt "CROSSTALK_COMPENSATION_PEAK_RATE_MCPS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = ALGO_PART_TO_PART_RANGE_OFFSET_MM;                         // schreibt "ALGO_PART_TO_PART_RANGE_OFFSET_MM" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT;               // schreibt "FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT;                        // schreibt "PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT;                        // schreibt "PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_TIMEOUT_MACROP;                                // schreibt "MSRC_CONFIG_TIMEOUT_MACROP" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI;                        // schreibt "PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt ""SYSTEM_SEQUENCE_CONFIG in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI;                        // schreibt "PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_VCSEL_PERIOD;                           // schreibt "FINAL_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI;                      // schreibt "FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_RANGE_CONFIG;                                       // schreibt "SYSTEM_RANGE_CONFIG" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0xE8;                                                      // schreibt "0xE8" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = MSRC_CONFIG_TIMEOUT_MACROP;                                // schreibt "MSRC_CONFIG_TIMEOUT_MACROP" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI;                        // schreibt "PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI;                        // schreibt "PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_VCSEL_PERIOD;                           // schreibt "FINAL_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = 0x71;
    writeBuffer[1] = 0x02;
    writeBuffer[2] = 0x94;
	I2C1_MasterWrite(&writeBuffer[0], 3, Slave_Address, &status);               // schreibt 3 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_VCSEL_PERIOD;                           // schreibt "FINAL_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI;                        // schreibt "PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_VCSEL_PERIOD;                             // schreibt "PRE_RANGE_CONFIG_VCSEL_PERIOD" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI;                        // schreibt "PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_VCSEL_PERIOD;                           // schreibt "" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI;                      // schreibt "FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = DYNAMIC_SPAD_REF_EN_START_OFFSET;                          // schreibt "DYNAMIC_SPAD_REF_EN_START_OFFSET" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD;                       // schreibt "DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD" in "writeBuffer[0]"
	writeBuffer[1] = 0x2C;                                                      // schreibt "0x2C" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_PEAK_SIGNAL_RATE_REF;                               // schreibt "RESULT_PEAK_SIGNAL_RATE_REF" in "writeBuffer[0]"
	writeBuffer[1] = 0xB4;                                                      // schreibt "0xB4" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x41;                                                      // schreibt "0x41" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0x02;                                                      // schreibt "" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0xE8;                                                      // schreibt "0xE8" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = GLOBAL_CONFIG_SPAD_ENABLES_REF_0;
    writeBuffer[1] = 0x07;
    writeBuffer[2] = 0x00;
    writeBuffer[3] = 0x00;
    writeBuffer[4] = 0x00;
    writeBuffer[5] = 0x00;
    writeBuffer[6] = 0x00;
	I2C1_MasterWrite(&writeBuffer[0], 7, Slave_Address, &status);               // schreibt 7 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = GLOBAL_CONFIG_SPAD_ENABLES_REF_0;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 6, Slave_Address, &status);                 // liest von  I2C1 in 6 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0xC0;                                                      // schreibt "0xC0" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x91;                                                      // schreibt "0x91" in "writeBuffer[0]"
	writeBuffer[1] = 0x3C;                                                      // schreibt "0x3C" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = RESULT_RANGE_STATUS;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 12, Slave_Address, &status);                // liest von  I2C1 in 12 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_PEAK_SIGNAL_RATE_REF;                               // schreibt "RESULT_PEAK_SIGNAL_RATE_REF" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_PEAK_SIGNAL_RATE_REF;                               // schreibt "RESULT_PEAK_SIGNAL_RATE_REF" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0xE8;                                                      // schreibt "0xE8" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = GLOBAL_CONFIG_SPAD_ENABLES_REF_0;
    writeBuffer[1] = 0x0F;
    writeBuffer[2] = 0x00;
    writeBuffer[3] = 0x00;
    writeBuffer[4] = 0x00;
    writeBuffer[5] = 0x00;
    writeBuffer[6] = 0x00;
	I2C1_MasterWrite(&writeBuffer[0], 7, Slave_Address, &status);               // schreibt 7 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0xC0;                                                      // schreibt "0xC0" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x91;                                                      // schreibt "0x91" in "writeBuffer[0]"
	writeBuffer[1] = 0x3C;                                                      // schreibt "0x3C" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = RESULT_RANGE_STATUS;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 12, Slave_Address, &status);                // liest von  I2C1 in 12 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_PEAK_SIGNAL_RATE_REF;                               // schreibt "RESULT_PEAK_SIGNAL_RATE_REF" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_PEAK_SIGNAL_RATE_REF;                               // schreibt "RESULT_PEAK_SIGNAL_RATE_REF" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0xE8;                                                      // schreibt "0xE8" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = GLOBAL_CONFIG_SPAD_ENABLES_REF_0;
    writeBuffer[1] = 0x1F;
    writeBuffer[2] = 0x00;
    writeBuffer[3] = 0x00;
    writeBuffer[4] = 0x00;
    writeBuffer[5] = 0x00;
    writeBuffer[6] = 0x00;
	I2C1_MasterWrite(&writeBuffer[0], 7, Slave_Address, &status);               // schreibt 7 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0xC0;                                                      // schreibt "0xC0" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x91;                                                      // schreibt "0x91" in "writeBuffer[0]"
	writeBuffer[1] = 0x3C;                                                      // schreibt "0x3C" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = RESULT_RANGE_STATUS;
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 12, Slave_Address, &status);                // liest von  I2C1 in 12 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_PEAK_SIGNAL_RATE_REF;                               // schreibt "RESULT_PEAK_SIGNAL_RATE_REF" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_PEAK_SIGNAL_RATE_REF;                               // schreibt "RESULT_PEAK_SIGNAL_RATE_REF" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0xE8;                                                      // schreibt "0xE8" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x41;                                                      // schreibt "0x41" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xCB;                                                      // schreibt "0xCB" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0x02;                                                      // schreibt "0x02" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xEE;                                                      // schreibt "0xEE" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_SEQUENCE_CONFIG;                                    // schreibt "SYSTEM_SEQUENCE_CONFIG" in "writeBuffer[0]"
	writeBuffer[1] = 0xE8;                                                      // schreibt "0xE8" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = 0x44;
    writeBuffer[1] = 0x00;
    writeBuffer[2] = 0x20;
	I2C1_MasterWrite(&writeBuffer[0], 3, Slave_Address, &status);               // schreibt 3 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    __delay_ms(10);                                                             // warte 10ms
}                                                                               //

float VL53L0X_Millimeter_GetValue(void){                                        // "VL53L0X_Millimeter_GetValue"
    float       DistDistanzAnz;                                                 // definiert "DistDistanzAnz"
    uint16_t    data16;                                                         // definiert "data16"

    Slave_Address = VL53L0X_ADDR;                                               // schreibt "STTS22H_ADDR" in "Slave_Address"

	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt "POWER_MANAGEMENT_GO1_POWER_FORCE" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x91;                                                      // schreibt "0x91" in "writeBuffer[0]"
	writeBuffer[1] = 0x3C;                                                      // schreibt "0x3C" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = POWER_MANAGEMENT_GO1_POWER_FORCE;                          // schreibt POWER_MANAGEMENT_GO1_POWER_FORCE"" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 1, Slave_Address, &status);                 // liest von  I2C1 in 1 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    writeBuffer[0] = 0x1E; //RESULT_RANGE_STATUS                                //
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 12, Slave_Address, &status);                // liest von  I2C1 in 12 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf

    data16 = (readBuffer[0] << 8) + readBuffer[1];                              //

	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_PEAK_SIGNAL_RATE_REF;                               // schreibt "RESULT_PEAK_SIGNAL_RATE_REF" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = 0xFF;                                                      // schreibt "0xFF" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x01;                                                      // schreibt "0x01" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = SYSTEM_INTERRUPT_CLEAR;                                    // schreibt "SYSTEM_INTERRUPT_CLEAR" in "writeBuffer[0]"
	writeBuffer[1] = 0x00;                                                      // schreibt "0x00" in "writeBuffer[1]"
	I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);               // schreibt 2 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
	writeBuffer[0] = RESULT_INTERRUPT_STATUS;                                   // schreibt "RESULT_INTERRUPT_STATUS" in "writeBuffer[0]"
	I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);               // schreibt 1 Byte (writeBuffer[0]) an I2C1
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf
    I2C1_MasterRead(&readBuffer[0], 2, Slave_Address, &status);                 // liest von  I2C1 in 2 Byte (readBuffer[0])
		i2c1_message_pending_100();                                             // ruft "i2c1_message_pending_100" auf

    DistDistanzAnz = data16;                                                    // kopiert "data32" in "DistDistanzAnz"

    return DistDistanzAnz;                                                      // gibt "DistDistanzAnz" zur?ck
}                                                                               //

void i2c1_message_pending_100(void){                                    // "i2c1_message_pending_100"
    retryTimeOut1 = 0;                                                  // setzt "retryTimeOut1" auf 0
    retryTimeOut2 = 0;                                                  // setzt "retryTimeOut2" auf 0
    while(status == I2C1_MESSAGE_PENDING){                              // solange "status" gleich "I2C1_MESSAGE_PENDING" ist
        if (retryTimeOut2 == 1000){                                      // wenn "retryTimeOut2" gleich 100 ist
            break;                                                      // Abbruch der Schleife
        }else{                                                          // sonst
            retryTimeOut2++;                                            // "retryTimeOut2" +1
        }                                                               //
    }
    while(status != I2C1_MESSAGE_FAIL){                                 // solange "status" nicht gleich "I2C1_MESSAGE_FAIL" ist
        if (retryTimeOut1 == 1000){                                      // wenn "retryTimeOut1" gleich 100 ist
            break;                                                      // Abbruch der Schleife
        }else{                                                          // sonst
            retryTimeOut1++;                                            // "retryTimeOut1" +1
        }                                                               //
    }                                                                   //
}                                                                       //
