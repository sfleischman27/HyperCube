cmake_minimum_required(VERSION 3.5)

project(WebP LANGUAGES C VERSION "1.0.3")

# Options for coder / decoder executables.
option(WEBP_ENABLE_SIMD "Enable any SIMD optimization." ON)
option(WEBP_BUILD_ANIM_UTILS "Build animation utilities." ON)
option(WEBP_BUILD_CWEBP "Build the cwebp command line tool." OFF)
option(WEBP_BUILD_DWEBP "Build the dwebp command line tool." OFF)
option(WEBP_BUILD_GIF2WEBP "Build the gif2webp conversion tool." OFF)
option(WEBP_BUILD_IMG2WEBP "Build the img2webp animation tool." OFF)
option(WEBP_BUILD_VWEBP "Build the vwebp viewer tool." OFF)
option(WEBP_BUILD_WEBPINFO "Build the webpinfo command line tool." OFF)
option(WEBP_BUILD_WEBPMUX "Build the webpmux command line tool." OFF)
option(WEBP_BUILD_EXTRAS "Build extras." ON)
option(WEBP_BUILD_WEBP_JS "Emscripten build of webp.js." OFF)
option(WEBP_NEAR_LOSSLESS "Enable near-lossless encoding" ON)
option(WEBP_ENABLE_SWAP_16BIT_CSP "Enable byte swap for 16 bit colorspaces."
       OFF)
set(WEBP_BITTRACE "0" CACHE STRING "Bit trace mode (0=none, 1=bit, 2=bytes)")
set_property(CACHE WEBP_BITTRACE PROPERTY STRINGS 0 1 2)

# Option needed for handling Unicode file names on Windows.
if(WIN32)
  option(WEBP_UNICODE "Build Unicode executables." ON)
endif()

if(WEBP_BUILD_WEBP_JS)
  set(WEBP_ENABLE_SIMD OFF)
  set(WEBP_BUILD_ANIM_UTILS OFF)
  set(WEBP_BUILD_CWEBP OFF)
  set(WEBP_BUILD_DWEBP OFF)
  set(WEBP_BUILD_GIF2WEBP OFF)
  set(WEBP_BUILD_IMG2WEBP OFF)
  set(WEBP_BUILD_VWEBP OFF)
  set(WEBP_BUILD_WEBPINFO OFF)
  set(WEBP_BUILD_WEBPMUX OFF)
  set(WEBP_BUILD_EXTRAS OFF)
endif()

set(WEBP_DEP_LIBRARIES)
set(WEBP_DEP_INCLUDE_DIRS)

# Include dependencies.
include(cmake/deps.cmake)
include(GNUInstallDirs)

# ##############################################################################
# Options.
if(WEBP_ENABLE_SWAP_16BIT_CSP)
  add_definitions(-DWEBP_SWAP_16BIT_CSP=1)
endif()

if(NOT WEBP_BITTRACE STREQUAL "0")
  add_definitions(-DBITTRACE=${WEBP_BITTRACE})
endif()

if(WEBP_UNICODE)
  # Windows recommends setting both UNICODE and _UNICODE.
  add_definitions(-DUNICODE -D_UNICODE)
endif()

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix "\$\{prefix\}")
set(libdir "\$\{prefix\}/lib")
set(includedir "\$\{prefix\}/include")
set(PTHREAD_LIBS ${CMAKE_THREAD_LIBS_INIT})
set(INSTALLED_LIBRARIES)

# ##############################################################################
# Android only.
if(ANDROID)
  include_directories(${ANDROID_NDK}/sources/android/cpufeatures)
  add_library(cpufeatures STATIC
              ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
  list(APPEND INSTALLED_LIBRARIES cpufeatures)
  target_link_libraries(cpufeatures dl)
  set(WEBP_DEP_LIBRARIES ${WEBP_DEP_LIBRARIES} cpufeatures)
  set(WEBP_DEP_INCLUDE_DIRS ${WEBP_DEP_INCLUDE_DIRS}
      ${ANDROID_NDK}/sources/android/cpufeatures)
  add_definitions(-DHAVE_CPU_FEATURES_H=1)
  set(HAVE_CPU_FEATURES_H 1)
else()
  set(HAVE_CPU_FEATURES_H 0)
endif()

function(configure_pkg_config FILE)
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/${FILE}")

  if(HAVE_MATH_LIBRARY)
    # MSVC doesn't have libm
    file(READ ${CMAKE_CURRENT_BINARY_DIR}/${FILE} data)
    string(REPLACE "-lm" "" data ${data})
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${FILE} ${data})
  endif()

  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${FILE}"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
  )
endfunction()

# ##############################################################################
# WebP source files. Read the Makefile.am to get the source files.

