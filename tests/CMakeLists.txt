find_package(GTest REQUIRED)

set(PROJECT_TEST_NAME "${PROJECT_NAME_STR}_tests")
include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})

add_executable(${PROJECT_TEST_NAME}
  main.cpp
  Lex/Keywords.cpp
  Lex/Literals.cpp
)
target_link_libraries(${PROJECT_TEST_NAME}
  ${PROJECT_NAME_STR}
  ${GTEST_BOTH_LIBRARIES}
  ${REQ_LLVM_LIBRARIES}
)

add_test(${PROJECT_TEST_NAME} ${PROJECT_TEST_NAME})

set(FILE_CHECK_PATH "${LLVM_ROOT}/bin/FileCheck")
set(DRIVER_PATH "${CMAKE_BINARY_DIR}/src/driver")
set(TESTS_PATH "${CMAKE_SOURCE_DIR}/tests")
set(TESTER_PATH "${TESTS_PATH}/run_file_check.sh")
add_test(lex_filecheck
  ${TESTER_PATH} ${DRIVER_PATH} ${FILE_CHECK_PATH} ${TESTS_PATH}/Lex/test.rf)
add_test(parse_empty_function_decls
  ${DRIVER_PATH} -dump-ast ${TESTS_PATH}/Parse/empty_function_decls.rf)
add_test(parse_variable_decl
  ${DRIVER_PATH} -dump-ast ${TESTS_PATH}/Parse/variable_decl.rf)
add_test(parse_variable_initializer
  ${DRIVER_PATH} -dump-ast ${TESTS_PATH}/Parse/variable_initializer.rf)
add_test(parse_function_with_local_vars
  ${DRIVER_PATH} -dump-ast ${TESTS_PATH}/Parse/function_with_local_vars.rf)
add_test(parse_unary_op
  ${DRIVER_PATH} -dump-ast ${TESTS_PATH}/Parse/unary_op.rf)
