# general cmake stuff
cmake_minimum_required (VERSION 2.6)
project (LLVM-Handbook)

# support cpp 11
set(CMAKE_CXX_FLAGS "-std=c++11")

# LLVM stuff
find_package(LLVM
  PATHS "${LLVM_ROOT}/lib/cmake/llvm" "$ENV{ProgramFiles}/lib/cmake/llvm")

if( LLVM_FOUND )
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_DIR}")
else()
    message(FATAL_ERROR "LLVM installation not found. Set LLVM_ROOT to the installation root path.")
endif()

include(LLVMConfig)
message(STATUS "Found LLVM Package Version:${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM Built type : ${LLVM_BUILD_TYPE}")
string(REGEX MATCH "[0-9]+\\.[0-9]+" LLVM_VER ${LLVM_PACKAGE_VERSION} )
if(${LLVM_VER} VERSION_LESS "3.8" )
    message(FATAL_ERROR "LLVM version before 3.8 are not supported. Abort!")
endif()
if(NOT ${LLVM_VER} VERSION_EQUAL "6.0" )
    message(WARNING "LLVM version before ${LLVM_VER} is not tested.")
endif()

llvm_map_components_to_libnames(REQ_LLVM_LIBRARIES mcjit interpreter native ipo)

# this needs to happen for every file

# 01
add_executable(01_printing 01_printing.cpp) # compile
target_link_libraries(01_printing ${REQ_LLVM_LIBRARIES}) # link with llvm

# 02
add_executable(02_numbers 02_numbers.cpp) # compile
target_link_libraries(02_numbers ${REQ_LLVM_LIBRARIES}) # link with llvm

# 03
add_executable(03_computation 03_computation.cpp) # compile
target_link_libraries(03_computation ${REQ_LLVM_LIBRARIES}) # link with llvm

# 04
add_executable(04_functions 04_functions.cpp) # compile
target_link_libraries(04_functions ${REQ_LLVM_LIBRARIES}) # link with llvm

# 05
add_executable(05_variables 05_variables.cpp) # compile
target_link_libraries(05_variables ${REQ_LLVM_LIBRARIES}) # link with llvm
