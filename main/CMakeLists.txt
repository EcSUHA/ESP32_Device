# CMakeLists.txt for this Project (needs to be customized)
# Platform: ESP IDF

# Set values of hypothetical variables that control the build of `foo`
set(FOO_BUILD_STATIC OFF)
set(FOO_BUILD_TESTS OFF)    


# Register the component
idf_component_register(SRCS "SCDE_Main.c"
                            "scde_task.c"
                            "c_timeutils.c"     
                            "logging.c"                         
                    INCLUDE_DIRS "include")



# Create and import the library targets

 # The SCDE default library targets
   include(lib/DefaultLibTargets.txt)
 
 # This SCDE projects additional library targets
 
  # the additional Modules
  add_subdirectory(lib/modules/Telnet_Module)
  #add_subdirectory(lib/modules/WebIf_Module) 


                    
# Link tinyxml2 to main component
target_link_libraries(${COMPONENT_LIB} PUBLIC core

											Attr_Command
											Define_Command 
											Delete_Command
											Deleteattr_Command
											Get_Command
											Help_Command
											Include_Command 
											IOWrite_Command	
											List_Command
											Rename_Command
											Rereadcfg_Command
											Save_Command
											Set_Command
											Setstate_Command
											Shutdown_Command
																					
											Global_Module
											Telnet_Module)      
											
	#			  target_link_libraries(${PROJECT_NAME} PUBLIC Telnet_Module)							
											
											
											
											
											
											
											           
