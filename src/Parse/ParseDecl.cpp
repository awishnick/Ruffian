#include "Parse/Parser.h"
#include "AST/Decl.h"
#include "AST/Expr.h"
#include "AST/Stmt.h"
#include "Lex/Lexer.h"
#include "Support/make_unique.h"
#include "llvm/Support/ErrorHandling.h"
#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>
using namespace std;
using llvm::StringRef;

unique_ptr<Decl> Parser::parseTopLevelDecl() {
  if (lex_.GetCurToken().GetKind() == Token::kw_func) {
    return parseFunctionDecl();
  }

  if (lex_.GetCurToken().GetKind() == Token::kw_var) {
    return parseVariableDecl();
  }

  diagnose(diag::expected_top_level_decl);
  return nullptr;
}

unique_ptr<FunctionDecl> Parser::parseFunctionDecl() {
  /* function_decl
      : func identifier ( function_arg_list ) -> identifier { stmt }
      | func identifier ( ) -> identifier { stmt }
      : func identifier ( function_arg_list ) { stmt }
      | func identifier ( ) { stmt }
   */

  if (!expectAndConsume(Token::kw_func)) {
    llvm_unreachable("This should only be called with the 'func' keyword");
  }

  // Get the name
  Token name_tok;
  if (!expectAndConsumeIdentifier(&name_tok)) {
    diagnose(diag::expected_identifier_after_func);
    return nullptr;
  }

  // Expect (
  if (!expectAndConsume(Token::lparen)) {
    // TODO: recover by assuming no args and searching for the
    // arrow, return type, or left bracket.
    diagnose(diag::expected_lparen_to_begin_function_args);
    return nullptr;
  }

  // Parse the argument list if the next token is not an rparen.
  function_arg_list args;
  if (lex_.GetCurToken().GetKind() != Token::rparen
      && !parseFunctionArgList(&args)) {
    diagnose(diag::could_not_parse_function_argument_declaration);
    // Recover by pretending there are no arguments, and finding the
    // matching rparen.
    args.clear();
    if (!ignoreTokensUntil(Token::rparen)) {
      diagnose(diag::could_not_recover_by_finding_rparen);
      return nullptr;
    }
  }

  // Expect )
  if (!expectAndConsume(Token::rparen)) {
    diagnose(diag::expected_rparen_to_end_function_args);
    // Recover by consuming up to the arrow
    if (!ignoreTokensUntil(Token::arrow)) {
      return nullptr;
    }
  }


  // If there's an arrow, parse the return type.
  unique_ptr<Token> maybe_return_type;
  if (lex_.GetCurToken().GetKind() == Token::arrow) {
    lex_.ConsumeCurToken();

    // Get the return type
    Token ret_type_tok;
    if (!expectAndConsumeIdentifier(&ret_type_tok)) {
      // TODO: recovery
      diagnose(diag::expected_function_return_type);
      return nullptr;
    }
    maybe_return_type.reset(new Token(ret_type_tok));
  }

  // Expect {
  if (!expectAndConsume(Token::lbrace)) {
    diagnose(diag::expected_lbrace_for_function_body);
    // For now, no recovery. Once a diagnostic is added, we could
    // recover by pretending we saw the brace.
    return nullptr;
  }

  // Now build the decl.
  vector<unique_ptr<VariableDecl>> arg_decls;
  arg_decls.reserve(args.size());
  transform(args.begin(), args.end(),
            back_inserter(arg_decls),
            [](const function_arg_pair& arg_pair) {
              return make_unique<VariableDecl>(arg_pair.first,
                                               arg_pair.second);
            });


  auto body = parseStmt();
#if 0
  if (!body) {
    // TODO: error
    return nullptr;
  }
#endif

  // Expect }
  if (!expectAndConsume(Token::rbrace)) {
    diagnose(diag::expected_rbrace_after_function_body);
    // TODO: Recovery
    return nullptr;
  }

  if (maybe_return_type) {
    return make_unique<FunctionDecl>(name_tok,
                                     *maybe_return_type,
                                     move(arg_decls),
                                     move(body));
  } else {
    return make_unique<FunctionDecl>(name_tok,
                                     move(arg_decls),
                                     move(body));
  }
  
}

bool Parser::parseFunctionArgList(function_arg_list* args) {
  /* function_arg_list
      : function_arg_pair , function_arg_list
     function_arg_pair
      : identifier identifier
   */

  // Read each function_arg_pair
  for (;;) {
    function_arg_pair arg;
    if (!parseFunctionArgPair(&arg)) {
      // TODO: error
      // TODO: recovery? Could emit an error but not return failure.
      return false;
    }
    args->push_back(move(arg));

    // If we've got a comma, eat it. Otherwise, we're done.
    if (lex_.GetCurToken().GetKind() == Token::comma) {
      lex_.ConsumeCurToken();
    } else {
      break;
    }
  }

  return true;
}

bool Parser::parseFunctionArgPair(function_arg_pair* arg) {
  Token type;
  if (!expectAndConsumeIdentifier(&type)) {
    diagnose(diag::expected_type_ident_in_func_arg_list);
    return false;
  }
  Token name;
  if (!expectAndConsumeIdentifier(&name)) {
    diagnose(diag::expected_name_ident_in_func_arg_list);
    return false;
  }

  *arg = make_pair(type, name);
  return true;
}

unique_ptr<VariableDecl> Parser::parseVariableDecl() {
  /* variable_decl
      : var identifier identifier ;
  */

  if (!expectAndConsume(Token::kw_var)) {
    llvm_unreachable("This should only be called with the 'var' keyword");
  }

  // Get the name
  Token name_tok;
  if (!expectAndConsumeIdentifier(&name_tok)) {
    diagnose(diag::expected_identifier_after_var);
    return nullptr;
  }

  // Get the type
  Token type_tok;
  if (!expectAndConsumeIdentifier(&type_tok)) {
    diagnose(diag::expected_type_in_var_decl);
    return nullptr;
  }

  // Expect a semicolon. Recover by pretending it was found.
  if (!expectAndConsume(Token::semicolon)) {
    diagnose(diag::expected_semicolon_after_var_decl);
  }

  return make_unique<VariableDecl>(name_tok, type_tok);
}

