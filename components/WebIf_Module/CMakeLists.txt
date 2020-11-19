idf_component_register(SRCS "WebIf_Module.c"
							"Base64.c"
							"CGI_NoAuthErr.c"
							"CGI_NotFoundErr.c"
							"CGI_Redirect.c"
							"CGI_Web.c"
							"Device_WebIfTokens.c"
							"sha1.c"
							"WebIf_ActiveDirTokens.c"
							"WebIf_EspFSAdvFileTX.c"
							"WebIf_EspFSStdFileTX.c"
							"WebIf_Module_Mimes.c"
							"WebIf_Module_Schemes.c"

   
   
   
   
                    INCLUDE_DIRS "include" 
                                 "../../main/include"
                   				 "../../main/scde_core/include"
                   				 "../WebIf_Content/include"
                   				 "../ESP32_DeVICE_Module/include")
