#include "SourceManager.h"
#include "SourceLocation.h"
#include "Support/make_unique.h"
#include <cassert>
#include <fstream>
#include <istream>
#include <memory>
#include <sstream>
#include <utility>
using namespace std;

struct SourceManager::Impl {
  istream* stream;
  unique_ptr<istream> own_stream;
};

SourceManager::SourceManager()
  : impl_(new Impl)
{
}

void SourceManager::SetMainFileFromString(string str) {
  auto stream = make_unique<stringstream>();
  stream->str(move(str));
  impl_->own_stream = move(stream);
  impl_->stream = impl_->own_stream.get();
}

void SourceManager::SetMainFileFromFile(const string& filename) {
  auto stream = make_unique<ifstream>();
  stream->exceptions(ios_base::badbit | ios_base::failbit);
  stream->open(filename);
  impl_->own_stream = move(stream);
  impl_->stream = impl_->own_stream.get();
}

void SourceManager::SetMainFileFromStream(istream& stream) {
  impl_->stream = &stream;
  impl_->own_stream.reset();
}

istream& SourceManager::GetStream() {
  return *impl_->stream;
}

const istream& SourceManager::GetStream() const {
  return *impl_->stream;
}

SourceLocation SourceManager::GetSourceLocationInStream() const {
  auto pos = const_cast<istream&>(GetStream()).tellg();
  SourceLocation loc;
  loc.loc_ = pos;
  return loc;
}

SourceManager::~SourceManager() {
}

