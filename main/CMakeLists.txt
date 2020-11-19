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
                    INCLUDE_DIRS "include"
                    )



# the project configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# the SCDE public headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/scde_core/include)



add_subdirectory(scde_core)
add_subdirectory(lib) #scde_optional)



# Create and import the library targets

 # The SCDE default library targets
 #  include(scde_core/DefaultLibTargets.txt)
 
 # This SCDE projects additional library targets
 
  # the additional Modules
#  add_subdirectory(lib/modules/Telnet_Module)
  #add_subdirectory(lib/modules/WebIf_Module) 


                    
# Link the librarys

 # The SCDE Default Librarys
 
  # the Core
  target_link_libraries(${COMPONENT_LIB} PUBLIC SCDE_CORE)

  # the embedded Commands
  target_link_libraries(${COMPONENT_LIB} PUBLIC Attr_Command
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
  # the embedded Modules																					
											Global_Module
  # the additional Commands
   
  # the additional Modules
											Telnet_Module)      
											
											
											
											
											
											
											           
