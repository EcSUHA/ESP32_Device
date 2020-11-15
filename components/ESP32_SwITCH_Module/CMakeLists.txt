idf_component_register(SRCS "ESP32_SwITCH_cgi.c"
"ESP32_SwITCH_jso.c"
"ESP32_SwITCH_Module.c"
"ESP32_SwITCH_set.c"
"ESP32_SwITCH_tpl.c"
"ESP32_SwITCH_WEBIF_get_cb.c"
   
                     INCLUDE_DIRS "include"
                                  "../../main/include"
                                  "../WebIf_Module/include"
                                  "../ESP32_I2C_Master_Module/include"
                                  "../ESP32_DeVICE_Module/include"
                                  "../WebIf_Content/include")
