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
#include "ESP32_SwITCH_WEBIF_get_cb.h"

#include "CGI_NoAuthErr.h"
#include "CGI_NotFoundErr.h"

/*
 *--------------------------------------------------------------------------------------------------
 *FName: ESP32_SwITCH_WEBIF_get_cb
 * Desc: For ESP32_SwITCH - the WEBIF_get resource Remote Procedure (RP).
 *       Should be assinged to URI resources. Is used to process incoming get, ... requests
 *       to HTTP & HATP Server. POST ; Scheme HTTP ; Path opt. with UID ; Mime .get 
 *       Used Header-Fields:
 *       An Finite-State-Machine processes requests and responses with code and message.
 * Info: conn->getArgs -> Query String
 *       conn->body_data -> reserved for internals and attribute exchange
 * Para: HTTPDConnSlotData *conn -> ptr to connection slot
 * Rets: int -> get result cmd
 *--------------------------------------------------------------------------------------------------
 */

// for: SwITCH_get RPC, Finite State Machine processing State - Stage 2 (after parsing finished)
enum SwITCH_get_state
  {					// #XX for debugging
    S2S_get_Check_Stage1_Parser_State = 0// #00 check if parser of stage 1 is in a state that stage 2 can continue work
  , S2S_get_Action_S1_Parse_Body_Data	// #01 detected that stage 1 has not parsed the complete message. Processing body data
  , S2S_get_Action_S1_Msg_Prsd_Complete	// #02 detected that stage 1 has parsed the complete message including body data
  , S2S_get_Check_RPC_Authorization	// #03 Check if request contains the required authorization
  , S2S_get_Check_and_Get_ADID		// #04 Check and get the Active-Directory-ID
  , S2S_get_Parse_Query_String		// #05 Parse the Query String

  // done - start writing response to TX buff.
  // No Error messages after this State !		 	   
  , S2S_get_TX_Response_with_Code	// #06 add response to the TX (response) buffer. TX + Disconn if error code
  , S2S_get_TX_Add_HDR_Flds		// #07 add header fields (if any) to the TX (response) buffer
  , S2S_get_TX_Add_Body_Data		// #08 add body data (if any) to the TX (response) buffer
  , S2S_get_DONE_TX_Response_Buff	// #09 done, incl. response, now TX response buffer and then disconnect
  };

int
ESP32_SwITCH_WEBIF_get_cb(WebIf_HTTPDConnSlotData_t* conn)
{
  // connection aborted? nothing to clean up.
  if ( conn->conn == NULL ) 
	return HTTPD_CGI_DISCONNECT_CONN;

//---------------------------------------------------------------------------------------------------

  # if ESP32_SwITCH_Module_DBG >= 4
  printf("|SwITCH_WEBIF_get>");
  # endif

/*
  #if ESP32_SwITCH_Module_DBG >= 5
  SCDEFn_at_ESP32_SwITCH_M->Log3Fn(conn->p_entry_active_dir_matching_definition->name,
	conn->p_entry_active_dir_matching_definition->nameLen,
	5,
	"SwITCH_get Fn of Module '%.*s' is called.",
	conn->p_entry_active_dir_matching_definition->module->provided->typeNameLen,
	conn->p_entry_active_dir_matching_definition->module->provided->typeName);
  #endif
*/

//---------------------------------------------------------------------------------------------------

  // init variables (that are only living this call)

  // pointer used to point to identified arguments (Query)
  char* Args = NULL; // null requred to prevent errror msg

  // the response message -> goes to body data
  Entry_String_t* respMsg = NULL;

  // the response header fields
//  strTextMultiple_t* respHdrFlds = NULL;



  // for extracted Active Directory ID
//  int16_t ADID = 0; // 0 blodsinn?

  // remember the readings affected by the parsing process
//  uint32 AffectedReadings = 0;



  // Response argument buffer & write position
//  char RespArgsBuf[512];
//  char* RespArgsWPos = RespArgsBuf;

  // Header Fields buffer & write position
//  char HdrFldsBuf[512];
//  char* HdrFldsWPos = NULL;

  // Body-Data buffer & write position
//  char BdyDataBuf[512];
//  char* BdyDataWPos = NULL;

  // store errors ...
//  int stage2_SwITCH_errno = (enum http_errno) 0; //enum?

//--------------------------------------------------------------------------------------------------

  // stage 2 processing state
//  int S2_Statex;

  // enum state S2_State = (enum state) conn->S2_State;
  enum SwITCH_get_state S2_State = (enum SwITCH_get_state) S2S_get_Check_Stage1_Parser_State;

  // set initial stage 2 processing state (= 0)
 // S2_Statex = S2S_get_Check_Stage1_Parser_State;

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

	case S2S_get_Check_Stage1_Parser_State:

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
		S2_State = S2S_get_Action_S1_Msg_Prsd_Complete;
	}

	break;

