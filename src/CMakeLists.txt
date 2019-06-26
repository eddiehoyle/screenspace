set(SS_LIBRARY screenspace)
set(SS_SOURCE_FILES
        PickerDrawOverride.cc
        PickerDrawOverride.hh
        PickerShape.cc
        PickerShape.hh
        PickerUI.cc
        PickerUI.hh
        PickerCommand.cc
        PickerCommand.hh
        Log.hh
        Log.cc
        Plugin.cc
        )

#set(SS_SOURCE_FILES
#        Example.cc
#        )

add_library(${SS_LIBRARY} SHARED ${SS_SOURCE_FILES})
target_include_directories(${SS_LIBRARY} PRIVATE ${OPENGL_INCLUDE_DIRS} ${MAYA_INCLUDE_DIR})
target_compile_definitions(${SS_LIBRARY} PRIVATE TNC_LOGGING_ENABLED=1 OSMac_)

target_link_libraries(${SS_LIBRARY} ${MAYA_LIBRARIES})
MAYA_PLUGIN(${SS_LIBRARY})


