//#################################################################################################
//###  Function: CGI Code for SwITCH Feature - Generic for SCD-Engine 2
//###  ESP 8266 / 32 / IoT SOC Activities ...
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
#include "ESP32_SwITCH_cgi.h"

/*
 *--------------------------------------------------------------------------------------------------
 *FName: SwITCH_cgi
 * Desc: Remote Procedure (RP). Should be assinged to resources. Is used to process incoming requests
 *       to HTTP & HATP Server. POST ; Scheme HTTP ; Path opt. with UID ; Mime .cgi 
 *       Used Header-Fields:
 *       An Finite-State-Machine processes requests and responses with code and message.
 * Info: conn->getArgs -> Query String
 *       conn->body_data -> reserved for internals and attribute exchange
 * Para: HTTPDConnSlotData *conn -> ptr to connection slot
 * Rets: int -> cgi result cmd
 *--------------------------------------------------------------------------------------------------
 */

// For: SwITCH_cgi RPC, Finite State Machine processing State - Stage 2 
enum SwITCH_cgi_state
  {					// #XX for debugging
    S2S_cgi_Check_Stage1_Parser_State = 0// #00 check if parser of stage 1 is in a state that stage 2 can continue work
  , S2S_cgi_Action_S1_Parse_Body_Data	// #01 detected that stage 1 has not parsed the complete message. Processing body data
  , S2S_cgi_Action_S1_Msg_Prsd_Complete	// #02 detected that stage 1 has parsed the complete message including body data
  , S2S_cgi_Check_RPC_Authorization	// #03 Check if request contains the required authorization
  , S2S_cgi_Check_and_Get_ADID		// #04 Check and get the Active-Directory-ID
  , S2S_cgi_Parse_Query_String		// #05 Parse the Query String

  // done - start writing response to TX buff.
  // No Error messages after this State !		 	   
  , S2S_cgi_TX_Response_with_Code	// #06 add response to the TX (response) buffer. TX + Disconn if error code
  , S2S_cgi_TX_Add_HDR_Flds		// #07 add header fields (if any) to the TX (response) buffer
  , S2S_cgi_TX_Add_Body_Data		// #08 add body data (if any) to the TX (response) buffer
  , S2S_cgi_DONE_TX_Response_Buff	// #09 done, incl. response, now TX response buffer and then disconnect
  };

