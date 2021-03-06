#include "AST/ASTPrinter.h"
#include "AST/Decl.h"
#include "AST/Expr.h"
#include "AST/Module.h"
#include "AST/Stmt.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/ErrorHandling.h"
#include <algorithm>
#include <iostream>
#include <sstream>
using namespace std;
using llvm::StringRef;
using llvm::SmallString;

template <class Derived>
class ASTVisitor {
public:
  // Module
  bool TraverseModule(Module* module) {
    if (!derived().WalkUpFromModule(module)) return false;

    for (auto& decl : module->decls_range()) {
      derived().TraverseDecl(decl.get());
    }

    return true;
  }
  bool VisitModule(Module*) {
    return true;
  }
  bool WalkUpFromModule(Module* module) {
    return derived().VisitModule(module);
  }

  // Stmt
  bool TraverseStmt(Stmt* stmt) {
    if (!derived().WalkUpFromStmt(stmt)) return false;

    if (auto child = dynamic_cast<BlockStmt*>(stmt)) {
      return derived().TraverseBlockStmt(child);
    } else if (auto child = dynamic_cast<Decl*>(stmt)) {
      return derived().TraverseDecl(child);
    } else {
      llvm_unreachable("Unimplemented subclass");
    }
  }
  bool VisitStmt(Stmt*) {
    return true;
  }
  bool WalkUpFromStmt(Stmt* stmt) {
    return derived().VisitStmt(stmt);
  }

  // BlockStmt
  bool TraverseBlockStmt(BlockStmt* stmt) {
    if (!derived().WalkUpFromBlockStmt(stmt)) return false;

    for (auto& child : stmt->stmts_range()) {
      derived().TraverseStmt(child.get());
    }

    return true;
  }
  bool VisitBlockStmt(Stmt*) {
    return true;
  }
  bool WalkUpFromBlockStmt(BlockStmt* stmt) {
    return derived().VisitBlockStmt(stmt);
  }

  // Decl
  bool TraverseDecl(Decl* decl) {
    if (!derived().WalkUpFromDecl(decl)) return false;

    if (auto child = dynamic_cast<VariableDecl*>(decl)) {
      return derived().TraverseVariableDecl(child);
    } else if( auto child = dynamic_cast<FunctionDecl*>(decl)) {
      return derived().TraverseFunctionDecl(child);
    } else {
      llvm_unreachable("Unimplemented subclass");
    }
  }
  bool VisitDecl(Decl*) {
    return true;
  }
  bool WalkUpFromDecl(Decl* decl) {
    if (!derived().WalkUpFromStmt(decl)) return false;
    return derived().VisitDecl(decl);
  }

  // VariableDecl
  bool TraverseVariableDecl(VariableDecl* decl) {
    if (!derived().WalkUpFromVariableDecl(decl)) return false;
    
    if (decl->GetInitializer()) {
      derived().TraverseExpr(decl->GetInitializer());
    }

    return true;
  }
  bool VisitVariableDecl(VariableDecl*) {
    return true;
  }
  bool WalkUpFromVariableDecl(VariableDecl* decl) {
    if (!derived().WalkUpFromDecl(decl)) return false;
    return derived().VisitVariableDecl(decl);
  }

  // FunctionDecl
  bool TraverseFunctionDecl(FunctionDecl* decl) {
    if (!derived().WalkUpFromFunctionDecl(decl)) return false;

    for (auto& arg : decl->args_range()) {
      derived().TraverseVariableDecl(arg.get());
    }

    return true;
  }
  bool VisitFunctionDecl(FunctionDecl*) {
    return true;
  }
  bool WalkUpFromFunctionDecl(FunctionDecl* decl) {
    if (!derived().WalkUpFromDecl(decl)) return false;
    return derived().VisitFunctionDecl(decl);
  }

  // Expr
  bool TraverseExpr(Expr* expr) {
    if (!derived().WalkUpFromExpr(expr)) return false;

    if (auto child = dynamic_cast<IdentifierExpr*>(expr)) {
      return derived().VisitIdentifierExpr(child);
    } else if (auto child = dynamic_cast<NumericLiteral*>(expr)) {
      return derived().VisitNumericLiteral(child);
    } else if (auto child = dynamic_cast<UnaryOpExpr*>(expr)) {
      return derived().VisitUnaryOpExpr(child);
    } else if (auto child = dynamic_cast<BinaryOpExpr*>(expr)) {
      return derived().VisitBinaryOpExpr(child);
    } else if (auto child = dynamic_cast<FunctionCall*>(expr)) {
      return derived().VisitFunctionCall(child);
    } else {
      llvm_unreachable("Unimplemented subclass");
    }
  }
  bool VisitExpr(Expr*) {
    return true;
  }
  bool WalkUpFromExpr(Expr* expr) {
    return derived().VisitExpr(expr);
  }

