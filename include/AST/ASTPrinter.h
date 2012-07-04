#pragma once

#include <iosfwd>
class Module;

class ASTPrinter {
public:
  ASTPrinter(std::ostream& output);
  ~ASTPrinter();

  void PrintModule(Module* module);
private:
  std::ostream& output_;
};

