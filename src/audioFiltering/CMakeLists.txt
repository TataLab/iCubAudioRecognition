# Authors: Lukas Grasse
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.8.9)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

SET(KEYWORD "audioFiltering")
PROJECT(${KEYWORD})

set(ICUB_CONTRIB_DIRS $ENV{ICUB_DIR}/include)

INCLUDE_DIRECTORIES(
    ${YARP_INCLUDE_DIRS}
    ${ICUB_INCLUDE_DIRS}
    ${ICUB_CONTRIB_DIRS}
)

add_definitions(-std=c++11 -static
        -DWEBRTC_POSIX=1
        -DWEBRTC_AUDIO_PROCESSING_ONLY_BUILD=1)

include_directories (${WEBRTC_AUDIO_PROCESSING_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)
link_directories(${WEBRTC_AUDIO_PROCESSING_LIBRARIES})
#link_libraries(webrtc_audio_processing)


# Search for source code.
FILE(GLOB folder_source src/*.cpp src/*.cc src/*.c ../Configuration/*.cpp ../Configuration/*.cc ../Configuration/*.c)
FILE(GLOB folder_header include/iCub/*.h ../Configuration/*.h)
SOURCE_GROUP("Source Files" FILES ${folder_source} )
SOURCE_GROUP("Header Files" FILES ${folder_header} )

# Set up the main executable.
IF (folder_source)
    ADD_EXECUTABLE(${KEYWORD}
        ${folder_source}
        ${folder_header}
    )

    TARGET_LINK_LIBRARIES(${KEYWORD}
      ${YARP_LIBRARIES}
      ${WEBRTC_AUDIO_PROCESSING_LIBRARIES}
      )

    INSTALL_TARGETS(/bin ${KEYWORD})
ELSE (folder_source)
    MESSAGE(FATAL_ERROR "No source code files found. Please add something")
ENDIF (folder_source)
