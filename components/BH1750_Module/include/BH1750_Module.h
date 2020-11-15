// SCDE-Module BH1750 (BH1750 Module, connected via I2C, communication via IO-Device for I2C Master communication for I2C-hardware)

#ifndef BH1750_MODULE_H
#define BH1750_MODULE_H

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
SCDERoot_t* SCDERoot_at_BH1750_M;

// stores SCDEFn (Functions / callbacks) provided for operation - at/for this Module
SCDEFn_t* SCDEFn_at_BH1750_M;

// -------------------------------------------------------------------------------------------------



// 
enum BH1750_QueryStates {
    s_INTIAL_DELAY = 0		// #00 default at start
  , s_BH1750_SET_MODE		// #01
  , s_BH1750_WAIT_24MS		// #02
  , s_BH1750_READ_DATA		// #03
};



/* 
 * BH1750_Definition stores values for operation valid only for the defined instance of an
 * loaded module. Values are initialized by HCTRLD an the loaded module itself.
 */
typedef struct BH1750_Definition_s {
  Common_Definition_t common;		// ... the common part of the definition
  WebIf_Provided_t WebIf_Provided;	// provided data for WebIf

  size_t stage1definitionNameLen;	// Stange 1 name assigned by def
  uint8_t *stage1definitionName;	// Stange 1 nameLen assigned by def

  ESP32_I2C_Master_Fn_t* ESP32_I2C_Master_Fn;	/*!< ESP32_I2C_MasterFn (Functions / callbacks) accessiblefor client modules */

  uint8_t BH1750_QueryState;		// enum BH1750QueryState -> state machine state for sensor data query

  uint8_t sensor_data_h, sensor_data_l; // sensor data coming from I2C

  } BH1750_Definition_t;



/*
 *  Functions provided to SCDE by Module - for type operation (A-Z)
 */

// Attribute Fn - called in case of attribute changes for this definition, to check them
Entry_String_t* BH1750_Attribute (Common_Definition_t* p_entry_definition, const String_t attr_command, const String_t attr_name, const String_t attr_value);

strTextMultiple_t* BH1750_Define(Common_Definition_t *Common_Definition);

int                BH1750_IdleCb(Common_Definition_t *Common_Definition);

int                BH1750_Initialize(SCDERoot_t* SCDERoot);

strTextMultiple_t* BH1750_Set(Common_Definition_t* Common_Definition, uint8_t *setArgs, size_t setArgsLen);

strTextMultiple_t* BH1750_Shutdown(Common_Definition_t* Common_Definition);

strTextMultiple_t* BH1750_Undefine(Common_Definition_t* Common_Definition);



/*
 *  helpers provided to module for type operation
 */
bool BH1750_ProcessKVInputArgs(BH1750_Definition_t* BH1750_Definition, parsedKVInputArgs_t *parsedKVInput, uint8_t *argsText, size_t argsTextLen);

bool BH1750_SetAffectedReadings(BH1750_Definition_t* BH1750_Definition, uint64_t affectedReadings);



#endif /*BH1750_MODULE_H*/
