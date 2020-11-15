// SCDE-Module BMP180 (BMP180 Module, connected via I2C, communication via IO-Device for I2C Master communication for I2C-hardware)

#ifndef BMP180_MODULE_H
#define BMP180_MODULE_H

// -------------------------------------------------------------------------------------------------

// this Module is made for the Smart-Connected-Device-Engine
#include "SCDE_s.h"

// this Module provides functions for other Modules:
#include "WebIf_Module global types.h"
//#include "WebIf_Module_s.h"

// this Module uses an 1st stage:
#include "ESP32_I2C_Master_Module_s.h"

// -------------------------------------------------------------------------------------------------

// stores the Root Data of the Smart Connected Devices Engine - at/for this Module
SCDERoot_t* SCDERoot_at_BMP180_M;

// stores SCDEFn (Functions / callbacks) provided for operation - at/for this Module
SCDEFn_t* SCDEFn_at_BMP180_M;

// -------------------------------------------------------------------------------------------------



// 
enum BMP180_QueryStates {
    s_INTIAL_DELAY = 0		// #00 default at start
  , s_BH1750_SET_MODE		// #01
  , s_BH1750_WAIT_24MS		// #02
  , s_BH1750_READ_DATA		// #03
};



/* 
 * BH1750_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct BMP180_Definition_s {
  Common_Definition_t common;		// ... the common part of the definition
  WebIf_Provided_t WebIf_Provided;	// provided data for WebIf

  size_t stage1definitionNameLen;	// Stange 1 name assigned by def
  uint8_t *stage1definitionName;	// Stange 1 nameLen assigned by def

  ESP32_I2C_Master_Definition_t* ESP32_I2C_Master_Definition; // stage 1 link -> wirklich speichern?

  uint8_t BMP180_QueryState;		// enum BH1750QueryState -> state machine state for sensor data query

  // BMP180 Settings
  uint32_t reference_pressure;		// users configuration
  uint8_t oversampling;			//

  // BMP180 Calibration parameters
  int16_t ac1;				//
  int16_t ac2;				//
  int16_t ac3;				//
  uint16_t ac4;				//
  uint16_t ac5;				//
  uint16_t ac6;				//
  int16_t b1;				//
  int16_t b2;				//
  int16_t mb;				//
  int16_t mc;				//
  int16_t md;				//


  // BMP180 measured values
  uint32_t pressure;			//
  float altitude;			//
  float temperature;			//
} BMP180_Definition_t;



/*
 *  Functions provided to SCDE by Module - for type operation (A-Z)
 */

// Attribute Fn - called in case of attribute changes for this definition, to check them
Entry_String_t* BMP180_Attribute (Common_Definition_t* p_entry_definition, const String_t attr_command, const String_t attr_name, const String_t attr_value);

strTextMultiple_t* BMP180_Define(Common_Definition_t *Common_Definition);

int                BMP180_IdleCb(Common_Definition_t *Common_Definition);

int                BMP180_Initialize(SCDERoot_t* SCDERoot);

strTextMultiple_t* BMP180_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);

strTextMultiple_t* BMP180_Shutdown(Common_Definition_t* Common_Definition);

strTextMultiple_t* BMP180_Undefine(Common_Definition_t* Common_Definition);



/*
 *  helpers provided to module for type operation
 */
bool BMP180_ProcessKVInputArgs(BMP180_Definition_t* BMP180_Definition, parsedKVInputArgs_t *parsedKVInput, uint8_t *argsText, size_t argsTextLen);

bool BMP180_SetAffectedReadings(BMP180_Definition_t* BMP180_Definition, uint64_t affectedReadings);








/*
 *  BMP180 helper Fn
 */

// ?
strTextMultiple_t* bmp180_master_write_slave(BMP180_Definition_t* BMP180_Definition, uint8_t* data_wr, size_t size);

//?
strTextMultiple_t* bmp180_write_reg(BMP180_Definition_t* BMP180_Definition, uint8_t reg, uint8_t cmd);

// ?
strTextMultiple_t* bmp180_master_read_slave(BMP180_Definition_t* BMP180_Definition, uint8_t* data_rd, size_t size);

// ?
strTextMultiple_t* bmp180_read_int16(BMP180_Definition_t* BMP180_Definition, uint8_t reg, int16_t* value);

// ?
strTextMultiple_t* bmp180_read_uint16(BMP180_Definition_t* BMP180_Definition, uint8_t reg, uint16_t* value);

// ?
strTextMultiple_t* bmp180_read_uint32(BMP180_Definition_t* BMP180_Definition, uint8_t reg, uint32_t* value);

// ?
strTextMultiple_t* bmp180_read_uncompensated_temperature(BMP180_Definition_t* BMP180_Definition, int16_t* temp);

// ?
strTextMultiple_t* bmp180_calculate_b5(BMP180_Definition_t* BMP180_Definition, int32_t* b5);

// ?
strTextMultiple_t* bmp180_read_uncompensated_pressure(BMP180_Definition_t* BMP180_Definition, uint32_t* up);

// ?
strTextMultiple_t* bmp180_read_temperature(BMP180_Definition_t* BMP180_Definition, float* temperature);

// ?
strTextMultiple_t* bmp180_read_pressure(BMP180_Definition_t* BMP180_Definition, uint32_t* pressure);

// ?
strTextMultiple_t*  bmp180_read_altitude(BMP180_Definition_t* BMP180_Definition, uint32_t reference_pressure, float* altitude);


#endif /*BMP180_MODULE_H*/