//--------------------------------------------------------------------------------------------------

	case S2S_get_Action_S1_Parse_Body_Data: // #01

	break;

//--------------------------------------------------------------------------------------------------

	// #02 set Arguments according to method, check if arguments

	case S2S_get_Action_S1_Msg_Prsd_Complete:

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
		S2_State = S2S_get_TX_Response_with_Code;
	}

	// else go to next step
	else {

		#if ESP32_SwITCH_Module_DBG >= 4
		printf("Args:%s>",
			Args);
  		#endif

		// we have args - go to next step
		S2_State = S2S_get_Check_RPC_Authorization;
	}

	break;

//--------------------------------------------------------------------------------------------------

	// #03 Check for valid authorization (AUTH_GENERIC_RESSOURCE)

	case S2S_get_Check_RPC_Authorization:

/* skip auth temp..
	if (SCDED_AuthCheck(conn, AUTH_GENERIC_RESSOURCE)) {

		// not authorized - finnish with error
		conn->parser_http_errno = HPE_NO_AUTH;

		// goto error;
		S2_State = S2S_get_TX_Response_with_Code;

	}

	else {

		// authorized - go to next step
		S2_State = S2S_get_Parse_Query_String; // SCHRITT übersprungen !!!S2S_get_Check_and_Get_ADID;
	}
*/S2_State = S2S_get_Parse_Query_String;
	break;

