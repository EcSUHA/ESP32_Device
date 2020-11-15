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
#include "ESP32_SwITCH_jso.h"

#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"

#define SwITCH_DBG  10

//##################################################################################################
//### FName: SwITCH_jso
//###  Desc: Creates DATA for client side Java Script and sends it (JSON)
//###        Req.Args: valid "conn->ActiveDirID" or args "fSwITCH + lSwITCH"
//###        no tist = full output!!! else:
//###        1st "ALL TIST CHECK" to detect anychange -> if not = Longpoll
//###        -> if yes output, but changed (NEWER TIST) only
//###  Para: PTR to HTTPDConnSlotData 
//###  Rets: -/-
//##################################################################################################
// Buffer-Write-Strategy defined by BIT-SET stored in conn->cgiData (initially zero):
// 00	-> StartResponseHeader
// 01   -> AddHdrFld -> content type
// 02   -> EndHeader
// 03   -> "jso Lead In"
// 04   -> "first row - no Komma" flag
// 05   -> "jso Main Data Rows"
// 06   -> "jso Lead Out"
// 30   -> "multi row - modules definitions" flag
// 31   -> "BWS assigned" flag
/*# Data Sample for one Switch.X.jso
{
 "STATUS": { 
"test": "0", 
"Switch": [
{"FUNC": "3", "NAME": "EG.Kinderzimmer.FBHHeizkreis", "VAL": "0", "MIN": "0", "MAX": "255", "DIM": "0"}
]
}
}
*/

// NOTES:
// conn->cgiData stores buffer write strategy
// conn->PCArg stores 'remote_tist'