# We expect the Makefiles to define the sources as defined in the first regex.
# E.g.: libimagedec_la_SOURCES  = image_dec.c image_dec.h
function(parse_Makefile_am FOLDER VAR SRC_REGEX)
  file(READ ${FOLDER}/Makefile.am MAKEFILE_AM)
  string(REGEX MATCHALL
               "${SRC_REGEX}_SOURCES[ ]*\\+?=[ ]+[0-9a-z\\._ ]*"
               FILES_PER_LINE
               ${MAKEFILE_AM})
  set(SRCS ${${VAR}})
  foreach(FILES ${FILES_PER_LINE})
    string(FIND ${FILES} "=" OFFSET)
    math(EXPR OFFSET "${OFFSET} + 2")
    string(SUBSTRING ${FILES}
                     ${OFFSET}
                     -1
                     FILES)
    if(FILES)
      string(REGEX MATCHALL
                   "[0-9a-z\\._]+"
                   FILES
                   ${FILES})
      foreach(FILE ${FILES})
        list(APPEND SRCS ${FOLDER}/${FILE})
      endforeach()
    endif()
  endforeach()
  set(${VAR} ${SRCS} PARENT_SCOPE)
endfunction()

set(WEBP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
parse_makefile_am(${WEBP_SRC_DIR}/dec "WEBP_DEC_SRCS" "")
parse_makefile_am(${WEBP_SRC_DIR}/demux "WEBP_DEMUX_SRCS" "")
parse_makefile_am(${WEBP_SRC_DIR}/dsp "WEBP_DSP_COMMON_SRCS" "COMMON")
parse_makefile_am(${WEBP_SRC_DIR}/dsp "WEBP_DSP_ENC_SRCS" "ENC")
parse_makefile_am(${WEBP_SRC_DIR}/dsp "WEBP_DSP_ENC_SRCS" "dsp_[^ ]*")
parse_makefile_am(${WEBP_SRC_DIR}/dsp "WEBP_DSP_DEC_SRCS" "decode_[^ ]*")
parse_makefile_am(${WEBP_SRC_DIR}/enc "WEBP_ENC_SRCS" "")
parse_makefile_am(${WEBP_SRC_DIR}/utils "WEBP_UTILS_COMMON_SRCS" "COMMON")
parse_makefile_am(${WEBP_SRC_DIR}/utils "WEBP_UTILS_ENC_SRCS" "ENC")
parse_makefile_am(${WEBP_SRC_DIR}/utils "WEBP_UTILS_DEC_SRCS" "decode_[^ ]*")

# Remove the files specific to SIMD we don't use.
foreach(FILE ${WEBP_SIMD_FILES_NOT_TO_INCLUDE})
  list(REMOVE_ITEM WEBP_DSP_ENC_SRCS ${FILE})
  list(REMOVE_ITEM WEBP_DSP_DEC_SRCS ${FILE})
endforeach()

# Generate the config.h file.
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/src/webp/config.h)
add_definitions(-DHAVE_CONFIG_H)
if (MSVC)
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-command-line-argument -Wno-xor-used-as-pow")
endif()

# ##############################################################################
# Build the webpdecoder library.
if(MSVC)
  # avoid security warnings for e.g., fopen() used in the examples.
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
  add_definitions(-Wall)
endif()
include_directories(${WEBP_DEP_INCLUDE_DIRS})
add_library(webpdecode OBJECT ${WEBP_DEC_SRCS})
target_include_directories(webpdecode
                           PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR})
add_library(webpdspdecode OBJECT ${WEBP_DSP_COMMON_SRCS} ${WEBP_DSP_DEC_SRCS})
target_include_directories(webpdspdecode
                           PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR})
add_library(webputilsdecode
            OBJECT
            ${WEBP_UTILS_COMMON_SRCS}
            ${WEBP_UTILS_DEC_SRCS})
target_include_directories(webputilsdecode
                           PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR})
add_library(webpdecoder STATIC
            $<TARGET_OBJECTS:webpdecode>
            $<TARGET_OBJECTS:webpdspdecode>
            $<TARGET_OBJECTS:webputilsdecode>)
target_link_libraries(webpdecoder ${WEBP_DEP_LIBRARIES})
target_include_directories(
  webpdecoder
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
set_target_properties(
  webpdecoder
  PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/webp/decode.h;\
${CMAKE_CURRENT_SOURCE_DIR}/src/webp/types.h")

configure_pkg_config("src/libwebpdecoder.pc")

# Build the webp library.
add_library(webpencode OBJECT ${WEBP_ENC_SRCS})
target_include_directories(webpencode
                           PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR})
add_library(webpdsp
            OBJECT
            ${WEBP_DSP_COMMON_SRCS}
            ${WEBP_DSP_DEC_SRCS}
            ${WEBP_DSP_ENC_SRCS})
target_include_directories(webpdsp
                           PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR})
add_library(webputils
            OBJECT
            ${WEBP_UTILS_COMMON_SRCS}
            ${WEBP_UTILS_DEC_SRCS}
            ${WEBP_UTILS_ENC_SRCS})
target_include_directories(webputils
                           PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR})
add_library(webp STATIC
            $<TARGET_OBJECTS:webpdecode>
            $<TARGET_OBJECTS:webpdsp>
            $<TARGET_OBJECTS:webpencode>
            $<TARGET_OBJECTS:webputils>)
