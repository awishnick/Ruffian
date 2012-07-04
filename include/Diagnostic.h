#pragma once

namespace diag {
  enum Kind {
#define DIAG(ident, str) ident,
#include "Diagnostics.inc"
    NUM_KINDS
  };
}