int ICACHE_FLASH_ATTR 
ESP32_SwITCH_jso(WebIf_HTTPDConnSlotData_t* conn)
{
   // Connection aborted? Nothing to clean up.
  if (conn->conn == NULL)
	return HTTPD_CGI_DISCONNECT_CONN;

//---------------------------------------------------------------------------------------------------

  // Check if Parser is ready
  if (conn->SlotParserState != s_HTTPD_Msg_Prsd_Complete)
	return HTTPD_CGI_WAITRX_CONN;

//---------------------------------------------------------------------------------------------------
/*later fix
  // Check for valid authorization (AUTH_GENERIC_RESSOURCE)
  if (SCDED_AuthCheck(conn, AUTH_GENERIC_RESSOURCE)) {

	// set callback for "Not Authorized Error"
	conn->cgi = NoAuthErr_cgi;

	return HTTPD_CGI_REEXECUTE;
  }	
*/
//---------------------------------------------------------------------------------------------------	

  char *Args;

  // GET Method ?
  if (conn->parser_method == HTTP_GET) {

	Args = conn->getArgs;
  }
  // POST Method ?
  else { //if (conn->parser_method == HTTP_POST)

	// all post data buffered? If not continue receiving all post data to buffer ...
	if (conn->parser_content_length) return HTTPD_CGI_PROCESS_CONN;

	Args = conn->p_body_data;
  }

//--------------------------------------------------------------------------------------------------

  # if SwITCH_DBG >= 3
  if ( Args == NULL ) {

	os_printf("|.jso->NO Args!>");
  }

  else {

	os_printf("|.jso->Args:%s>",Args);
  }
  # endif

//--------------------------------------------------------------------------------------------------

  // Load Buffer-Write-Strategy
  uint32_t BWS = (uint32_t) conn->PCData;

//--------------------------------------------------------------------------------------------------

  // Fresh connection? No Buffer-Write-Strategy flag set ? Init BWS !
  if ( !BWS ) {

	# if SwITCH_DBG >= 3
	os_printf("|BWS,init>");
	# endif

	// An 'definition' is assigned? -> Output .jso for one specific 'definition'
	// (http://192.168.0.29/1/SwITCH.jso) not assigned
	// (http://192.168.0.29/1/myswitch.jso) assigned, if name matches definition name
	if (conn->p_entry_active_dir_matching_definition) {

		// definition assigned, 
		// "multi row + modules definitions" flag stays cleared,
		// single row output
	}

	// An 'definition' is NOT assigned! -> Output .jso for all 'definition's of this 'module'
	else {
		// get first 'entry definition' for this 'module'
  		Entry_Definition_t* p_entry_definition = 
			STAILQ_FIRST (&SCDERoot_at_ESP32_SwITCH_M->head_definition);

 		// loop through the 'definition's till the 'module' matches 
		while ( p_entry_definition != NULL ) {

			// this 'module' ? break, keep !
			if ( p_entry_definition->module->provided == &ESP32_SwITCH_ProvidedByModule  ) break;

			// get next 'definition' entry, to process in loop
			p_entry_definition = STAILQ_NEXT(p_entry_definition, entries);
		}

		// No 'definition' from this 'module' found ? -> Show not found error!
		if (!p_entry_definition) {

			// set callback for "Not Found Error"
			conn->cgi = NotFoundErr_cgi;

			return HTTPD_CGI_REEXECUTE;
		}

		// store as start for Buffer-Write-Strategy
		conn->p_entry_active_dir_matching_definition = p_entry_definition;

		// set "multi row - modules definitions" flag	
		BWS |= ( 1 << 30 );
	}

//--------------------------------------------------------------------------------------------------

	// May be we can do longpoll in case of no new values ?
	int32_t remote_tist = -1;

	// Check if remote sent an "tist" (timestamp), 
	if ( HTTPDFindValueDecInt32FromKey (Args, "TIST", &remote_tist) ) {

		// Find the oldest module-feature 'tist' from 'definition's (needed for longpoll check)
		int32_t oldest_feat_tist = 0;

		// get first 'entry_definition' to deal with
		Entry_Definition_t* p_entry_definition = 
			conn->p_entry_active_dir_matching_definition;

		// for multi row output...  loop all definitions ...
		do {
	os_printf("|tistfndA>");
			// check if current entry definition belongs to this 'module'
			if ( p_entry_definition->module->provided == &ESP32_SwITCH_ProvidedByModule  ) {

				// an compare 'tist'
				int32_t cmp_tist;

				// make common entry to modul specific entry
				ESP32_SwITCH_Definition_t* p_entry_esp32_switch_definition =
					(ESP32_SwITCH_Definition_t*) p_entry_definition;

				// get an relevant 'tist' from this 'definition'
				cmp_tist = p_entry_esp32_switch_definition->RB01_X_TiSt;

				// update 'oldest_feat_tist'
				if ( oldest_feat_tist < cmp_tist ) oldest_feat_tist = cmp_tist;

				// get an relevant 'tist' from this 'definition'
				cmp_tist = p_entry_esp32_switch_definition->IB01_X_TiSt;

				// update 'oldest_feat_tist'
				if (oldest_feat_tist < cmp_tist) oldest_feat_tist = cmp_tist;
			}

			// get next definition entry to process it
			p_entry_definition = STAILQ_NEXT (p_entry_definition, entries);

  		// loop if pending 'definition's an "multi row - modules definitions" flag is set
		} while ( p_entry_definition && ( BWS & 1 << 30 ) );

		// now check if remote data is complete up to date, if yes do longpoll ...
		// ... else renew specific or all rows
		if ( oldest_feat_tist <= remote_tist ) {

			#if SwITCH_DBG >= 4
			os_printf("|Rmt.TIST:%d, Feat.TIST:%d, Real TIST: %d, LP!->%d Sec>",
				remote_tist,
				oldest_feat_tist,
				(int)SCDEFn_at_ESP32_SwITCH_M->GetTiStFn(),
				conn->LP10HzCounter);
			#endif

			// do longpoll, limit time (20 sec)
			if ( conn->LP10HzCounter < 20 * 10 ) {

				conn->LP10HzCounter++;

				return HTTPD_CGI_PROCESS_CONN;
			}
		}
	}

	// use conn->PCArg for 'remote_tist' storage
	conn->PCArg = (void *) remote_tist;

	// make Buffer-Write-Strategy (BWS) complete, to build correct answer
	BWS |= 0b10000000000000000000000001111111;
  }

//---------------------------------------------------------------------------------------------------

  // create response step by step, according to defined buffer-write-strategy
  int len;
  char buff[256];

//---------------------------------------------------------------------------------------------------

  // do StartResponseHeader ?
  if ( BWS & 1 << 0 ) {

	BWS &= ~( 1 << 0 );

	//if (!HTTPDStartRespHeader(conn, 200)) return HTTPD_CGI_PROCESS_CONN;
	if ( !SCDED_StartRespHeader2 (conn) ) return HTTPD_CGI_PROCESS_CONN;
  }

//---------------------------------------------------------------------------------------------------

  // do AddHdrFld ?
  if (BWS & 1 << 1 ) {

	BWS &= ~( 1 << 1 );

	if ( !SCDED_AddHdrFld (conn, "Content-Type", "text/json", -1) ) return HTTPD_CGI_PROCESS_CONN;
  }

//---------------------------------------------------------------------------------------------------

  // do EndHeader ?
  if ( BWS & 1 << 2 ) {

	BWS &= ~( 1 << 2 );

	if ( !SCDED_EndHeader (conn) ) return HTTPD_CGI_PROCESS_CONN;
  }

//---------------------------------------------------------------------------------------------------

  // do "jso Lead In" ?
  if ( BWS & 1 << 3 ) {

	BWS &= ~( 1 << 3 );

	len = sprintf (buff, "{\"STATUS\":{\"MEMORY\":\"%d\",\"TIME\":\"%u\",\n\"SwITCH\":[\n",
		0,//system_get_free_heap_size (),
		(int)SCDEFn_at_ESP32_SwITCH_M->GetTiStFn() );

	if ( !HTTPD_Send_To_Send_Buffer (conn, buff, len) ) return HTTPD_CGI_PROCESS_CONN;
  }

//---------------------------------------------------------------------------------------------------

  // do "jso Main Data Rows" ?
   if ( BWS & 1 << 5 ) {

	// conn->PCArg stores 'remote_tist'
	int32_t remote_tist =  (uint32_t) conn->PCArg;

	// get current 'entry_definition' to deal with
	Entry_Definition_t* p_entry_definition = 
		conn->p_entry_active_dir_matching_definition;

	// multi row output?  loop all definitions ...
 	 do {
		// check if current entry definition belongs to this 'module'
		if ( p_entry_definition->module->provided == &ESP32_SwITCH_ProvidedByModule ) {

			// store progress - because 'send_buffer' full may interrupt
			conn->p_entry_active_dir_matching_definition = p_entry_definition;

			// make common entry to modul specific entry
			ESP32_SwITCH_Definition_t* p_entry_esp32_switch_definition =
			(ESP32_SwITCH_Definition_t*) p_entry_definition;

			// 'tist'-check, output row if older than remote_tist, or no 'tist' ...
			if ( remote_tist == -1 ||
				remote_tist < p_entry_esp32_switch_definition->RB01_X_TiSt ||
				remote_tist < p_entry_esp32_switch_definition->IB01_X_TiSt) {

				// 'tist'-check result: Output this ROW
				len = sprintf(buff, 
					"%s{\"Name\":\"%.*s\",\"TIST\":\"%d\",\"VAL\":\"%u\",\"MIN\":\"%u\","
					"\"MAX\":\"%u\",\"DIM\":\"%u\"}",
					( BWS & 1 << 4 ) ? "" : ",\n",  // "first row - no Komma" flag
					p_entry_esp32_switch_definition->common.nameLen,
					p_entry_esp32_switch_definition->common.name,
					(int) p_entry_esp32_switch_definition->RB01_X_TiSt,
					(uint) p_entry_esp32_switch_definition->RB01_Val,
					(uint) p_entry_esp32_switch_definition->IB01_SwValMin,
					(uint) p_entry_esp32_switch_definition->IB01_SwValMax,
					(100 * (p_entry_esp32_switch_definition->RB01_Val -
						p_entry_esp32_switch_definition->IB01_SwValMin) / 
					(p_entry_esp32_switch_definition->IB01_SwValMax - 
						p_entry_esp32_switch_definition->IB01_SwValMin+10) ) );

				// delete "first row - no Komma" flag
				BWS &= ~( 1 << 4 );

				// send row
				if ( !HTTPD_Send_To_Send_Buffer (conn, buff, len) ) return HTTPD_CGI_PROCESS_CONN;
			}
		}

		// get next definition entry to process it
		p_entry_definition = STAILQ_NEXT (p_entry_definition, entries);

	// loop if pending entry_definition an of course an "multi row - modules definitions" flag
	} while (p_entry_definition && ( BWS & 1 << 30 ) );

	BWS &= ~( 1 << 5 );
  }

//--------------------------------------------------------------------------------------------------

  // do "jso Lead Out" ?
  if ( BWS & 1 << 6 ) {

	BWS &= ~( 1 << 6 );

	len = sprintf (buff, "]\n}\n}");

	if ( !HTTPD_Send_To_Send_Buffer (conn, buff, len) ) return HTTPD_CGI_PROCESS_CONN;
  }

//--------------------------------------------------------------------------------------------------

  return HTTPD_CGI_DISCONNECT_CONN;
}









