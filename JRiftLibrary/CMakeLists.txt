cmake_minimum_required(VERSION 2.8)

project(JRiftLibrary)

find_package(JNI REQUIRED)

add_subdirectory (OculusSDK)
 
include_directories(Include ${CMAKE_CURRENT_SOURCE_DIR}/Src)
include_directories(Include ${CMAKE_CURRENT_SOURCE_DIR}/OculusSDK/LibOVR/Include)
include_directories(Include ${CMAKE_CURRENT_SOURCE_DIR}/OculusSDK/LibOVR/Src)
include_directories(Include ${JNI_INCLUDE_DIRS})
link_directories(Include ${CMAKE_BINARY_DIR}/output)

if(WIN32)
    set ( NATIVES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/natives/windows )
elseif(APPLE)
    set ( NATIVES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/natives/osx )
else() 
    set ( NATIVES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/natives/linux )
endif()

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    set(BITS "64")
else()
    set(BITS "")
endif()

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${NATIVES_DIR} )
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${NATIVES_DIR} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set( JRIFT_DYNLIB_NAME JRiftLibrary${BITS} )
add_library(${JRIFT_DYNLIB_NAME} SHARED ${CMAKE_CURRENT_SOURCE_DIR}/Src/JRiftLibrary.cpp)
target_link_libraries (${JRIFT_DYNLIB_NAME} general ${OVR_LIBRARIES})
target_link_libraries (${JRIFT_DYNLIB_NAME} debug OculusVRd)
target_link_libraries (${JRIFT_DYNLIB_NAME} optimized OculusVR)