  // IdentifierExpr
  bool TraverseIdentifierExpr(IdentifierExpr* expr) {
    if (!derived().WalkUpFromIdentifierExpr(expr)) return false;
    return true;
  }
  bool VisitIdentifierExpr(IdentifierExpr*) {
    return true;
  }
  bool WalkUpFromIdentifierExpr(IdentifierExpr* expr) {
    if (!derived().WalkUpFromExpr(expr)) return false;
    return derived().VisitIdentifierExpr(expr);
  }

  // NumericLiteral
  bool TraverseNumericLiteral(NumericLiteral* expr) {
    if (!derived().WalkUpFromNumericLiteral(expr)) return false;
    return true;
  }
  bool VisitNumericLiteral(NumericLiteral*) {
    return true;
  }
  bool WalkUpFromNumericLiteral(NumericLiteral* expr) {
    if (!derived().WalkUpFromExpr(expr)) return false;
    return derived().VisitNumericLiteral(expr);
  }

  // UnaryOpExpr
  bool TraverseUnaryOpExpr(UnaryOpExpr* expr) {
    if (!derived().WalkUpFromUnaryOpExpr(expr)) return false;
    derived().TraverseExpr(expr->GetExpr());
    return true;
  }
  bool VisitUnaryOpExpr(UnaryOpExpr*) {
    return true;
  }
  bool WalkUpFromUnaryOpExpr(UnaryOpExpr* expr) {
    if (!derived().WalkUpFromExpr(expr)) return false;
    return derived().VisitUnaryOpExpr(expr);
  }

  // BinaryOpExpr
  bool TraverseBinaryOpExpr(BinaryOpExpr* expr) {
    if (!derived().WalkUpFromBinaryOpExpr(expr)) return false;
    derived().TraverseExpr(expr->GetLeft());
    derived().TraverseExpr(expr->GetRight());
    return true;
  }
  bool VisitBinaryOpExpr(BinaryOpExpr*) {
    return true;
  }
  bool WalkUpFromBinaryOpExpr(BinaryOpExpr* expr) {
    if (!derived().WalkUpFromExpr(expr)) return false;
    return derived().VisitBinaryOpExpr(expr);
  }

  // FunctionCall
  bool TraverseFunctionCall(FunctionCall* expr) {
    if (!derived().WalkUpFromFunctionCall(expr)) return false;

    for (auto& arg : expr->args_range()) {
      TraverseExpr(arg.get());
    }

    return true;
  }
  bool VisitFunctionCall(FunctionCall*) {
    return true;
  }
  bool WalkUpFromFunctionCall(FunctionCall* expr) {
    if (!derived().WalkUpFromExpr(expr)) return false;
    return derived().VisitFunctionCall(expr);
  }
private:
  Derived& derived() { return static_cast<Derived&>(*this); }
};

namespace {
  class ASTPrinterVisitor : public ASTVisitor<ASTPrinterVisitor> {
  public:
    ASTPrinterVisitor(ostream& output)
      : output_(output)
      , parent_("Module")
      , unique_index_(0)
    {
      output_ << "digraph G {\n";
    }

    ~ASTPrinterVisitor() {
      output_ << "}\n";
    }

    bool VisitFunctionDecl(FunctionDecl* decl) {
      string sub_node_name = string("FunctionDecl_")
                             + decl->GetName().GetIdentifier().data();
      string node_name = make_node_name(sub_node_name);

      output_ << node_name << " [shape=record,label=\""
              << "{FunctionDecl|{<name> "
              << decl->GetName().GetIdentifier().data();
      if (auto return_type = decl->GetReturnType()) {
          output_ << "|" << return_type->GetIdentifier().data();
      }

      if (!decl->args_range().empty()) {
        output_ << "|<args> Args";
      }

      if (decl->GetBody()) {
        output_ << "|<body> Body";
      }

      output_ << "}}\"];\n";
      add_child(node_name);

      // Output arguments.
      {
        parent_raii parent(this, node_name+":args");
        for (auto& arg : decl->args_range()) {
          TraverseVariableDecl(arg.get());
        }
      }

      // Output body.
      if (decl->GetBody()) {
        parent_raii parent(this, node_name+":body");
        TraverseBlockStmt(decl->GetBody());
      }

      // Don't visit any more children, since we've already visited
      // the ones that we want to print.
      return false;
    }

    bool VisitVariableDecl(VariableDecl* decl) {
      stringstream node_name_stm;
      node_name_stm << "VariableDecl_"
                    << decl->GetName().GetIdentifier().data();
      string node_name = make_node_name(node_name_stm.str());
      
      output_ << node_name.data()
              << " [shape=record,label=\""
              << "{VariableDecl|{"
              << decl->GetName().GetIdentifier().data()
              << "|" << decl->GetType().GetIdentifier().data();

      if (decl->GetInitializer()) {
        output_ << "|<init> Init";
      }

      output_ << "}}\"];\n";

      add_child(node_name);

      if (Expr* init = decl->GetInitializer()) {
        parent_raii parent(this, node_name+":init");
        TraverseExpr(init);
      }

      return false;
    }