//--------------------------------------------------------------------------------------------------
/*
	// #04 Check & Get Active Directory ID (ADID)

	case S2S_get_Check_and_Get_ADID:
//ist immer valid!!!
	// Get index value from conn->ActiveDirID
	ADID  = conn->ActiveDirID -1;

	// resource available check ...
	if ( ADID >= SysCfgRamFlashMirrored->MySwITCH_FeatHWCfg.NumFeat)

		{

		// Active Directory ID (ADID) out of range error- finnish with error
		conn->parser_http_errno = HPE_ADID_AOR;
		//goto error;
		S2_State = S2S_get_TX_Response_with_Code;

		}

	else {

		#if ESP32_SwITCH_Module_DBG >= 4
		printf("ADID:%d>",
			ADID);
  		#endif

		// Got valid ADID - go to next step
		S2_State = S2S_get_Parse_Query_String;
	}

	break;
*/
//--------------------------------------------------------------------------------------------------

	// #05 Parse the Query String

	case S2S_get_Parse_Query_String:
	{























//--------------------------------------------------------------------------------------------------

  // Load Buffer-Write-Strategy
  uint32_t BWS = (uint32_t) conn->PCData;

//--------------------------------------------------------------------------------------------------

  // Fresh connection? No Buffer-Write-Strategy flag set ? Init BWS !
  if ( !BWS ) {

	# if ESP32_SwITCH_Module_DBG >= 3
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





//	// use conn->PCArg for 'remote_tist' storage
//	conn->PCArg = (void *) remote_tist;

	// make Buffer-Write-Strategy (BWS) complete, to build correct answer
	BWS |= 0b10000000000000000000000001111111;
  }


//--------------------------------------------------------------------------------------------------

  // create response step by step, according to defined buffer-write-strategy
  int len;
  char buff[256];

//---------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------

  // do "jso Main Data Rows" ?
   if ( BWS & 1 << 5 ) {

//	// conn->PCArg stores 'remote_tist'
//	int32_t remote_tist =  (uint32_t) conn->PCArg;

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

			char **argv;
        		int argc;

       			// split the query to args
        		argv = SCDEFn_at_ESP32_SwITCH_M->ArgParse_SplitURLEncodedArgsToAllocatedMemFn(&argc, Args);

        		respMsg = ESP32_SwITCH_GetV2(p_entry_esp32_switch_definition
              			,argv, argc);

        		// ArgParse Split_ToArgsFn has allocated mem. Free it.
        		SCDEFn_at_ESP32_SwITCH_M->ArgParse_FreeSplittedArgsInAllocatedMemFn(argv);

			// needs an update
			if (respMsg) {




	char* p_leadIn;
	int len;





//---------------------------------------------------------------------------------------------------

  // do StartResponseHeader ?
  if ( BWS & 1 << 0 ) {

	BWS &= ~( 1 << 0 );

	SCDED_StartRespHeader2(conn);
        SCDED_AddHdrFld(conn, "Content-Type", "text/json", -1);
        SCDED_EndHeader(conn);


        len = asprintf(&p_leadIn,
		"{\"STATUS\":{\"MEMORY\":\"%d\",\"TIME\":\"%u\",\n\"SwITCH\":[\n"
		,0//system_get_free_heap_size ()
		,(int)SCDEFn_at_ESP32_SwITCH_M->GetTiStFn() );

	HTTPD_Send_To_Send_Buffer (conn, p_leadIn, len);

        free(p_leadIn);
  }

//---------------------------------------------------------------------------------------------------
















				len = 0;

				// 'tist'-check result: Output this ROW
				len += sprintf(buff, 
					"%s{",
					( BWS & 1 << 4 ) ? "" : "},\n");  // "first row - no Komma" flag
					
				//insert fn result
				strncpy(buff + len, (char *) respMsg->string.p_char, respMsg->string.len);
 
				len += respMsg->string.len;

				if (respMsg->string.p_char) free(respMsg->string.p_char);
				free(respMsg);
				respMsg = NULL;

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










// longpoll?? no output till now ?
		if ( BWS & 1 << 4 ) {

/*			#if ESP32_SwITCH_Module_DBG >= 4
			os_printf("|Rmt.TIST:%d, Feat.TIST:%d, Real TIST: %d, LP!->%d Sec>",
				remote_tist,
				oldest_feat_tist,
				(int)SCDEFn_at_ESP32_SwITCH_M->GetTiStFn(),
				conn->LP10HzCounter);
			#endif
*/
			// do longpoll, limit time (20 sec)
			if ( conn->LP10HzCounter < 20 * 10 ) {

				conn->LP10HzCounter++;

				return HTTPD_CGI_PROCESS_CONN;
			}
		}










	BWS &= ~( 1 << 5 );
  }

//--------------------------------------------------------------------------------------------------

  // do "jso Lead Out" ?
  if ( BWS & 1 << 6 ) {

	BWS &= ~( 1 << 6 );

	len = sprintf (buff, "}]\n}\n}");

	if ( !HTTPD_Send_To_Send_Buffer (conn, buff, len) ) return HTTPD_CGI_PROCESS_CONN;
  }

//--------------------------------------------------------------------------------------------------












// test args
//GET switch://192.168.0.24:80/SSR.1.at.GPIO.13.get?TIST=0 HTTP/1.1\r\n\r\n
//GET switch://192.168.0.24:80/SwITCH.get?TIST=0 HTTP/1.1\r\n\r\n



/*

        char **argv;
        int argc;

        // split the query to args
        argv = SCDEFn_at_ESP32_SwITCH_M->ArgParse_SplitURLEncodedArgsToAllocatedMemFn(&argc, Args);

        respMsg = ESP32_SwITCH_GetV2(conn->p_entry_active_dir_matching_definition
              ,argv, argc);

        // ArgParse Split_ToArgsFn has allocated mem. Free it.
        SCDEFn_at_ESP32_SwITCH_M->ArgParse_FreeSplittedArgsInAllocatedMemFn(argv);
*/
	// assuming processed OK
	conn->parser_http_errno = 0;

	// parsed query string  - go to next step
	S2_State = S2S_get_TX_Response_with_Code;

	}

	break;

//--------------------------------------------------------------------------------------------------

	// #06 add response to the TX (response) buffer. TX + Disconn if error code

	case S2S_get_TX_Response_with_Code:

	// start resonse header with code and desc.
	SCDED_StartRespHeader2(conn);

	// >0 -> error, something went wrong, disconn
	if (conn->parser_http_errno) {

		// response added to TX buf, code ERROR - NO header fields, NO body data !!!
		return HTTPD_CGI_DISCONNECT_CONN;
	}

	// response added to TX buf, code OK - go to next step
	S2_State = S2S_get_TX_Add_HDR_Flds;

	break;

//--------------------------------------------------------------------------------------------------

	// #07 add header fields (if any) to the TX (response) buffer

	case S2S_get_TX_Add_HDR_Flds:

	// add 'CONTENT-LENGHT' Hdr-Fld, if we will send data
	if (respMsg && respMsg->string.len) {

	  char *contentLengthStr;

          int contentLengthLen = asprintf(&contentLengthStr
			,"%d"
			,respMsg->string.len);

          if (contentLengthStr) {

		SCDED_AddHdrFld(conn, "CONTENT-LENGHT", contentLengthStr, contentLengthLen);

		free(contentLengthStr);
	  }
	}

	// end header by adding '\r\n'
	SCDED_EndHeader(conn);

	// header fields added - go to next step
	S2_State = S2S_get_TX_Add_Body_Data;

	break;

//--------------------------------------------------------------------------------------------------

	// #08 add body data (if any) to the TX (response) buffer

	case S2S_get_TX_Add_Body_Data:

	if (respMsg) {

		HTTPD_Send_To_Send_Buffer(conn
			,(char *) respMsg->string.p_char
			,respMsg->string.len);

		if (respMsg->string.p_char) free(respMsg->string.p_char);
		free(respMsg);
	}

	// body data added - go to next step
	S2_State = S2S_get_DONE_TX_Response_Buff;

	break;

//--------------------------------------------------------------------------------------------------

	// #09 done, incl. response, now TX response buffer and then disconnect
	case S2S_get_DONE_TX_Response_Buff:

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
	S2_State = S2S_get_TX_Response_with_Code;

	break;

//--------------------------------------------------------------------------------------------------

  }

  goto reexecute;

  return HTTPD_CGI_DISCONNECT_CONN;
}



