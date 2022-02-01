cmake_minimum_required(VERSION 3.13.0 FATAL_ERROR)

project(ChucKDesigner VERSION 0.0.1)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ChucKDesignerCHOP)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################
project(ChucKDesignerShared VERSION 0.0.1)

file(GLOB CK_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/regcomp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/regerror.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/regexec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/tre-ast.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/tre-compile.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/tre-filter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/tre-match-approx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/tre-match-backtrack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/tre-match-parallel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/tre-mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/tre-parse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/tre-stack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex/tre-xmalloc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/lo/*.c
)

if(WIN32)
    list(FILTER CK_SOURCES EXCLUDE REGEX ".*server_thread.c")
    list(FILTER CK_SOURCES EXCLUDE REGEX ".*lo/config.h")
else()
    list(FILTER CK_SOURCES EXCLUDE REGEX ".*win32.h")
    list(FILTER CK_SOURCES EXCLUDE REGEX ".*win32.c")
    list(FILTER CK_SOURCES EXCLUDE REGEX ".*core/regex/*")
    list(APPEND CK_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/chuck.yy.c")
endif()

file(GLOB TOUCHDESIGNER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/TouchDesigner/*)

add_library(ChucKDesignerShared SHARED
    ${CK_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Plugin_ChucK.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Plugin_ChucK.cpp
    ${TOUCHDESIGNER_SOURCES}
    )
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

# Include header directories
target_include_directories(ChucKDesignerShared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/TouchDesigner>
    $<INSTALL_INTERFACE:ChucKDesignerShared>
    )
if(APPLE)
    target_include_directories(ChucKDesignerShared PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/lo>
        )

    # Link MultitouchSupport Apple framework
    find_library( multitouchsupportlibs MultitouchSupport /System/Library/PrivateFrameworks )
    message(STATUS "Found multitouchsupportlibs libs: ${multitouchsupportlibs}" )
    target_link_libraries(${PROJECT_NAME} PUBLIC ${multitouchsupportlibs})
else()
    target_include_directories(ChucKDesignerShared PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex>
        )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "NDEBUG"
    "_USRDLL"
    "HAVE_CONFIG_H;"
    "CHUCKDESIGNERSHARED_EXPORTS"
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "WIN32;"
        "__PLATFORM_WIN32__;"
        "__WINDOWS_MODERN__;"
        "__WINDOWS_DS__;"
        "_WINDOWS;"
    )
  # win sock 32
  target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32)
  # windows multimedia for rt midi
  target_link_libraries(${PROJECT_NAME} PUBLIC winmm)
  # more
  target_link_libraries(${PROJECT_NAME} PUBLIC wsock32)
  target_link_libraries(${PROJECT_NAME} PUBLIC dsound dinput8 dxguid)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "__MACOSX_CORE__;"
        "__APPLE__;"
        "__MACH__;"
    )
    target_link_libraries(${PROJECT_NAME} PUBLIC "-framework CoreFoundation" "-framework CoreMIDI" "-framework CoreAudio" "-framework IOKit" "-framework Carbon" "-framework AppKit" "-framework Foundation")
endif()

if (MSVC)
add_custom_command(TARGET ${PROJECT_NAME}
                 POST_BUILD
                 COMMAND ${CMAKE_COMMAND} -E copy_if_different
                 "$<TARGET_FILE:ChucKDesignerShared>"
                 ${CMAKE_SOURCE_DIR}/Plugins
                 # "%USERPROFILE%/Documents/Derivative/Plugins"
                 )
endif()

###################### ChucKListenerCHOP

project(ChucKListenerCHOP VERSION 0.0.1)

add_library(ChucKListenerCHOP MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ChucKListenerCHOP.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ChucKListenerCHOP.cpp
    ${TOUCHDESIGNER_SOURCES}
    )
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_target_properties(${PROJECT_NAME} PROPERTIES
    BUNDLE true
    BUNDLE_EXTENSION "plugin"
    # PRODUCT_BUNDLE_IDENTIFIER design.dirt.cpp.${PROJECT_NAME}
    # MACOSX_BUNDLE_GUI_IDENTIFIER design.dirt.cpp.${PROJECT_NAME}
    MACOSX_BUNDLE_INFO_STRING ${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
    MACOSX_BUNDLE_COPYRIGHT "David Braun"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/Info.plist
    XCODE_ATTRIBUTE_FRAMEWORK_SEARCH_PATHS "/System/Library/PrivateFrameworks /Library/Frameworks"
    )

# Include header directories
target_include_directories(ChucKListenerCHOP PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/TouchDesigner>
    $<INSTALL_INTERFACE:ChucKListenerCHOP>
    )

set(Python3_FIND_REGISTRY "LAST")
set(Python3_FIND_STRATEGY "LOCATION")
find_package(Python3 3.9 EXACT REQUIRED COMPONENTS Interpreter Development)
target_link_libraries(${PROJECT_NAME} PUBLIC Python::Python)

target_link_libraries(${PROJECT_NAME} PUBLIC ChucKDesignerShared)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "NDEBUG"
    "HAVE_CONFIG_H;"
    "_USRDLL"
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "WIN32;"
        "__PLATFORM_WIN32__;"
        "__WINDOWS_MODERN__;"
        "__WINDOWS_DS__;"
        "_WINDOWS;"
    )
    # win sock 32
    target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32)
    # windows multimedia for rt midi
    target_link_libraries(${PROJECT_NAME} PUBLIC winmm)
    # more
    target_link_libraries(${PROJECT_NAME} PUBLIC wsock32)
    target_link_libraries(${PROJECT_NAME} PUBLIC dsound dinput8 dxguid)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "__MACOSX_CORE__;"
        "__APPLE__;"
        "__MACH__;"
    )
    target_link_libraries(${PROJECT_NAME} PUBLIC "-framework CoreFoundation" "-framework CoreMIDI" "-framework CoreAudio")
endif()

if(MSVC)
add_custom_command(TARGET ${PROJECT_NAME}
                 POST_BUILD
                 COMMAND ${CMAKE_COMMAND} -E copy_if_different
                 "$<TARGET_FILE:ChucKListenerCHOP>"
                 ${CMAKE_SOURCE_DIR}/Plugins
                 # "%USERPROFILE%/Documents/Derivative/Plugins"
                 )
endif()

###################### ChucKDesignerCHOP

project(ChucKDesignerCHOP VERSION 0.0.1)

add_library(ChucKDesignerCHOP MODULE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ChucKDesignerCHOP.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ChucKDesignerCHOP.cpp
    ${TOUCHDESIGNER_SOURCES}
    )
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_target_properties(${PROJECT_NAME} PROPERTIES
    BUNDLE true
    BUNDLE_EXTENSION "plugin"
    # PRODUCT_BUNDLE_IDENTIFIER design.dirt.cpp.${PROJECT_NAME}
    # MACOSX_BUNDLE_GUI_IDENTIFIER design.dirt.cpp.${PROJECT_NAME}
    MACOSX_BUNDLE_INFO_STRING ${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
    MACOSX_BUNDLE_COPYRIGHT "David Braun"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/Info.plist
    XCODE_ATTRIBUTE_FRAMEWORK_SEARCH_PATHS "/System/Library/PrivateFrameworks /Library/Frameworks"
    )

# Include header directories
target_include_directories(ChucKDesignerCHOP PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/chuck/src/core/regex>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/TouchDesigner>
    $<INSTALL_INTERFACE:ChucKDesignerCHOP>
    )

set(Python3_FIND_REGISTRY "LAST")
set(Python3_FIND_STRATEGY "LOCATION")
find_package(Python3 3.9 EXACT REQUIRED COMPONENTS Interpreter Development)
target_link_libraries(${PROJECT_NAME} PUBLIC Python::Python)

target_link_libraries(${PROJECT_NAME} PUBLIC ChucKDesignerShared)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "NDEBUG"
    "HAVE_CONFIG_H"
    "_USRDLL"
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "WIN32;"
        "__PLATFORM_WIN32__;"
        "__WINDOWS_MODERN__;"
        "__WINDOWS_DS__;"
        "_WINDOWS;"
    )
    # win sock 32
    target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32)
    # windows multimedia for rt midi
    target_link_libraries(${PROJECT_NAME} PUBLIC winmm)
    # more
    target_link_libraries(${PROJECT_NAME} PUBLIC wsock32)
    target_link_libraries(${PROJECT_NAME} PUBLIC dsound dinput8 dxguid)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "__MACOSX_CORE__;"
        "__APPLE__;"
        "__MACH__;"
    )
    target_link_libraries(${PROJECT_NAME} PUBLIC "-framework CoreFoundation" "-framework CoreMIDI" "-framework CoreAudio")
endif()

if(MSVC)
add_custom_command(TARGET ${PROJECT_NAME}
                 POST_BUILD
                 COMMAND ${CMAKE_COMMAND} -E copy_if_different
                 "$<TARGET_FILE:ChucKDesignerCHOP>"
                 ${CMAKE_SOURCE_DIR}/Plugins
                 # "%USERPROFILE%/Documents/Derivative/Plugins"
                 )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
                      VS_DEBUGGER_COMMAND "C:\\Program Files\\Derivative\\TouchDesigner\\bin\\TouchDesigner.exe"
                      VS_DEBUGGER_COMMAND_ARGUMENTS "..\\ChucKDesigner.toe")
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
