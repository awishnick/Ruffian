#pragma once

#include <cstdint>

class SourceManager;

class SourceLocation {
public:
  friend class SourceManager;
private:
  // Opaque offset into a file
  uint32_t loc_;
};

