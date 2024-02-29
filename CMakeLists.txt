# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(PANDASim)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

IF(UNIX)
#Setup CRY-----------------------------------
find_library( CRY NAMES CRY PATHS $ENV{CRYHOME}/lib NO_DEFAULT_PATH )

set(CRY_INCLUDE_DIRS $ENV{CRYHOME}/src)
#set(CRY_INCLUDE_DIRS /home/lab/programs/cry/cry_v1.7/src)
MESSAGE ("Cry setup: " ${CRY_INCLUDE_DIRS} )
#set(CRY_LIBRARY_DIR ${CRYHOME}/lib)
set(CRY_LIBRARIES $ENV{CRYHOME}/lib/libCRY.a)
#set(CRY_LIBRARIES /home/lab/programs/cry/cry_v1.7//lib/libCRY.a)
#-----------------------------------------------

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#

include_directories(${CRY_INCLUDE_DIRS})
ENDIF ()
#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(PANDASim PANDASim.cc ${sources} ${headers})
target_link_libraries(PANDASim ${Geant4_LIBRARIES})

IF (UNIX)
target_link_libraries(PANDASim ${CRY_LIBRARIES})
ENDIF ()

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build PANDASim. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(PANDASim_SCRIPTS
  init_vis.mac
  vis.mac
  run.mac
  cry.mac
  calibration.mac
  neutrino.mac
  He8Li9.mac
  calibration.mac
  detector.mac
  physics.mac
  README.md
  )

foreach(_script ${PANDASim_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

file(COPY ${PROJECT_SOURCE_DIR}/spectra DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/output DESTINATION ${PROJECT_BINARY_DIR})

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(PANDA DEPENDS PANDASim)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS PANDASim DESTINATION bin)


