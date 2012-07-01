#include <iostream>
#include "Lex/Lexer.h"
#include "llvm/Support/CommandLine.h"
#include <fstream>
#include <streambuf>
#include <string>
namespace cl = llvm::cl;
using namespace std;


// Reads an entire file into a string.
// This will throw ios_base::failure if there's an error.
static string ReadFile(const string& filename) {
  ifstream file;
  file.exceptions(ios_base::badbit | ios_base::failbit);
  file.open(filename);
  return string(istreambuf_iterator<char>(file),
                istreambuf_iterator<char>());
}

// Throws ios_base::failure if there's an error reading the file
static int LexAndDump(const string& filename, ostream& stream) {
  Lexer lex(ReadFile(filename));

  while (lex.GetCurToken().GetKind() != Token::eof) {
    auto tok = lex.ConsumeCurToken();

    switch (tok.GetKind()) {
      case Token::kw_func:
        stream << "func";
        break;
      case Token::kw_var:
        stream << "var";
        break;
      case Token::ident:
        stream << "IDENT(" << tok.GetIdentifier().data() << ")";
        break;
      case Token::int_literal:
        stream << "INT_LITERAL("
               << tok.GetIntLiteral().toString(10, /*signed*/true)
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
      case Token::assign:
        stream << "=";
        break;
      case Token::minus:
        stream << "-";
        break;
      case Token::plus:
        stream << "+";
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

cl::opt<string> input_filename(cl::Positional, cl::desc("<input file>"),
                               cl::Required);
cl::opt<bool> only_lex("only-lex",
                       cl::desc("Run the input file through the lexer and "
                                "output the tokens"));
int main(int argc, char* argv[]) {
  cl::ParseCommandLineOptions(argc, argv);

  if (only_lex) {
    try {
      return LexAndDump(input_filename, cout);
    } catch (const ios_base::failure& e) {
      cerr << "Error reading from \"" << input_filename << "\"" << endl;
      return 1;
    }
  }

  return 0;
}