int
ESP32_SwITCH_cgi(WebIf_HTTPDConnSlotData_t* conn)
{
  // Connection aborted? Nothing to clean up.
  if ( conn->conn == NULL ) 
	return HTTPD_CGI_DISCONNECT_CONN;

//---------------------------------------------------------------------------------------------------

  # if ESP32_SwITCH_Module_DBG >= 4
 // printf("|SwITCH_cgi>");
  # endif

  #if ESP32_SwITCH_Module_DBG >= 5
  SCDEFn_at_ESP32_SwITCH_M->Log3Fn(conn->p_entry_active_dir_matching_definition->name,
	conn->p_entry_active_dir_matching_definition->nameLen,
	5,
	"SwITCH_cgi Fn of Module '%.*s' is called.",
	conn->p_entry_active_dir_matching_definition->module->provided->typeNameLen,
	conn->p_entry_active_dir_matching_definition->module->provided->typeName);
  #endif

//---------------------------------------------------------------------------------------------------

  // init variables used in finite-state-machine

  // for extracted Active Directory ID
//  int16_t ADID = 0; // 0 blodsinn?

//neu
  strTextMultiple_t* respMsgAsBodyData = NULL; // no msg

  // remember the readings affected by the parsing process
//  uint32 AffectedReadings = 0;

  // pointer to identified arguments (Query)
  char* Args = NULL; //null blödsinn?

  // Response argument buffer & write position
  char RespArgsBuf[512];
  char* RespArgsWPos = RespArgsBuf;

  // Header Fields buffer & write position
//  char HdrFldsBuf[512];
  char* HdrFldsWPos = NULL;

  // Body-Data buffer & write position
//  char BdyDataBuf[512];
  char* BdyDataWPos = NULL;

  // store errors ...
//  int stage2_scde_errno = (enum http_errno) 0; //enum?

  int S2_Statex = 0; //test

  // load Stage 2 processing state (initial is 0)
 // enum state S2_State = (enum state) conn->S2_State;
  enum SwITCH_cgi_state S2_State = (enum SwITCH_cgi_state) S2_Statex;

  # if ESP32_SwITCH_Module_DBG >= 4
  printf("|FST-Start>");
  # endif

  reexecute:

  // Debug output, Stage 2, FSM State,
  # if ESP32_SwITCH_Module_DBG >= 4
  printf("|S2.%d>",
	S2_State);
  # endif

  switch (S2_State) {

//--------------------------------------------------------------------------------------------------

	// #00 check if parser of stage 1 is in a state that stage 2 can continue work

	case S2S_cgi_Check_Stage1_Parser_State:

	if (conn->SlotParserState != s_HTTPD_Msg_Prsd_Complete) {

		return HTTPD_CGI_WAITRX_CONN;
	}

//	else
//		{
//		// Header parsed, now parsing BodyData - update state
//		S2_State = Step_Msg_Prs_Body_Data;
//		}

	else {

		// Message parsed complete - update state
		S2_State = S2S_cgi_Action_S1_Msg_Prsd_Complete;
	}

	break;

//--------------------------------------------------------------------------------------------------

	case S2S_cgi_Action_S1_Parse_Body_Data: // #01

	break;

//--------------------------------------------------------------------------------------------------

	// #02 set Arguments according to method, check if arguments

	case S2S_cgi_Action_S1_Msg_Prsd_Complete:

	// GET Method ?
	if (conn->parser_method == HTTP_GET) {

		Args = conn->getArgs;

	}

	// POST Method ?
	else if (conn->parser_method == HTTP_POST) {

		Args = conn->p_body_data;

	}

	// do we have arguments ? -> Error & break if not
	if (!Args) {

		// no arguments error- finnish
		conn->parser_http_errno = HPE_INVALID_URL;

		// goto error;
		S2_State = S2S_cgi_TX_Response_with_Code;
	}

	// else go to next step
	else {

		#if ESP32_SwITCH_Module_DBG >= 4
		printf("Args:%s>",
			Args);
  		#endif

		// we have args - go to next step
		S2_State = S2S_cgi_Check_RPC_Authorization;
	}

	break;

//--------------------------------------------------------------------------------------------------

	// #03 Check for valid authorization (AUTH_GENERIC_RESSOURCE)

	case S2S_cgi_Check_RPC_Authorization:

/* skip auth temp..
	if (SCDED_AuthCheck(conn, AUTH_GENERIC_RESSOURCE)) {

		// not authorized - finnish with error
		conn->parser_http_errno = HPE_NO_AUTH;

		// goto error;
		S2_State = S2S_cgi_TX_Response_with_Code;

	}

	else {

		// authorized - go to next step
		S2_State = S2S_cgi_Parse_Query_String; // SCHRITT übersprungen !!!S2S_cgi_Check_and_Get_ADID;
	}
*/S2_State = S2S_cgi_Parse_Query_String;
	break;

//--------------------------------------------------------------------------------------------------
/*
	// #04 Check & Get Active Directory ID (ADID)

	case S2S_cgi_Check_and_Get_ADID:
//ist immer valid!!!
	// Get index value from conn->ActiveDirID
	ADID  = conn->ActiveDirID -1;

	// resource available check ...
	if ( ADID >= SysCfgRamFlashMirrored->MySwITCH_FeatHWCfg.NumFeat)

		{

		// Active Directory ID (ADID) out of range error- finnish with error
		conn->parser_http_errno = HPE_ADID_AOR;
		//goto error;
		S2_State = S2S_cgi_TX_Response_with_Code;

		}

	else {

		#if ESP32_SwITCH_Module_DBG >= 4
		printf("ADID:%d>",
			ADID);
  		#endif

		// Got valid ADID - go to next step
		S2_State = S2S_cgi_Parse_Query_String;
	}

	break;
*/
//--------------------------------------------------------------------------------------------------

	// #05 Parse the Query String

	case S2S_cgi_Parse_Query_String:
	{

	// conversion .. uriencode missing!!!!!!!!1
	uint8_t *setArgs = (uint8_t*) Args;
	size_t setArgsLen = strlen(Args);

// ------------------------------------------------------------------------------------------------

	respMsgAsBodyData = ESP32_SwITCH_Set(conn->p_entry_active_dir_matching_definition
		,setArgs
                ,setArgsLen);

	// assuming processed OK
	conn->parser_http_errno = 0;


	// parsed query string  - go to next step
	S2_State = S2S_cgi_TX_Response_with_Code;

	}

	break;

//--------------------------------------------------------------------------------------------------

	// #06 add response to the TX (response) buffer. TX + Disconn if error code

	case S2S_cgi_TX_Response_with_Code:

	// start resonse header with code and desc.
	SCDED_StartRespHeader2(conn);

	// >0 -> error, something went wrong, disconn
	if (conn->parser_http_errno) {

		// response added to TX buf, code ERROR - NO header fields, NO body data !!!
		return HTTPD_CGI_DISCONNECT_CONN;
	}

	// response added to TX buf, code OK - go to next step
	S2_State = S2S_cgi_TX_Add_HDR_Flds;

	break;

//--------------------------------------------------------------------------------------------------

	// #07 add header fields (if any) to the TX (response) buffer

	case S2S_cgi_TX_Add_HDR_Flds:

	if (RespArgsWPos != RespArgsBuf) {

		SCDED_AddHdrFld(conn, "RespMsg", (const char *) RespArgsBuf, -1);
	}

	// DO NOT FORGET TO ADD CONTENT-LENGHT IF BODY DATA IS PRESENT !!!

	// End header by adding '\r\n'
	SCDED_EndHeader(conn);

	// header fields added - go to next step
	S2_State = S2S_cgi_TX_Add_Body_Data;

	break;

//--------------------------------------------------------------------------------------------------

	// #08 add body data (if any) to the TX (response) buffer

	case S2S_cgi_TX_Add_Body_Data:

	if (respMsgAsBodyData) {

		HTTPD_Send_To_Send_Buffer(conn
			,respMsgAsBodyData->strText
			,respMsgAsBodyData->strTextLen);

		if (respMsgAsBodyData->strText) free(respMsgAsBodyData->strText);
		free(respMsgAsBodyData);
	}

	// body data added - go to next step
	S2_State = S2S_cgi_DONE_TX_Response_Buff;

	break;

//--------------------------------------------------------------------------------------------------

	// #09 done, incl. response, now TX response buffer and then disconnect
	case S2S_cgi_DONE_TX_Response_Buff:

	# if ESP32_SwITCH_Module_DBG >= 4
	printf("|Done!>");
	# endif

	return HTTPD_CGI_DISCONNECT_CONN;

	break;

//--------------------------------------------------------------------------------------------------

	default:
	
	// set error number
	conn->parser_http_errno = HPE_UNHANDLED_STATE;

	// error number set - go to next step (TX response with code and disconnect)
	S2_State = S2S_cgi_TX_Response_with_Code;

	break;

//--------------------------------------------------------------------------------------------------

  }

  goto reexecute;

  return HTTPD_CGI_DISCONNECT_CONN;
}



