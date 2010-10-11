/*
 * Copyright 2010, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _SLANG_COMPILER_SLANG_RS_HPP
#define _SLANG_COMPILER_SLANG_RS_HPP

#include "slang.h"

namespace slang {
  class RSContext;

class SlangRS : public Slang {
 private:
  // Context for RenderScript
  RSContext *mRSContext;

  bool mAllowRSPrefix;

 protected:
  virtual void initDiagnostic();
  virtual void initPreprocessor();
  virtual void initASTContext();

  virtual clang::ASTConsumer
  *createBackend(const clang::CodeGenOptions& CodeGenOpts,
                 llvm::raw_ostream *OS,
                 Slang::OutputType OT);


 public:
  static bool IsRSHeaderFile(const char *File);

  SlangRS(const std::string &Triple, const std::string &CPU,
          const std::vector<std::string> &Features);

  // The package name that's really applied will be filled in RealPackageName.
  bool reflectToJava(const std::string &OutputPathBase,
                     const std::string &OutputPackageName,
                     std::string *RealPackageName);

  virtual void reset();

  inline void allowRSPrefix(bool V = true) { mAllowRSPrefix = V; }

  virtual ~SlangRS();
};
}

#endif  // _SLANG_COMPILER_SLANG_RS_HPP
