/*
 * Copyright 2017 WebAssembly Community Group participants
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

#include "src/opcode.h"

#include <algorithm>

#include "src/feature.h"

namespace wabt {

// static
Opcode::Info Opcode::infos_[] = {
#define WABT_OPCODE(rtype, type1, type2, type3, mem_size, prefix, code, Name, \
                    text)                                                     \
  {text,        Type::rtype, Type::type1,                                     \
   Type::type2, Type::type3, mem_size,                                        \
   prefix,      code,        PrefixCode(prefix, code)},
#include "src/opcode.def"
#undef WABT_OPCODE
};

#define WABT_OPCODE(rtype, type1, type2, type3, mem_size, prefix, code, Name, \
                    text)                                                     \
  /* static */ Opcode Opcode::Name##_Opcode(Opcode::Name);
#include "src/opcode.def"
#undef WABT_OPCODE

// static
Opcode Opcode::FromCode(uint32_t code) {
  return FromCode(0, code);
}

// static
Opcode Opcode::FromCode(uint8_t prefix, uint32_t code) {
  uint32_t prefix_code = PrefixCode(prefix, code);
  auto begin = infos_;
  auto end = infos_ + WABT_ARRAY_SIZE(infos_);
  auto iter = std::lower_bound(begin, end, prefix_code,
                               [](const Info& info, uint32_t prefix_code) {
                                 return info.prefix_code < prefix_code;
                               });
  if (iter == end || iter->prefix_code != prefix_code) {
    return Opcode(EncodeInvalidOpcode(prefix_code));
  }

  return Opcode(static_cast<Enum>(iter - infos_));
}

Opcode::Info Opcode::GetInfo() const {
  if (enum_ < Invalid) {
    return infos_[enum_];
  }

  uint8_t prefix;
  uint32_t code;
  DecodeInvalidOpcode(enum_, &prefix, &code);
  const Info invalid_info = {
      "<invalid>", Type::Void, Type::Void,
      Type::Void,  Type::Void, 0,
      prefix,      code,       PrefixCode(prefix, code),
  };
  return invalid_info;
}

bool Opcode::IsNaturallyAligned(Address alignment) const {
  Address opcode_align = GetMemorySize();
  return alignment == WABT_USE_NATURAL_ALIGNMENT || alignment == opcode_align;
}

Address Opcode::GetAlignment(Address alignment) const {
  if (alignment == WABT_USE_NATURAL_ALIGNMENT) {
    return GetMemorySize();
  }
  return alignment;
}

