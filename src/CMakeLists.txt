add_library(ruffian STATIC
  AST/ASTPrinter.cpp
  AST/Decl.cpp
  AST/Module.cpp
  Lex/Lexer.cpp
  Lex/Token.cpp
  Parse/Parser.cpp
  Parse/ParseDecl.cpp
  Parse/ParseModule.cpp
  Parse/ParseStmt.cpp
)

add_executable(driver
  Driver/main.cpp
)

include_directories(${COMMON_INCLUDES})

target_link_libraries(driver ${PROJECT_NAME_STR} ${REQ_LLVM_LIBRARIES})

