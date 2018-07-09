#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
SITE_NAME(mySite)
message("On this thing: " ${mySite})
if(${mySite} MATCHES "udc-*")
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
endif()
project(prexsim)
set(useROOT true)        # use true or false
#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
if(${mySite} MATCHES "ifarm*")
   if(NOT DEFINED Geant4_DIR)
   	  set(Geant4_DIR $ENV{G4INSTALL}/build)
   endif()
   message("On ifarm using Geant4_dir: " ${Geant4_Dir})
endif()
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
find_package(Geant4 REQUIRED ui_all vis_all gdml)
else()
find_package(Geant4 REQUIRED gdml)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(FindROOT.cmake)
message(STATUS "Found Root ${ROOT_LIBRARY_DIR}")
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIR} )
set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR})
link_directories( ${LINK_DIRECTORIES})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
set(LINK_LIBRARIES ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} )

#add_definitions(-std=c++11)
add_executable(hallRad hallRadiation.cc ${sources} ${headers})
target_link_libraries(hallRad ${LINK_LIBRARIES})
