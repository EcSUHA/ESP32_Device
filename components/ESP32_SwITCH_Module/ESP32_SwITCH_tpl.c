//#################################################################################################
//###  Function: CGI Code for SwITCH Feature - Generic for SCD-Engine
//###  ESP 8266EX SOC Activities ...
//###  (C) EcSUHA
//###  Maik Schulze, Sandfuhren 4, 38448 Wolfsburg, Germany 
//###  MSchulze780@GMAIL.COM
//###  EcSUHA - ECONOMIC SURVEILLANCE AND HOME AUTOMATION - WWW.EcSUHA.DE
//##################################################################################################
#include <ProjectConfig.h>
#include <esp8266.h>
#include <Platform.h>

// the Smart-Connected-Device-Engine structures & types ..
#include <SCDE_s.h>

// provides WebIf, need the structures & types ...
#include "WebIf_Module.h"

#include "ESP32_SwITCH_Module.h"
#include "ESP32_SwITCH_tpl.h"



/*
 *--------------------------------------------------------------------------------------------------
 *FName: SwITCH_tpl
 * Desc: Template %X% replacement Code for the /X/SwITCH.htm Page
 * Info: WARNING: CHECK BUFFERSIZE
 * Para: HTTPDConnSlotData *conn -> ptr to connection slot
 *       char *token -> ptr to string which contains the token to replace
 *       void **arg -> ???
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
void
ESP32_SwITCH_tpl(WebIf_HTTPDConnSlotData_t* conn, char* token, void** arg) 
{
  char buff[600];
	
  if ( token == NULL ) return;

//--------------------------------------------------------------------------------------------------	
/*
  int  ADID  = conn->ActiveDirID -1;			// Index value from conn->ActiveDirID
//  uint32_t Key32 =( ((uint8_t)token[0]<<24) |		// 32 Bit Token Extraction (Key32==0x4e414d45)
//	((uint8_t)token[1]<<16) |
//	((uint8_t)token[2]<< 8) |
//	 (uint8_t)token[3] );
*/
//--------------------------------------------------------------------------------------------------

  switch(token[0])
	{

//--------------------------------------------------------------------------------------------------
/*/ now done by WebIfToken s
	case '?':				// '?' -> Device Name requested!

	{

	// get Devicename from current Service AP settings
	struct softap_config softap_config;
	wifi_softap_get_config (&softap_config);

	os_strcpy(buff,
		(char*) &softap_config.ssid);

	}

	break;

//--------------------------------------------------------------------------------------------------

	case '!':				// '!' -> Active destination URL replacement

	if (SysCfgRamFlashMirrored->MySCDEETX_Cfg.ETX_GenOpt & F_ADRESS_DIRECT)	// do direct adressing

		{
		os_sprintf(buff, "/%d/SwITCH.cgi", conn->ActiveDirID); 
		}

  	else									// do indirect adressing

		{
		os_sprintf(buff, "/%s/SwITCH.cgi",
			SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[conn->ActiveDirID-1].IB01_Name); 
		}

	break;
*/
//--------------------------------------------------------------------------------------------------

	case 'A':				// 'A' -> Requested feature number (ADID)

	if (conn->p_entry_active_dir_matching_definition) {

	// make common ptr to modul specific ptr
	ESP32_SwITCH_Definition_t* entry_esp32_switch_definition =
		  (ESP32_SwITCH_Definition_t*) conn->p_entry_active_dir_matching_definition;

		sprintf(buff, "%.*s",
			entry_esp32_switch_definition->common.nameLen,
			entry_esp32_switch_definition->common.name);
	
	}

	// An 'definition' is NOT assigned! -> Output .jso for all 'definition's of this 'module'
	else {

		sprintf(buff, "SwITCH");
	}

	break;

//--------------------------------------------------------------------------------------------------
/*
	case 'B':				// 'B' -> Last buildin Feature Number X (0-X)

	os_sprintf(buff, "%d",
		SysCfgRamFlashMirrored->MySwITCH_FeatHWCfg.NumFeat);

	break;
//--------------------------------------------------------------------------------------------------

	case 'C':				// 'C' -> NAME (MAX31+1)

	os_strcpy(buff,
		SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_Name);
	break;

//--------------------------------------------------------------------------------------------------

	case 'D':				// 'D' -> TIme-STamp

	os_sprintf(buff, "%d",
		(int) SysCfgRamNoMirror->MySwITCH_FeatCfgRamNoMirror[ADID].RB01_X_TiSt);

	break;

//--------------------------------------------------------------------------------------------------

	case 'E':				// 'E' -> current Value of Feature

	os_sprintf(buff, "%u",
		(uint) SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[ADID].RB01_Val);

	break;

//--------------------------------------------------------------------------------------------------

	case 'F':				// 'F' -> Minimal Value of Feature

	os_sprintf(buff, "%u",
		(uint) SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMin);

	break;

//--------------------------------------------------------------------------------------------------

	case 'G':				// 'G' -> Maximal Value of Feature

	os_sprintf(buff, "%u",
		(uint) SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMax);

	break;
//--------------------------------------------------------------------------------------------------

	case 'H':				// 'H' -> current DIM-Value of Feature

	os_sprintf(buff, "%u",
		(100 *
		(SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[ADID].RB01_Val -
		SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMin) / 
		(SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMax - 
		SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[ADID].IB01_SwValMin) ) );

	break;
*/
//--------------------------------------------------------------------------------------------------

	default:				// Not found? Zero terminate!

	buff[0] = 0;

	break;

//--------------------------------------------------------------------------------------------------
	}

  HTTPD_Send_To_Send_Buffer(conn, buff, -1);
}



