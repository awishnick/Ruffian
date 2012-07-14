#pragma once

#include <memory>
#include <istream>
#include <string>

class SourceLocation;

class SourceManager {
public:
  SourceManager();
  ~SourceManager();

  void SetMainFileFromString(std::string str);
  void SetMainFileFromFile(const std::string& filename);
  void SetMainFileFromStream(std::istream& stream);

  std::istream& GetStream();
  const std::istream& GetStream() const;
  SourceLocation GetSourceLocationInStream() const;

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

