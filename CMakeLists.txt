cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME_STR ruffian)
project(${PROJECT_NAME_STR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(LLVM_ROOT "" CACHE PATH "Root of LLVM install.")

if( NOT EXISTS ${LLVM_ROOT}/include/llvm )
  message(FATAL_ERROR "LLVM_ROOT (${LLVM_ROOT}) is not a valid LLVM install")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_ROOT}/share/llvm/cmake")

include(LLVMConfig)

link_directories( ${LLVM_LIBRARY_DIRS} )
add_definitions( ${LLVM_DEFINITIONS} )

llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES jit native)

enable_testing()

set(COMMON_INCLUDES ${LLVM_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(tests)