bool Opcode::IsEnabled(const Features& features) const {
  switch (enum_) {
    case Opcode::Try:
    case Opcode::Catch:
    case Opcode::Throw:
    case Opcode::Rethrow:
    case Opcode::CatchAll:
      return features.exceptions_enabled();

    case Opcode::I32TruncSSatF32:
    case Opcode::I32TruncUSatF32:
    case Opcode::I32TruncSSatF64:
    case Opcode::I32TruncUSatF64:
    case Opcode::I64TruncSSatF32:
    case Opcode::I64TruncUSatF32:
    case Opcode::I64TruncSSatF64:
    case Opcode::I64TruncUSatF64:
      return features.sat_float_to_int_enabled();

    case Opcode::I32Extend8S:
    case Opcode::I32Extend16S:
    case Opcode::I64Extend8S:
    case Opcode::I64Extend16S:
    case Opcode::I64Extend32S:
      return features.sign_extension_enabled();

    case Opcode::AtomicWake:
    case Opcode::I32AtomicWait:
    case Opcode::I64AtomicWait:
    case Opcode::I32AtomicLoad:
    case Opcode::I64AtomicLoad:
    case Opcode::I32AtomicLoad8U:
    case Opcode::I32AtomicLoad16U:
    case Opcode::I64AtomicLoad8U:
    case Opcode::I64AtomicLoad16U:
    case Opcode::I64AtomicLoad32U:
    case Opcode::I32AtomicStore:
    case Opcode::I64AtomicStore:
    case Opcode::I32AtomicStore8:
    case Opcode::I32AtomicStore16:
    case Opcode::I64AtomicStore8:
    case Opcode::I64AtomicStore16:
    case Opcode::I64AtomicStore32:
    case Opcode::I32AtomicRmwAdd:
    case Opcode::I64AtomicRmwAdd:
    case Opcode::I32AtomicRmw8UAdd:
    case Opcode::I32AtomicRmw16UAdd:
    case Opcode::I64AtomicRmw8UAdd:
    case Opcode::I64AtomicRmw16UAdd:
    case Opcode::I64AtomicRmw32UAdd:
    case Opcode::I32AtomicRmwSub:
    case Opcode::I64AtomicRmwSub:
    case Opcode::I32AtomicRmw8USub:
    case Opcode::I32AtomicRmw16USub:
    case Opcode::I64AtomicRmw8USub:
    case Opcode::I64AtomicRmw16USub:
    case Opcode::I64AtomicRmw32USub:
    case Opcode::I32AtomicRmwAnd:
    case Opcode::I64AtomicRmwAnd:
    case Opcode::I32AtomicRmw8UAnd:
    case Opcode::I32AtomicRmw16UAnd:
    case Opcode::I64AtomicRmw8UAnd:
    case Opcode::I64AtomicRmw16UAnd:
    case Opcode::I64AtomicRmw32UAnd:
    case Opcode::I32AtomicRmwOr:
    case Opcode::I64AtomicRmwOr:
    case Opcode::I32AtomicRmw8UOr:
    case Opcode::I32AtomicRmw16UOr:
    case Opcode::I64AtomicRmw8UOr:
    case Opcode::I64AtomicRmw16UOr:
    case Opcode::I64AtomicRmw32UOr:
    case Opcode::I32AtomicRmwXor:
    case Opcode::I64AtomicRmwXor:
    case Opcode::I32AtomicRmw8UXor:
    case Opcode::I32AtomicRmw16UXor:
    case Opcode::I64AtomicRmw8UXor:
    case Opcode::I64AtomicRmw16UXor:
    case Opcode::I64AtomicRmw32UXor:
    case Opcode::I32AtomicRmwXchg:
    case Opcode::I64AtomicRmwXchg:
    case Opcode::I32AtomicRmw8UXchg:
    case Opcode::I32AtomicRmw16UXchg:
    case Opcode::I64AtomicRmw8UXchg:
    case Opcode::I64AtomicRmw16UXchg:
    case Opcode::I64AtomicRmw32UXchg:
    case Opcode::I32AtomicRmwCmpxchg:
    case Opcode::I64AtomicRmwCmpxchg:
    case Opcode::I32AtomicRmw8UCmpxchg:
    case Opcode::I32AtomicRmw16UCmpxchg:
    case Opcode::I64AtomicRmw8UCmpxchg:
    case Opcode::I64AtomicRmw16UCmpxchg:
    case Opcode::I64AtomicRmw32UCmpxchg:
      return features.threads_enabled();

    case Opcode::V128Const:
    case Opcode::I8X16Splat:
    case Opcode::I16X8Splat:
    case Opcode::I32X4Splat:
    case Opcode::I64X2Splat:
    case Opcode::F32X4Splat:
    case Opcode::F64X2Splat:
    case Opcode::I8X16Add:
    case Opcode::I16X8Add:
    case Opcode::I32X4Add:
    case Opcode::I64X2Add:
    case Opcode::I8X16Sub:
    case Opcode::I16X8Sub:
    case Opcode::I32X4Sub:
    case Opcode::I64X2Sub:
    case Opcode::I8X16Mul:
    case Opcode::I16X8Mul:
    case Opcode::I32X4Mul:
    case Opcode::I8X16Neg:
    case Opcode::I16X8Neg:
    case Opcode::I32X4Neg:
    case Opcode::I64X2Neg:
    case Opcode::I8X16AddSaturateS:
    case Opcode::I8X16AddSaturateU:
    case Opcode::I16X8AddSaturateS:
    case Opcode::I16X8AddSaturateU:
    case Opcode::I8X16SubSaturateS:
    case Opcode::I8X16SubSaturateU:
    case Opcode::I16X8SubSaturateS:
    case Opcode::I16X8SubSaturateU:
    case Opcode::I8X16Shl:
    case Opcode::I16X8Shl:
    case Opcode::I32X4Shl:
    case Opcode::I64X2Shl:
      return features.simd_enabled();

    // Interpreter opcodes are never "enabled".
    case Opcode::InterpAlloca:
    case Opcode::InterpBrUnless:
    case Opcode::InterpCallHost:
    case Opcode::InterpData:
    case Opcode::InterpDropKeep:
      return false;

    default:
      return true;
  }
}

}  // end anonymous namespace
