/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ART_RUNTIME_DEX_FILE_VERIFIER_H_
#define ART_RUNTIME_DEX_FILE_VERIFIER_H_

#include "dex_file.h"
#include "safe_map.h"

namespace art {

class DexFileVerifier {
 public:
  static bool Verify(const DexFile* dex_file, const byte* begin, size_t size,
                     const char* location, std::string* error_msg);

  const std::string& FailureReason() const {
    return failure_reason_;
  }

 private:
  DexFileVerifier(const DexFile* dex_file, const byte* begin, size_t size, const char* location)
      : dex_file_(dex_file), begin_(begin), size_(size), location_(location),
        header_(&dex_file->GetHeader()), ptr_(NULL), previous_item_(NULL)  {
  }

  bool Verify();

  bool CheckShortyDescriptorMatch(char shorty_char, const char* descriptor, bool is_return_type);
  bool CheckPointerRange(const void* start, const void* end, const char* label);
  bool CheckListSize(const void* start, uint32_t count, uint32_t element_size, const char* label);
  bool CheckIndex(uint32_t field, uint32_t limit, const char* label);

  bool CheckHeader();
  bool CheckMap();

  uint32_t ReadUnsignedLittleEndian(uint32_t size);
  bool CheckAndGetHandlerOffsets(const DexFile::CodeItem* code_item,
                                 uint32_t* handler_offsets, uint32_t handlers_size);
  bool CheckClassDataItemField(uint32_t idx, uint32_t access_flags, bool expect_static);
  bool CheckClassDataItemMethod(uint32_t idx, uint32_t access_flags, uint32_t code_offset,
                                bool expect_direct);
  bool CheckPadding(size_t offset, uint32_t aligned_offset);
  bool CheckEncodedValue();
  bool CheckEncodedArray();
  bool CheckEncodedAnnotation();

  bool CheckIntraClassDataItem();
  bool CheckIntraCodeItem();
  bool CheckIntraStringDataItem();
  bool CheckIntraDebugInfoItem();
  bool CheckIntraAnnotationItem();
  bool CheckIntraAnnotationsDirectoryItem();

  bool CheckIntraSectionIterate(size_t offset, uint32_t count, uint16_t type);
  bool CheckIntraIdSection(size_t offset, uint32_t count, uint16_t type);
  bool CheckIntraDataSection(size_t offset, uint32_t count, uint16_t type);
  bool CheckIntraSection();

  bool CheckOffsetToTypeMap(size_t offset, uint16_t type);
  uint16_t FindFirstClassDataDefiner(const byte* ptr) const;
  uint16_t FindFirstAnnotationsDirectoryDefiner(const byte* ptr) const;

  bool CheckInterStringIdItem();
  bool CheckInterTypeIdItem();
  bool CheckInterProtoIdItem();
  bool CheckInterFieldIdItem();
  bool CheckInterMethodIdItem();
  bool CheckInterClassDefItem();
  bool CheckInterAnnotationSetRefList();
  bool CheckInterAnnotationSetItem();
  bool CheckInterClassDataItem();
  bool CheckInterAnnotationsDirectoryItem();

  bool CheckInterSectionIterate(size_t offset, uint32_t count, uint16_t type);
  bool CheckInterSection();

  void ErrorStringPrintf(const char* fmt, ...)
      __attribute__((__format__(__printf__, 2, 3))) COLD_ATTR;

  const DexFile* const dex_file_;
  const byte* const begin_;
  const size_t size_;
  const char* const location_;
  const DexFile::Header* const header_;

  SafeMap<uint32_t, uint16_t> offset_to_type_map_;
  const byte* ptr_;
  const void* previous_item_;

  std::string failure_reason_;
};

}  // namespace art

#endif  // ART_RUNTIME_DEX_FILE_VERIFIER_H_
