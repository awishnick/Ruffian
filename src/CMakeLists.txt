add_library(ruffian STATIC
  AST/ASTPrinter.cpp
  AST/Decl.cpp
  AST/Expr.cpp
  AST/Module.cpp
  AST/Stmt.cpp
  Lex/Lexer.cpp
  Lex/Token.cpp
  Parse/Parser.cpp
  Parse/ParseDecl.cpp
  Parse/ParseExpr.cpp
  Parse/ParseModule.cpp
  Parse/ParseStmt.cpp
  SourceManager.cpp
)

add_executable(driver
  Driver/main.cpp
  Driver/Repl.cpp
)

include_directories(${COMMON_INCLUDES})

target_link_libraries(driver ${PROJECT_NAME_STR} ${REQ_LLVM_LIBRARIES})

