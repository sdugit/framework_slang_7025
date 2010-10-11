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

#include "slang_rs_export_element.h"

#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/IdentifierTable.h"

#include "clang/AST/Decl.h"
#include "clang/AST/Type.h"

#include "slang_rs_context.h"
#include "slang_rs_export_type.h"

using namespace slang;

bool RSExportElement::Initialized = false;
RSExportElement::ElementInfoMapTy RSExportElement::ElementInfoMap;

void RSExportElement::Init() {
  if (!Initialized) {
    // Initialize ElementInfoMap
#define USE_ELEMENT_DATA_TYPE
#define USE_ELEMENT_DATA_KIND
#define DEF_ELEMENT(_name, _dk, _dt, _norm, _vsize)     \
    {                                                   \
      ElementInfo *EI = new ElementInfo;                \
      EI->kind = GET_ELEMENT_DATA_KIND(_dk);            \
      EI->type = GET_ELEMENT_DATA_TYPE(_dt);            \
      EI->normalized = _norm;                           \
      EI->vsize = _vsize;                               \
                                                        \
      llvm::StringRef Name(_name);                      \
      ElementInfoMap.insert(                            \
          ElementInfoMapTy::value_type::Create(         \
              Name.begin(),                             \
              Name.end(),                               \
              ElementInfoMap.getAllocator(),            \
              EI));                                     \
    }
#include "slang_rs_export_element_support.inc"

    Initialized = true;
  }
  return;
}

RSExportType *RSExportElement::Create(RSContext *Context,
                                      const clang::Type *T,
                                      const ElementInfo *EI) {
  // Create RSExportType corresponded to the @T first and then verify

  llvm::StringRef TypeName;
  RSExportType *ET = NULL;

  if (!Initialized)
    Init();

  assert(EI != NULL && "Element info not found");

  if (!RSExportType::NormalizeType(T, TypeName))
    return NULL;

  switch (T->getTypeClass()) {
    case clang::Type::Builtin:
    case clang::Type::Pointer: {
      assert(EI->vsize == 1 && "Element not a primitive class (please check "
                               "your macro)");
      RSExportPrimitiveType *EPT =
          RSExportPrimitiveType::Create(Context,
                                        T,
                                        TypeName,
                                        EI->kind,
                                        EI->normalized);
      // Verify
      assert(EI->type == EPT->getType() && "Element has unexpected type");
      ET = EPT;
      break;
    }
    case clang::Type::ExtVector: {
      assert(EI->vsize > 1 && "Element not a vector class (please check your "
                              "macro)");
      RSExportVectorType *EVT =
          RSExportVectorType::Create(Context,
                                     static_cast<clang::ExtVectorType*>(
                                         T->getCanonicalTypeInternal()
                                             .getTypePtr()),
                                     TypeName,
                                     EI->kind,
                                     EI->normalized);
      // Verify
      assert(EI->type == EVT->getType() && "Element has unexpected type");
      assert(EI->vsize == EVT->getNumElement() && "Element has unexpected size "
                                                  "of vector");
      ET = EVT;
      break;
    }
    default: {
      // TODO(zonr): warn that type is not exportable
      fprintf(stderr, "RSExportElement::Create : type '%s' is not exportable\n",
              T->getTypeClassName());
      break;
    }
  }

  return ET;
}

RSExportType *RSExportElement::CreateFromDecl(RSContext *Context,
                                              const clang::DeclaratorDecl *DD) {
  const clang::Type* T = RSExportType::GetTypeOfDecl(DD);
  const clang::Type* CT = GET_CANONICAL_TYPE(T);
  const ElementInfo* EI = NULL;

  // Note: RS element like rs_pixel_rgb elements are either in the type of
  // primitive or vector.
  if ((CT->getTypeClass() != clang::Type::Builtin) &&
      (CT->getTypeClass() != clang::Type::ExtVector)) {
    return RSExportType::Create(Context, T);
  }

  // Following the typedef chain to see whether it's an element name like
  // rs_pixel_rgb or its alias (via typedef).
  while (T != CT) {
    if (T->getTypeClass() != clang::Type::Typedef) {
      break;
    } else {
      const clang::TypedefType *TT = static_cast<const clang::TypedefType*>(T);
      const clang::TypedefDecl *TD = TT->getDecl();
      EI = GetElementInfo(TD->getName());
      if (EI != NULL)
        break;

      T = TD->getUnderlyingType().getTypePtr();
    }
  }

  if (EI == NULL) {
    return RSExportType::Create(Context, T);
  } else {
    return RSExportElement::Create(Context, T, EI);
  }
}

const RSExportElement::ElementInfo *
RSExportElement::GetElementInfo(const llvm::StringRef &Name) {
  if (!Initialized)
    Init();

  ElementInfoMapTy::const_iterator I = ElementInfoMap.find(Name);
  if (I == ElementInfoMap.end())
    return NULL;
  else
    return I->getValue();
}