    bool VisitBlockStmt(BlockStmt* stmt) {
      stringstream node_name_stm;
      node_name_stm << "BlockStmt" << make_unique_index();
      string node_name = make_node_name(node_name_stm.str());

      output_ << node_name
              << "[label=\"BlockStmt\"];\n";
      add_child(node_name);

      // Output each child statement.
      parent_raii parent(this, node_name);
      for (auto& child : stmt->stmts_range()) {
        TraverseStmt(child.get());
      }

      return false;
    }

    bool VisitIdentifierExpr(IdentifierExpr* expr) {
      stringstream node_name_stm;
      node_name_stm << "IdentifierExpr" << make_unique_index();
      string node_name = make_node_name(node_name_stm.str());

      Token ident = expr->GetIdentifier();
      output_ << node_name
              << "[label=\"" << ident.GetIdentifier().data()
              << "\"];\n";
      add_child(node_name);
      
      return false;
    }

    bool VisitNumericLiteral(NumericLiteral* expr) {
      stringstream node_name_stm;
      node_name_stm << "NumericLiteral" << make_unique_index();
      string node_name = make_node_name(node_name_stm.str());

      llvm::APInt value = expr->GetValue().GetIntLiteral();
      output_ << node_name
              << "[label=\"" << value.toString(10, /*signed*/false)
              << "\"];\n";
      add_child(node_name);

      return false;
    }

    bool VisitUnaryOpExpr(UnaryOpExpr* expr) {
      stringstream node_name_stm;
      node_name_stm << "UnaryOpExpr" << make_unique_index();
      string node_name = make_node_name(node_name_stm.str());

      SmallString<8> op;
      switch (expr->GetOp().GetKind()) {
        case Token::minus: op = "-"; break;
        default: llvm_unreachable("Unexpected unary op");
      };
      output_ << node_name
              << "[label=\"" << op.c_str() << "\"];\n";
      add_child(node_name);

      // Visit the child expression.
      parent_raii parent(this, node_name);
      TraverseExpr(expr->GetExpr());

      // We've already visited the child expression.
      return false;
    }

    bool VisitBinaryOpExpr(BinaryOpExpr* expr) {
      stringstream node_name_stm;
      node_name_stm << "BinaryOpExpr" << make_unique_index();
      string node_name = make_node_name(node_name_stm.str());

      SmallString<8> op;
      switch (expr->GetOp().GetKind()) {
        case Token::assign: op = "="; break;
        case Token::plus: op = "+"; break;
        case Token::minus: op = "-"; break;
        case Token::star: op = "*"; break;
        case Token::slash: op = "/"; break;
        default: llvm_unreachable("Unexpected binary op");
      };

      output_ << node_name.data()
              << " [shape=record,label=\""
              << "{BinaryOpExpr|{"
              << "<left> Left"
              << "|" << op.c_str()
              << "|<right> Right}"
              << "}\"];\n";
      add_child(node_name);

      // Output left and right.
      {
        parent_raii parent(this, node_name+":left");
        TraverseExpr(expr->GetLeft());
      }
      {
        parent_raii parent(this, node_name+":right");
        TraverseExpr(expr->GetRight());
      }

      return false;
    }

    bool VisitFunctionCall(FunctionCall* expr) {
      string sub_node_name = string("FunctionCall_")
                             + expr->GetName().GetIdentifier().data();
      string node_name = make_node_name(sub_node_name);

      output_ << node_name << " [shape=record,label=\""
              << "{FunctionCall|{<name> "
              << expr->GetName().GetIdentifier().data();

      if (!expr->args_range().empty()) {
        output_ << "|<args> Args";
      }

      output_ << "}}\"];\n";
      add_child(node_name);

      // Output arguments.
      {
        parent_raii parent(this, node_name+":args");
        for (auto& arg : expr->args_range()) {
          TraverseExpr(arg.get());
        }
      }

      // Don't visit any more children, since we've already visited
      // the ones that we want to print.
      return false;
    }
  private:
    ostream& output_;
    string parent_;
    unsigned unique_index_;

    unsigned make_unique_index() {
      return unique_index_++;
    }

    string make_node_name(StringRef child_name) {
      stringstream name_stm;
      name_stm << child_name.data() << make_unique_index();
      return name_stm.str();
    }

    void add_child(StringRef name) {
      output_ << parent_ << " -> " << name.data() << ";\n";
    }

    class parent_raii {
    public:
      parent_raii(ASTPrinterVisitor* instance, string new_parent)
        : instance_(instance)
        , old_parent_(instance->parent_)
      {
        instance_->parent_ = new_parent;
      }
      ~parent_raii() {
        instance_->parent_ = old_parent_;
      }
    private:
        ASTPrinterVisitor* instance_;
        string old_parent_;
    };

  };
}

ASTPrinter::ASTPrinter(ostream& output)
  : output_(output) {
}

void ASTPrinter::PrintModule(Module* module) {
  ASTPrinterVisitor visitor(output_);
  visitor.TraverseModule(module);
}

ASTPrinter::~ASTPrinter() {}

