cmake_minimum_required (VERSION 3.14 FATAL_ERROR)

find_program(CMAKE_C_COMPILER NAMES $ENV{CC} gcc PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} g++ PATHS ENV PATH NO_DEFAULT_PATH)


project(HKPlotMaker
        LANGUAGES CXX)

#Set this to TRUE to enable build debugging messages
set(BUILD_DEBUG_MSGS TRUE)

#Changes default install path to be a subdirectory of the build dir.
#Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
if(CMAKE_INSTALL_PREFIX STREQUAL "" OR CMAKE_INSTALL_PREFIX STREQUAL
  "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
elseif(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
endif()

############################  ROOT Compiler ####################################
if(NOT DEFINED ROOT_FOUND)

# Get cflags from ROOT
execute_process (COMMAND root-config
--cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" ROOT_CXX_FLAGS "${ROOT_CXX_FLAGS}")
# Get libdir from ROOT
execute_process (COMMAND root-config
--libdir OUTPUT_VARIABLE ROOT_LIBRARY_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
# Get version from ROOT
execute_process (COMMAND root-config
--version OUTPUT_VARIABLE ROOT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

LIST(APPEND ROOT_LIB_NAMES Core)

if(ROOT_VERSION VERSION_LESS 6)
  LIST(APPEND ROOT_LIB_NAMES Cint)
endif()

LIST(APPEND ROOT_LIB_NAMES 
  RIO
  Net
  Hist
  Graf
  Graf3d
  Gpad
  Tree
  Rint
  Postscript
  Matrix
  Physics
  MathCore
  Thread)

foreach(LN ${ROOT_LIB_NAMES})
  if(NOT EXISTS ${ROOT_LIBRARY_DIR}/lib${LN}.so)
    message(FATAL_ERROR "Cannot find ROOT library: ${LN} in expected directory: ${ROOT_LIBRARY_DIR}")
  endif()
endforeach()

add_library(ROOT::ROOT INTERFACE IMPORTED)
set_target_properties(ROOT::ROOT PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ROOT_INCLUDE_DIRS}"
    INTERFACE_COMPILE_OPTIONS "${ROOT_CXX_FLAGS}"
    INTERFACE_LINK_DIRECTORIES "${ROOT_LIBRARY_DIR}"
    INTERFACE_LINK_LIBRARIES "${ROOT_LIB_NAMES}"
)

message(STATUS "Built ROOT::ROOT Imported target (T2KReWeight: ND280Polyfill)")
message(STATUS "        ROOT_INCLUDE_DIRS: ${ROOT_INCLUDE_DIRS}: ")
message(STATUS "        ROOT_CXX_FLAGS: ${ROOT_CXX_FLAGS}")
message(STATUS "        ROOT_LIBRARIES: ${ROOT_LIBRARIES}")
message(STATUS "        ROOT_VERSION: ${ROOT_VERSION}")

else() #Use ROOTConfig.cmake as Fons intended

if(DEFINED ROOT_USE_FILE AND NOT "${ROOT_USE_FILE}x" STREQUAL "x")
include(${ROOT_USE_FILE})
endif()

if(NOT TARGET ROOT::ROOT)
if(NOT DEFINED ROOT_CXX_FLAGS AND DEFINED ROOT_DEFINITIONS)
  SET(ROOT_CXX_FLAGS ${ROOT_DEFINITIONS})
endif()

STRING(STRIP "${ROOT_CXX_FLAGS}" ROOT_CXX_FLAGS)
STRING(REPLACE " " ";" ROOT_CXX_FLAGS ${ROOT_CXX_FLAGS})

if("${CMAKE_PROJECT_NAME} " STREQUAL "T2KReWeight ")
  list (FIND ROOT_CXX_FLAGS "-std=c++11" CPP11_INDEX)
  list (FIND ROOT_CXX_FLAGS "-std=c++14" CPP14_INDEX)
  list (FIND ROOT_CXX_FLAGS "-std=c++17" CPP17_INDEX)
  list (FIND ROOT_CXX_FLAGS "-std=c++20" CPP20_INDEX)
  if (${CPP11_INDEX} GREATER -1)
    SET(CMAKE_CXX_STANDARD 11)
  elseif (${CPP14_INDEX} GREATER -1)
    SET(CMAKE_CXX_STANDARD 14)
  elseif (${CPP17_INDEX} GREATER -1)
    SET(CMAKE_CXX_STANDARD 17)
  elseif (${CPP20_INDEX} GREATER -1)
    SET(CMAKE_CXX_STANDARD 20)
  else()
    SET(CMAKE_CXX_STANDARD 11)
  endif()

  if (NOT DEFINED CMAKE_CXX_STANDARD OR "${CMAKE_CXX_STANDARD} " STREQUAL " ")
    SET(CMAKE_CXX_STANDARD 11)
  endif()
endif()

list(REMOVE_ITEM ROOT_CXX_FLAGS "-std=c++11")
list(REMOVE_ITEM ROOT_CXX_FLAGS "-std=c++14")
list(REMOVE_ITEM ROOT_CXX_FLAGS "-std=c++17")
list(REMOVE_ITEM ROOT_CXX_FLAGS "-std=c++20")
#We should let CMake set this
list(REMOVE_ITEM ROOT_CXX_FLAGS "-fPIC")

execute_process (COMMAND root-config
  --version OUTPUT_VARIABLE ROOT_CONFIG_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

add_library(ROOT::ROOT INTERFACE IMPORTED)

LIST(APPEND ROOT_LIB_NAMES Core)

if(ROOT_VERSION VERSION_LESS 6)
  LIST(APPEND ROOT_LIB_NAMES Cint)
endif()

LIST(APPEND ROOT_LIB_NAMES 
  RIO
  Net
  Hist
  Graf
  Graf3d
  Gpad
  Tree
  Rint
  Postscript
  Matrix
  Physics
  MathCore
  Thread)

foreach(LN ${ROOT_LIB_NAMES})
  if(NOT EXISTS ${ROOT_LIBRARY_DIR}/lib${LN}.so)
    message(FATAL_ERROR "Cannot find ROOT library: ${LN} in expected directory: ${ROOT_LIBRARY_DIR}")
  endif()
endforeach()

set_target_properties(ROOT::ROOT PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ROOT_INCLUDE_DIRS}"
    INTERFACE_COMPILE_OPTIONS "${ROOT_CXX_FLAGS}"
    INTERFACE_LINK_DIRECTORIES "${ROOT_LIBRARY_DIR}"
    INTERFACE_LINK_LIBRARIES "${ROOT_LIB_NAMES}"
)

message(STATUS "Built ROOT::ROOT Imported target (T2KReWeight)")
message(STATUS "        ROOT_INCLUDE_DIRS: ${ROOT_INCLUDE_DIRS}: ")
message(STATUS "        ROOT_CXX_FLAGS: ${ROOT_CXX_FLAGS}")
message(STATUS "        ROOT_LIBRARIES: ${ROOT_LIBRARIES}")
message(STATUS "        ROOT_CONFIG_VERSION: ${ROOT_CONFIG_VERSION}")
endif()
endif()

############################  c++ Compiler  ####################################
include(${CMAKE_SOURCE_DIR}/cmake/c++CompilerSetup.cmake)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(app)