set(SS_LIBRARY screenspace)
set(SS_SOURCE_FILES
        screenspace/PickerDrawOverride.cc
        screenspace/PickerDrawOverride.hh
        screenspace/PickerShape.cc
        screenspace/PickerShape.hh
        screenspace/commands/AddCommand.cc
        screenspace/commands/AddCommand.hh
        screenspace/commands/RemoveCommand.cc
        screenspace/commands/RemoveCommand.hh
        screenspace/Log.hh
        screenspace/Log.cc
        screenspace/Plugin.cc
        )

add_library(${SS_LIBRARY} SHARED ${SS_SOURCE_FILES})
target_include_directories(${SS_LIBRARY} PRIVATE ${OPENGL_INCLUDE_DIRS} ${MAYA_INCLUDE_DIR} .)
target_compile_definitions(${SS_LIBRARY} PRIVATE TNC_LOGGING_ENABLED=1 OSMac_)

target_link_libraries(${SS_LIBRARY} ${MAYA_LIBRARIES})
MAYA_PLUGIN(${SS_LIBRARY})


