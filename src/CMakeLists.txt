add_library(ruffian STATIC
  Lex/Lexer.cpp
  Lex/Token.cpp
)

add_executable(driver
  Driver/main.cpp
)

include_directories(${COMMON_INCLUDES})

target_link_libraries(driver ${PROJECT_NAME_STR} ${REQ_LLVM_LIBRARIES})

