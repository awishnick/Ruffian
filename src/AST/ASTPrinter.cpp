#include "AST/ASTPrinter.h"
#include "AST/Decl.h"
#include "AST/Module.h"
#include "llvm/Support/ErrorHandling.h"
#include <iostream>
#include <sstream>
using namespace std;

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
  bool VisitModule(Module* module) {
    return true;
  }
  bool WalkUpFromModule(Module* module) {
    return derived().VisitModule(module);
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
  bool VisitDecl(Decl* decl) {
    return true;
  }
  bool WalkUpFromDecl(Decl* decl) {
    return derived().VisitDecl(decl);
  }

  // VariableDecl
  bool TraverseVariableDecl(VariableDecl* decl) {
    if (!derived().WalkUpFromVariableDecl(decl)) return false;
    return true;
  }
  bool VisitVariableDecl(VariableDecl* decl) {
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
  bool VisitFunctionDecl(FunctionDecl* decl) {
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
    {
      output_ << "digraph G {\n";
    }

    ~ASTPrinterVisitor() {
      output_ << "}\n";
    }

    bool VisitFunctionDecl(FunctionDecl* decl) {
      string node_name = string("FunctionDecl_")
                         + decl->GetName().GetIdentifier().data();
      output_ << node_name << " [shape=record,label=\""
              << "{FunctionDecl|{<name> "
              << decl->GetName().GetIdentifier().data();
      if (auto return_type = decl->GetReturnType()) {
          output_ << "|" << return_type->GetIdentifier().data();
      }

      if (!decl->args_range().empty()) {
        output_ << "|<args> Args";
      }

      output_ << "}}\"];\n";
      add_child(node_name);

      // Output arguments.
      {
        parent_raii parent(this, node_name+":args");
        unsigned arg_index = 0;
        for (auto& arg : decl->args_range()) {
          // Format a unique name for the node.
          stringstream arg_node_name;
          arg_node_name << "VariableDecl_" << node_name << "_arg" << arg_index;

          output_ << arg_node_name.str()
                  << " [shape=record,label=\""
                  << "{VariableDecl|{"
                  << arg->GetName().GetIdentifier().data()
                  << "|" << arg->GetType().GetIdentifier().data()
                  << "}}\"];\n";
          add_child(arg_node_name.str());

          ++arg_index;
        }
      }

      // Don't visit any more children, since we've already visited
      // the ones that we want to print.
      return false;
    }

    bool VisitVariableDecl(VariableDecl* decl) {
      add_child(decl->GetName().GetIdentifier());
      return true;
    }
  private:
    ostream& output_;
    string parent_;

    void add_child(llvm::StringRef name) {
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