target_link_libraries(webp ${WEBP_DEP_LIBRARIES})
target_include_directories(webp
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_BINARY_DIR}
                           PUBLIC $<INSTALL_INTERFACE:include>)
set_target_properties(
  webp
  PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/webp/decode.h;\
${CMAKE_CURRENT_SOURCE_DIR}/src/webp/encode.h;\
${CMAKE_CURRENT_SOURCE_DIR}/src/webp/types.h")

# Make sure the OBJECT libraries are built with position independent code (it is
# not ON by default).
set_target_properties(webpdecode
                      webpdspdecode
                      webputilsdecode
                      webpencode
                      webpdsp
                      webputils
                      PROPERTIES POSITION_INDEPENDENT_CODE ON)
configure_pkg_config("src/libwebp.pc")

# Build the webp demux library.
add_library(webpdemux ${WEBP_DEMUX_SRCS})
target_link_libraries(webpdemux webp)
target_include_directories(webpdemux
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_BINARY_DIR}
                           PUBLIC $<INSTALL_INTERFACE:include>)
set_target_properties(
  webpdemux
  PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/webp/decode.h;\
${CMAKE_CURRENT_SOURCE_DIR}/src/webp/demux.h;\
${CMAKE_CURRENT_SOURCE_DIR}/src/webp/mux_types.h;\
${CMAKE_CURRENT_SOURCE_DIR}/src/webp/types.h")

configure_pkg_config("src/demux/libwebpdemux.pc")

# Set the version numbers.
function(parse_version FILE NAME VAR)
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/src/${FILE} SOURCE_FILE)
  string(REGEX MATCH
               "${NAME}_la_LDFLAGS[^\n]* -version-info [0-9:]+"
               TMP
               ${SOURCE_FILE})
  string(REGEX MATCH
               "[0-9:]+"
               TMP
               ${TMP})
  string(REGEX
         REPLACE ":"
                 "."
                 VERSION
                 ${TMP})
  set(${VAR} "${VERSION}" PARENT_SCOPE)
endfunction()
parse_version(Makefile.am webp WEBP_WEBP_SOVERSION)
set_target_properties(webp
                      PROPERTIES VERSION
                                 ${PACKAGE_VERSION}
                                 SOVERSION
                                 ${WEBP_WEBP_SOVERSION})
parse_version(Makefile.am webpdecoder WEBP_DECODER_SOVERSION)
set_target_properties(webpdecoder
                      PROPERTIES VERSION
                                 ${PACKAGE_VERSION}
                                 SOVERSION
                                 ${WEBP_DECODER_SOVERSION})
parse_version(demux/Makefile.am webpdemux WEBP_DEMUX_SOVERSION)
set_target_properties(webpdemux
                      PROPERTIES VERSION
                                 ${PACKAGE_VERSION}
                                 SOVERSION
                                 ${WEBP_DEMUX_SOVERSION})
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/configure.ac CONFIGURE_FILE)
string(REGEX MATCH
             "AC_INIT\\([^\n]*\\[[0-9\\.]+\\]"
             TMP
             ${CONFIGURE_FILE})
string(REGEX MATCH
             "[0-9\\.]+"
             PROJECT_VERSION
             ${TMP})

# Define the libraries to install.
list(APPEND INSTALLED_LIBRARIES webpdecoder webp webpdemux)

# Deal with SIMD. Change the compile flags for SIMD files we use.
list(LENGTH WEBP_SIMD_FILES_TO_INCLUDE WEBP_SIMD_FILES_TO_INCLUDE_LENGTH)
math(EXPR WEBP_SIMD_FILES_TO_INCLUDE_RANGE
     "${WEBP_SIMD_FILES_TO_INCLUDE_LENGTH}-1")

foreach(I_FILE RANGE ${WEBP_SIMD_FILES_TO_INCLUDE_RANGE})
  list(GET WEBP_SIMD_FILES_TO_INCLUDE ${I_FILE} FILE)
  list(GET WEBP_SIMD_FLAGS_TO_INCLUDE ${I_FILE} SIMD_COMPILE_FLAG)
  set_source_files_properties(${FILE}
                              PROPERTIES
                              COMPILE_FLAGS
                              ${SIMD_COMPILE_FLAG})
endforeach()

# Install the different headers and libraries.
include(GNUInstallDirs)
install(TARGETS ${INSTALLED_LIBRARIES}
        EXPORT ${PROJECT_NAME}Targets
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/webp
        INCLUDES
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
set(ConfigPackageLocation ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/cmake/)
install(EXPORT ${PROJECT_NAME}Targets
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${ConfigPackageLocation})

# Create the CMake version file.
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/WebPConfigVersion.cmake"
  VERSION ${PACKAGE_VERSION}
  COMPATIBILITY AnyNewerVersion)

# Create the Config file.
include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/WebPConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/WebPConfig.cmake
  INSTALL_DESTINATION
  ${ConfigPackageLocation})

# Install the generated CMake files.
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/WebPConfigVersion.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/WebPConfig.cmake"
        DESTINATION ${ConfigPackageLocation})
