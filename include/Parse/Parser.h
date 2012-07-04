#pragma once

#include "Diagnostic.h"
#include "Lex/Token.h"
#include <iosfwd>
#include <memory>
#include <utility>
#include <vector>

class Lexer; class Module; class Decl; class FunctionDecl; class VariableDecl;
class Stmt; class BlockStmt;

class Parser {
public:
  Parser(Lexer& lex);
  ~Parser();

  std::unique_ptr<Module> ParseModule();
private:
  Lexer& lex_;

  // Diagnoses an error
  std::ostream& diagnose(diag::Kind kind);

  // We expect the given token kind. If it is found, consume it
  // and return true. Otherwise, return false, and do not consume it.
  bool expectAndConsume(Token::Kind kind);
  // We expect an identifier. If it is found, write it to tok and return true.
  // Otherwise, tok is unmodified, and return false.
  bool expectAndConsumeIdentifier(Token* tok);
  // Ignores tokens until the given token kind is found, leaving this token
  // in the buffer. If EOF is hit first, returns false.
  bool ignoreTokensUntil(Token::Kind kind);

  ////////////////////////////////////////
  // Decl
  ////////////////////////////////////////
  std::unique_ptr<Decl> parseDecl();
  std::unique_ptr<FunctionDecl> parseFunctionDecl();
  
  // Parses a function argument list, placing it into args.
  // On error, returns false.
  typedef std::pair<Token, Token> function_arg_pair;
  typedef std::vector<function_arg_pair> function_arg_list;
  bool parseFunctionArgList(function_arg_list* args);
  
  // Parses a function argument pair, placing it into arg.
  // If none is found (not necessarily an error), returns false.
  bool parseFunctionArgPair(function_arg_pair* arg);

  std::unique_ptr<VariableDecl> parseVariableDecl();

  ////////////////////////////////////////
  // Stmt
  ////////////////////////////////////////
  std::unique_ptr<Stmt> parseStmt();
  std::unique_ptr<BlockStmt> parseBlockStmt();
};

