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

#ifndef _SLANG_COMPILER_RS_EXPORT_VAR_H
#define _SLANG_COMPILER_RS_EXPORT_VAR_H

#include <string>

#include "llvm/ADT/StringRef.h"

#include "clang/AST/Decl.h"
#include "clang/AST/Expr.h"

#include "slang_rs_exportable.h"

namespace clang {
  class APValue;
}

namespace slang {
  class RSContext;
  class RSExportType;

class RSExportVar : public RSExportable {
  friend class RSContext;
 private:
  RSContext *mContext;
  std::string mName;
  const RSExportType *mET;
  bool mIsConst;

  clang::Expr::EvalResult mInit;

  RSExportVar(RSContext *Context,
              const clang::VarDecl *VD,
              const RSExportType *ET);

 public:
  inline const std::string &getName() const { return mName; }
  inline const RSExportType *getType() const { return mET; }
  inline RSContext *getRSContext() const { return mContext; }
  inline bool isConst() const { return mIsConst; }

  inline const clang::APValue &getInit() const { return mInit.Val; }
};  // RSExportVar

}   // namespace slang

#endif  // _SLANG_COMPILER_RS_EXPORT_VAR_H
