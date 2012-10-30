#include <iostream>
#include "AST/ASTPrinter.h"
#include "AST/Module.h"
#include "Lex/Lexer.h"
#include "Parse/Parser.h"
#include "Repl.h"
#include "SourceManager.h"
#include "llvm/Support/CommandLine.h"
#include <fstream>
#include <streambuf>
#include <string>
namespace cl = llvm::cl;
using namespace std;

cl::opt<string> input_filename(cl::Positional, cl::desc("<input file>"),
                               cl::init(""));
cl::opt<bool> only_lex("only-lex",
                       cl::desc("Run the input file through the lexer and "
                                "output the tokens"));
cl::opt<bool> dump_ast("dump-ast",
                       cl::desc("Parse and output the abstract syntax tree"));

// Throws ios_base::failure if there's an error reading the file
static int LexAndDump(istream& input, ostream& stream) {
  input >> noskipws;
  SourceManager sm;
  sm.SetMainFileFromString(string(istreambuf_iterator<char>(input),
                                  istreambuf_iterator<char>()));
  Lexer lex(sm);

  while (lex.GetCurToken().GetKind() != Token::eof) {
    auto tok = lex.ConsumeCurToken();

    switch (tok.GetKind()) {
      case Token::kw_func:
        stream << "func";
        break;
      case Token::kw_var:
        stream << "var";
        break;
      case Token::kw_return:
        stream << "return";
        break;
      case Token::ident:
        stream << "IDENT(" << tok.GetIdentifier().data() << ")";
        break;
      case Token::int_literal:
        stream << "INT_LITERAL("
               << tok.GetIntLiteral().toString(10, /*signed*/false)
               << ")";
        break;
      case Token::lparen:
        stream << "(";
        break;
      case Token::rparen:
        stream << ")";
        break;
      case Token::lbrace:
        stream << "{";
        break;
      case Token::rbrace:
        stream << "}";
        break;
      case Token::lbracket:
        stream << "[";
        break;
      case Token::rbracket:
        stream << "]";
        break;
      case Token::semicolon:
        stream << ";";
        break;
      case Token::comma:
        stream << ",";
        break;
      case Token::assign:
        stream << "=";
        break;
      case Token::minus:
        stream << "-";
        break;
      case Token::plus:
        stream << "+";
        break;
      case Token::star:
        stream << "*";
        break;
      case Token::slash:
        stream << "/";
        break;
      case Token::arrow:
        stream << "->";
        break;
      case Token::unknown:
        stream << "UNKNOWN";
        break;
      default:
        llvm_unreachable("unexpected token kind");
    }
    stream << " ";
  }

  assert(lex.GetCurToken().GetKind() == Token::eof);
  stream << "EOF\n";

  return 0;
}

int RunOnlyLex() {
  try {
    istream* input;
    ifstream file;
    if (!input_filename.empty()) {
      file.exceptions(ios_base::badbit | ios_base::failbit);
      file.open(input_filename);
      input = &file;
    } else {
      input = &cin;
    }
    return LexAndDump(*input, cout);
  } catch (const ios_base::failure& e) {
    cerr << "Error reading from \"" << input_filename << "\": ";
    cerr << e.what() << endl;

    return 1;
  }
  return 0;
}

int RunDumpAST() {
  try {
    SourceManager sm;
    if (!input_filename.empty()) {
      sm.SetMainFileFromFile(input_filename);
    } else {
      sm.SetMainFileFromStream(cin);
    }

    Lexer lex(sm);

    Parser parse(lex);
    auto module = parse.ParseModule();

    if (!module) {
      cerr << "Parse error, no module was created.\n";
      return 1;
    }
    
    ASTPrinter printer(cout);
    printer.PrintModule(module.get());
  } catch (const ios_base::failure& e) {
    cerr << "Error reading from \"" << input_filename << "\"" << endl;
    return 1;
  }
  return 0;
}
               
int main(int argc, char* argv[]) {
  cl::ParseCommandLineOptions(argc, argv);
    
  if (input_filename.empty()) {
    Repl repl;
    return repl.Run(std::cin, std::cout, std::cerr);
  }
  
  if (only_lex) {
    return RunOnlyLex();
  }

  if (dump_ast) {
    return RunDumpAST();
  }

  return 0;
}

