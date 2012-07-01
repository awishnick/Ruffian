find_package(GTest REQUIRED)

set(PROJECT_TEST_NAME "${PROJECT_NAME_STR}_tests")
include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})

add_executable(${PROJECT_TEST_NAME}
  main.cpp
  Lex/Keywords.cpp
)
target_link_libraries(${PROJECT_TEST_NAME}
  ${PROJECT_NAME_STR}
  ${GTEST_BOTH_LIBRARIES}
  ${REQ_LLVM_LIBRARIES}
)

add_test(${PROJECT_TEST_NAME} ${PROJECT_TEST_NAME})

