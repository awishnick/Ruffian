#include "AST/ASTPrinter.h"
#include "AST/Decl.h"
#include "AST/Module.h"
#include "AST/Stmt.h"
#include "llvm/Support/ErrorHandling.h"
#include <algorithm>
#include <iostream>
#include <sstream>
using namespace std;
using llvm::StringRef;

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
      return derived().TraveseVariableDecl(child);
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
      derived().VisitStmt(child.get());
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
      derived().VisitVariableDecl(arg.get());
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
        parent_raii parent(this, sub_node_name+":args");
        for (auto& arg : decl->args_range()) {
          VisitVariableDecl(arg.get());
        }
      }

      // Output body.
      if (decl->GetBody()) {
        parent_raii parent(this, sub_node_name+":body");
        VisitBlockStmt(decl->GetBody());
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
      
      print_variable_decl(node_name,
                          decl->GetName().GetIdentifier(),
                          decl->GetType().GetIdentifier());
      add_child(node_name);
      return true;
    }

    bool VisitBlockStmt(BlockStmt* stmt) {
      stringstream node_name_stm;
      node_name_stm << "BlockStmt" << make_unique_index();
      string sub_node_name = node_name_stm.str();
      string node_name = make_node_name(sub_node_name);

      output_ << node_name
              << "[label=\"" << sub_node_name << "\"];\n";

      add_child(node_name);
      return true;
    }
  private:
    ostream& output_;
    string parent_;
    unsigned unique_index_;

    unsigned make_unique_index() {
      return unique_index_++;
    }

    string make_node_name(StringRef child_name) const {
      string node_name = parent_ + '_' + child_name.data();
      replace(node_name.begin(), node_name.end(), ':', '_');
      return node_name;
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
        instance_->parent_ += '_';
        instance_->parent_ += new_parent;
      }
      ~parent_raii() {
        instance_->parent_ = old_parent_;
      }
    private:
        ASTPrinterVisitor* instance_;
        string old_parent_;
    };

    void print_variable_decl(StringRef node_name, StringRef name,
                             StringRef type) {
      output_ << node_name.data()
              << " [shape=record,label=\""
              << "{VariableDecl|{"
              << name.data()
              << "|" << type.data()
              << "}}\"];\n";

    }
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

