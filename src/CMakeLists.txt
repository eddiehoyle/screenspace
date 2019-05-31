set(SS_LIBRARY screenspace)
set(SS_SOURCE_FILES
        Log.hh
        Log.cc
        Plugin.cc
        )

add_library(${SS_LIBRARY} SHARED ${SS_SOURCE_FILES})
target_include_directories(${SS_LIBRARY} PRIVATE Maya::Maya)
target_compile_definitions(${SS_LIBRARY} PRIVATE TNC_LOGGING_ENABLED=1)
MAYA_PLUGIN(${SS_LIBRARY})
target_link_libraries(${SS_LIBRARY} PRIVATE Maya::Maya)

