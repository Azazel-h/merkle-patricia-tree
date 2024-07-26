#ifndef SILKWORM_BYTES_TO_STRING_HPP
#define SILKWORM_BYTES_TO_STRING_HPP

#ifdef __cplusplus

/*
   Copyright 2022 The Silkworm Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

// Utilities for type casting

#include <string_view>

#include "base.hpp"
#include "bytes.hpp"

namespace silkworm {

// Cast between pointers to char and unsigned char (i.e. uint8_t)
    inline char* byte_ptr_cast(uint8_t* ptr) { return reinterpret_cast<char*>(ptr); }
    inline const char* byte_ptr_cast(const uint8_t* ptr) { return reinterpret_cast<const char*>(ptr); }
    inline uint8_t* byte_ptr_cast(char* ptr) { return reinterpret_cast<uint8_t*>(ptr); }
    inline const uint8_t* byte_ptr_cast(const char* ptr) { return reinterpret_cast<const uint8_t*>(ptr); }

    inline Bytes string_to_bytes(const std::string& s) { return {s.begin(), s.end()}; }
    inline ByteView string_view_to_byte_view(std::string_view v) { return {byte_ptr_cast(v.data()), v.length()}; }

    template <std::size_t Size>
    ByteView array_to_byte_view(const std::array<unsigned char, Size>& array) {
        return ByteView{reinterpret_cast<const uint8_t*>(array.data()), Size};
    }

    inline std::string_view byte_view_to_string_view(ByteView v) { return {byte_ptr_cast(v.data()), v.length()}; }

}  // namespace silkworm

#endif // __cplusplus

// C interface
#ifdef __cplusplus
extern "C" {
#endif

#include "bytes.hpp"

// Convert a string to silkworm_Bytes
silkworm_Bytes silkworm_string_to_bytes(const char* str, size_t length);

// Convert a string view to silkworm_ByteView
silkworm_ByteView silkworm_string_view_to_byte_view(const char* str, size_t length);

// Convert an array to silkworm_ByteView
silkworm_ByteView silkworm_array_to_byte_view(const unsigned char* array, size_t size);

// Convert a silkworm_ByteView to a string view (returned as a struct with pointer and length)
typedef struct {
    const char* data;
    size_t length;
} silkworm_StringView;

silkworm_StringView silkworm_byte_view_to_string_view(silkworm_ByteView view);

#ifdef __cplusplus
}
#endif

#endif // SILKWORM_BYTES_TO_STRING_HPP
