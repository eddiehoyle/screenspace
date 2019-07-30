set(SS_LIBRARY screenspace)
set(SS_SOURCE_FILES
        ss/Log.hh
        ss/Log.cc
        ss/PickableDrawOverride.cc
        ss/PickableDrawOverride.hh
        ss/PickableShape.cc
        ss/PickableShape.hh
        ss/Plugin.cc
        ss/commands/AddCommand.cc
        ss/commands/AddCommand.hh
        ss/commands/RemoveCommand.cc
        ss/commands/RemoveCommand.hh
        )

add_library(${SS_LIBRARY} SHARED ${SS_SOURCE_FILES})
target_include_directories(${SS_LIBRARY} PRIVATE ${OPENGL_INCLUDE_DIRS} ${MAYA_INCLUDE_DIR} .)

set(COMPILE_DEFINITIONS)
if (APPLE)
    list(APPEND COMPILE_DEFINITIONS OSMac_)
endif(APPLE)
target_compile_definitions(${SS_LIBRARY} PRIVATE ${COMPILE_DEFINITIONS})

target_link_libraries(${SS_LIBRARY} ${MAYA_LIBRARIES})
MAYA_PLUGIN(${SS_LIBRARY})

add_custom_command(
        TARGET ${SS_LIBRARY} POST_BUILD
        COMMENT "Copying icons..."
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/ss/icons
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/icons)

add_custom_command(
        TARGET ${SS_LIBRARY} POST_BUILD
        COMMENT "Copying scripts..."
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/ss/scripts
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/scripts)

add_custom_command(TARGET ${SS_LIBRARY} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "All done!")