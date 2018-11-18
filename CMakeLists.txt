#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(radStudy)


# Default install path is the source directory
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(STATUS "    Install-prefix was at default -> forcing it to the source-dir" )
    message(STATUS "    Use -DCMAKE_INSTALL_PREFIX=/usr/local to set to something else" )
    set (CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}"
         CACHE PATH "default install path" FORCE )
endif()

SITE_NAME(mySite)
message("On this thing: " ${mySite})

#ROOT path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
find_package(ROOT REQUIRED)


# this is to ensure we find the header we need
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

#----------------------------------------------------------------------------
# generate dictionary
ROOT_GENERATE_DICTIONARY(
    peDict                         # path to dictionary to generate
    "include/petypes.hh"           # list of classes to process
    LINKDEF "include/peLinkDef.h"  # ROOT linkDef file
    OPTIONS -p
)

#----------------------------------------------------------------------------
# Debugging symbols, warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -pg")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -g -Woverloaded-virtual")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -g -Wno-shadow -Wno-pedantic")

# Ignore warning of struct initialization { } for gcc < 5.0 (after that it is ignored internally)
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
endif()

# C++11 standard if avialable
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++11 support.")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++0x support.")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

#----------------------------------------------------------------------------
# Generate dictionaries as well (so we can write objects to ROOT files
#
include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
set(LINK_LIBRARIES ${ROOT_LIBRARIES} )

add_library(pe-lib SHARED ${sources} ${headers} peDict.cxx)
set_target_properties(pe-lib PROPERTIES OUTPUT_NAME pe)
target_link_libraries(pe-lib ${ROOT_LIBRARIES} )

add_executable(pe-bin pe.cc)
set_target_properties(pe-bin PROPERTIES OUTPUT_NAME pe)
target_link_libraries(pe-bin pe-lib)

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(PE DEPENDS pe-bin)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS pe-bin DESTINATION bin)
install(TARGETS pe-lib DESTINATION lib)