/* THIS IS MASTER CODE 02.08.2016!
 *--------------------------------------------------------------------------------------------------
 *FName: SwITCH_jso
 * Desc: Creates DATA for client side Java Script and sends it (JSON)
 * Info: Req.Args: valid args "fSwITCH + lSwITCH"; "TIST" optional
 *       no TIST => full output!!! else:
 *       - does "ALL TIST CHECK" to detect anychange -> if no change = Longpoll
 *       - if change =  output, but changed (NEWER TIST) only
 * Para: HTTPDConnSlotData *conn -> ptr to the connection slot
 *       stores Buffer Write Strategy in conn->PCData (-> MUST BE ZERO WHEN INITIALLY CALLED)
 *       stores remote TIST in conn->PCArg
 * Rets: -/-
 *--------------------------------------------------------------------------------------------------
 */
/*
// SwITCH Buffer Write Strategy
typedef struct SwITCH_BWS_s // 32Bit
  {
  bool DoHTTPStartResp : 1;   
  bool DoHTTPHeader : 1;   
  bool DoHTTPEndHeader : 1;   
  bool DoJSOLeadInData : 1;   
  bool FlagFirstRowNoComma : 1;   
  bool free : 1;   
  unsigned int JSOSwITCHProcCnt : 12;   
  unsigned int JSOLastSwITCHPos : 12;   
  bool DoJSOLeadOutData : 1;   
  bool BWSAssigned : 1;   
} SwITCH_BWS_t;
*/
/*# Data Sample for one Switch.X.jso
{
 "STATUS": { 
"test": "0", 
"Switch": [
{"FUNC": "3", "NAME": "EG.Kinderzimmer.FBHHeizkreis", "VAL": "0", "MIN": "0", "MAX": "255", "DIM": "0"}
]
}
}
*/
/*
int ICACHE_FLASH_ATTR 
SwITCH_jso(HTTPDConnSlotData *conn) 
{
   // Connection aborted? Nothing to clean up.
  if (conn->conn == NULL)
	return HTTPD_CGI_DISCONNECT_CONN;

//---------------------------------------------------------------------------------------------------

  // Check if Parser is ready
  if (conn->SlotParserState != s_HTTPD_Msg_Prsd_Complete)
	return HTTPD_CGI_WAITRX_CONN;

//---------------------------------------------------------------------------------------------------

  // Check for valid authorization (AUTH_GENERIC_RESSOURCE)
  if (HTTPDAuthCheck(conn, AUTH_GENERIC_RESSOURCE))
	{
	// set callback for "Not Authorized Error"
	conn->cgi = NoAuthErr_cgi;
	return HTTPD_CGI_REEXECUTE;
	}	

//---------------------------------------------------------------------------------------------------	

  char *Args;
  if (conn->parser_method == HTTP_GET)		// GET Method ?
	{
	Args = conn->getArgs;
	}
  else //if (conn->parser_method == HTTP_POST)	// POST Method ?
	{
	// all post data buffered? If not continue receiving all post data to buffer ...
	if (conn->parser_content_length) return HTTPD_CGI_PROCESS_CONN;

	Args = conn->body_data;
	}

//--------------------------------------------------------------------------------------------------

  # if SwITCH_DBG >= 3
  if (Args == NULL)
	{
	os_printf(".jso->NO Args!");
	}
  else
	{
	os_printf(".jso->Args:%s\n",Args);
	}
  # endif

//--------------------------------------------------------------------------------------------------

  // get Buffer-Write-Strategy from storage location
  SwITCH_BWS_t SwITCH_BWS = (SwITCH_BWS_t) conn->PCData;

  // fresh connection without Buffer-Write-Strategy ?
  if (!SwITCH_BWS->BWSAssigned)
	{

	# if SwITCH_DBG >= 3
	os_printf("BWS,Start\n");
	# endif

	int32 fSwITCH;
	int32 lSwITCH;
	// get info about first and last feature to output from ADID (http://192.168.0.29/1/S0.jso)
	if (conn->ActiveDirID > 0)
		{
		fSwITCH = conn->ActiveDirID;
		lSwITCH = fSwITCH;
		}
	// get info about first and last feature to output from query (http://192.168.0.29/S0.jso?fS0=1&lS0=5)
	else if (Args!= NULL)
		{
		if ( (!HTTPDFindValueDecInt32FromKey(Args,"fSwITCH", &fSwITCH)) ||
			(!HTTPDFindValueDecInt32FromKey(Args,"lSwITCH", &lSwITCH)) )
			{
			// set callback for "Not Found Error"
			conn->cgi = NotFoundErr_cgi;
			return HTTPD_CGI_REEXECUTE;
			}
		}
	// No Args?
	else
		{
		// set callback for "Not Found Error"
		conn->cgi = NotFoundErr_cgi;
		return HTTPD_CGI_REEXECUTE;
		}

	// Limits check ...
	if (fSwITCH<1||lSwITCH>NUMBER_SWITCH_FEAT||(lSwITCH-fSwITCH)<0)
		{
		// set callback for "Not Found Error"
		conn->cgi = NotFoundErr_cgi;
		return HTTPD_CGI_REEXECUTE;
		}

	// Check if remote sent "TIST" timestamp, so that we can do longpoll in case of no new values
	int32 RmtTIST = -1;
	if (HTTPDFindValueDecInt32FromKey(Args,
		"TIST", &RmtTIST))
		{
		// find oldest Timestamp from Features to check for longpoll
		int32 FeatTIST = 0;
		int32 nowSwITCH = fSwITCH;
		while (nowSwITCH <= lSwITCH)
			{
			int32 cmpTIST;
			cmpTIST = SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[nowSwITCH-1].RB01_X_TiSt;
			if (FeatTIST < cmpTIST)
				FeatTIST = cmpTIST;

			cmpTIST = SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[nowSwITCH-1].IB01_X_TiSt;
			if (FeatTIST < cmpTIST)
				FeatTIST = cmpTIST;
			nowSwITCH++;
			}

		// check if remote data is complete up to date, if yes do longpoll ... else renew specific or all rows
		if (FeatTIST <= RmtTIST)
			{
			#if SwITCH_DBG >= 4
			os_printf("Rmt.TIST:%d, Feat.TIST:%d, Real TIST: %d, LP!->%d Sec\n",
				RmtTIST,
				FeatTIST,
				GetTIST(),
				conn->LP10HzCounter);
			#endif

			// do longpoll, limit time (30 sec)
			if (conn->LP10HzCounter < 30*10)
				{
				conn->LP10HzCounter++;

				return HTTPD_CGI_PROCESS_CONN;
				}
			}
		}
	// setup Buffer-Write-Strategy (BWS) for answer
	*SwITCH_BWS.DoHTTPStartResp = TRUE;   
	*SwITCH_BWS.DoHTTPHeader = TRUE;   
	*SwITCH_BWS.DoHTTPEndHeader = TRUE;   
	*SwITCH_BWS.DoJSOLeadInData = TRUE;   
	*SwITCH_BWS.FlagFirstRowNoComma = TRUE;   
	*SwITCH_BWS.free = FALSE;   
	*SwITCH_BWS.JSOSwITCHProcCnt = fSwITCH;   
	*SwITCH_BWS.JSOLastSwITCHPos = lSwITCH;   
	*SwITCH_BWS.DoJSOLeadOutData = TRUE;   
	*SwITCH_BWS.BWSAssigned = TRUE;
	}

// Create Answer according to Buffer-Write-Strategy to save ram-memory
  int len;
  char buff[256];

//---------------------------------------------------------------------------------------------------

  // do HTTPStartResponse ?
  if (*SwITCH_BWS.DoHTTPStartResp)
	{
	*SwITCH_BWS.DoHTTPStartResp = FALSE;

	//if (!HTTPDStartRespHeader(conn, 200)) return HTTPD_CGI_PROCESS_CONN;

	if (!HTTPD_StartRespHeaderNEU(conn)) return HTTPD_CGI_PROCESS_CONN;
	}

//---------------------------------------------------------------------------------------------------

  // do HTTPHeader ?
  if (*SwITCH_BWS.DoHTTPHeader)
	{
	*SwITCH_BWS.DoHTTPHeader = FALSE;

	if (!HTTPDAddHdrFld(conn, "Content-Type", "text/json")) return HTTPD_CGI_PROCESS_CONN;
	}

//---------------------------------------------------------------------------------------------------

  // do HTTPEndHeader ?
  if (*SwITCH_BWS.DoHTTPEndHeader)
	{
	*SwITCH_BWS.DoHTTPEndHeader = FALSE;

	if (!HTTPDEndHeader(conn)) return HTTPD_CGI_PROCESS_CONN;
	}

//---------------------------------------------------------------------------------------------------

  // do "jso Lead In" Data ?
  if (*SwITCH_BWS.DoJSOLeadInData)
	{
	*SwITCH_BWS.DoJSOLeadInData = FALSE;

	len = os_sprintf(buff, "{\"STATUS\":{\"MEMORY\":\"%d\",\"TIME\":\"%u\",\n\"SwITCH\":[\n",
		system_get_free_heap_size(),
		GetTIST());

	if (!HTTPDSend(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;
	}

//---------------------------------------------------------------------------------------------------

  // do "jso Main Data Rows" Data ?



	int32 RmtTIST;
	if (HTTPDFindValueDecInt32FromKey(conn->body_data,
		"TIST", &RmtTIST))
  os_printf("tmp Rmt.TIST:%d\n",RmtTIST);
	else RmtTIST = -1;

// ziel ist das speichern des tist im argument!!!!!!!!!!
//  int32 RmtTIST = (int) conn->PCArg;




  int lSwITCH = *SwITCH_BWS.JSOLastSwITCHPos;
  int nowSwITCH = *SwITCH_BWS.JSOSwITCHProcCnt;
  while (nowSwITCH <= lSwITCH)
	{
	// store "jso LiGHTS Processing Count"
//	*BWS = (*BWS & 0b11111111111111000000000000111111)|(((nowSwITCH+1)<<6));	// store pos+1
	*SwITCH_BWS.JSOSwITCHProcCnt++;
	int nowSwITCHme = nowSwITCH-1;

	// DO TIST Output Check, output row if older, or no TIST...
	if (RmtTIST == -1 ||
		RmtTIST < SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[nowSwITCHme].RB01_X_TiSt ||
		RmtTIST < SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[nowSwITCHme].IB01_X_TiSt)
		// TIST-Check result: Output this ROW
		{
		len = os_sprintf(buff, "%s{\"NO\":\"%d\",\"NAME\":\"%s\",\"TIST\":\"%d\",\"VAL\":\"%u\",\"MIN\":\"%u\",\"MAX\":\"%u\",\"DIM\":\"%u\"}",
			(*SwITCH_BWS.FlagFirstRowNoComma)?"":",\n",
			nowSwITCH,
			SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[nowSwITCHme].IB01_Name,
			(int) SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[nowSwITCHme].RB01_X_TiSt,
			(uint) SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[nowSwITCHme].RB01_Val,
			(uint) SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[nowSwITCHme].IB01_SwValMin,
			(uint) SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[nowSwITCHme].IB01_SwValMax,
			(100 * (SysCfgRamRtcMirrored->MySwITCH_FeatCfgRamRtcMirrored[nowSwITCHme].RB01_Val -
				SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[nowSwITCHme].IB01_SwValMin) / 
			(SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[nowSwITCHme].IB01_SwValMax - 
				SysCfgRamFlashMirrored->MySwITCH_FeatCfgRamFlashMirrored[nowSwITCHme].IB01_SwValMin) ) );

		// delete "first row - no Komma" flag
		*SwITCH_BWS.FlagFirstRowNoComma = FALSE;

		// send row
		if (!HTTPDSend(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;
		}

	// next row
	nowSwITCH++;
	}

//--------------------------------------------------------------------------------------------------

  // do "jso Lead Out" Data ?
  if (*SwITCH_BWS.DoJSOLeadOutData)
	{
	*SwITCH_BWS.DoJSOLeadOutData = FALSE;

	len = os_sprintf(buff, "]\n}\n}");

	if (!HTTPDSend(conn, buff, len)) return HTTPD_CGI_PROCESS_CONN;
	}

//--------------------------------------------------------------------------------------------------

  return HTTPD_CGI_DISCONNECT_CONN;
}
*/

 

