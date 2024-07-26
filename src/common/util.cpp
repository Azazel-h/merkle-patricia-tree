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


#include <algorithm>
#include <cstdio>
#include <regex>
#include "merkle-patricia-tree/common/util.hpp"
#include "merkle-patricia-tree/common/assert.hpp"

namespace silkworm {

// ASCII -> hex value (0xff means bad [hex] char)
    static constexpr uint8_t kUnhexTable[256] = {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08,
            0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

// ASCII -> hex value << 4 (upper nibble) (0xff means bad [hex] char)
    static constexpr uint8_t kUnhexTable4[256] = {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
            0x80,
            0x90, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    ByteView zeroless_view(ByteView data) {
        const auto is_zero_byte = [](const auto &b) { return b == 0x0; };
        const auto first_nonzero_byte_it{std::ranges::find_if_not(data, is_zero_byte)};
        return data.substr(static_cast<size_t>(std::distance(data.begin(), first_nonzero_byte_it)));
    }

    std::string to_hex(ByteView bytes, bool with_prefix) {
        static const char *kHexDigits{"0123456789abcdef"};
        std::string out(bytes.length() * 2 + (with_prefix ? 2 : 0), '\0');
        char *dest{&out[0]};
        if (with_prefix) {
            *dest++ = '0';
            *dest++ = 'x';
        }
        for (const auto &b: bytes) {
            *dest++ = kHexDigits[b >> 4];    // Hi
            *dest++ = kHexDigits[b & 0x0f];  // Lo
        }
        return out;
    }

    std::string abridge(std::string_view input, size_t length) {
        if (input.length() <= length) {
            return std::string(input);
        }
        return std::string(input.substr(0, length)) + "...";
    }

    static inline uint8_t unhex_lut(uint8_t x) { return kUnhexTable[x]; }

    static inline uint8_t unhex_lut4(uint8_t x) { return kUnhexTable4[x]; }

    std::optional<uint8_t> decode_hex_digit(char ch) noexcept {
        auto ret{unhex_lut(static_cast<uint8_t>(ch))};
        if (ret == 0xff) {
            return std::nullopt;
        }
        return ret;
    }

    std::optional<Bytes> from_hex(std::string_view hex) noexcept {
        if (has_hex_prefix(hex)) {
            hex.remove_prefix(2);
        }
        if (hex.empty()) {
            return Bytes{};
        }

        size_t pos(hex.length() & 1);  // "[0x]1" is legit and has to be treated as "[0x]01"
        Bytes out((hex.length() + pos) / 2, '\0');
        const char *src{hex.data()};
        const char *last = src + hex.length();
        uint8_t *dst{&out[0]};

        if (pos) {
            auto b{unhex_lut(static_cast<uint8_t>(*src++))};
            if (b == 0xff) {
                return std::nullopt;
            }
            *dst++ = b;
        }

        // following "while" is unrolling the loop when we have >= 4 target bytes
        // this is optional, but 5-10% faster
        while (last - src >= 8) {
            auto a{unhex_lut4(static_cast<uint8_t>(*src++))};
            auto b{unhex_lut(static_cast<uint8_t>(*src++))};
            auto c{unhex_lut4(static_cast<uint8_t>(*src++))};
            auto d{unhex_lut(static_cast<uint8_t>(*src++))};
            auto e{unhex_lut4(static_cast<uint8_t>(*src++))};
            auto f{unhex_lut(static_cast<uint8_t>(*src++))};
            auto g{unhex_lut4(static_cast<uint8_t>(*src++))};
            auto h{unhex_lut(static_cast<uint8_t>(*src++))};
            if ((b | d | f | h) == 0xff || (a | c | e | g) == 0xff) {
                return std::nullopt;
            }
            *dst++ = a | b;
            *dst++ = c | d;
            *dst++ = e | f;
            *dst++ = g | h;
        }

        while (src < last) {
            auto a{unhex_lut4(static_cast<uint8_t>(*src++))};
            auto b{unhex_lut(static_cast<uint8_t>(*src++))};
            if (a == 0xff || b == 0xff) {
                return std::nullopt;
            }
            *dst++ = a | b;
        }
        return out;
    }

    inline bool case_insensitive_char_comparer(char a, char b) { return (tolower(a) == tolower(b)); }

    bool iequals(const std::string_view a, const std::string_view b) {
        return (a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), case_insensitive_char_comparer));
    }

    std::optional<uint64_t> parse_size(const std::string &sizestr) {
        if (sizestr.empty()) {
            return 0ull;
        }

        static const std::regex pattern{R"(^(\d*)(\.\d{1,3})?\ *?(B|KB|MB|GB|TB)?$)", std::regex_constants::icase};
        std::smatch matches;
        if (!std::regex_search(sizestr, matches, pattern, std::regex_constants::match_default)) {
            return std::nullopt;
        }

        std::string int_part, dec_part, suf_part;
        uint64_t multiplier{1};  // Default for bytes (B|b)

        int_part = matches[1].str();
        if (!matches[2].str().empty()) {
            dec_part = matches[2].str().substr(1);
        }
        suf_part = matches[3].str();

        if (!suf_part.empty()) {
            if (iequals(suf_part, "KB")) {
                multiplier = kKibi;
            } else if (iequals(suf_part, "MB")) {
                multiplier = kMebi;
            } else if (iequals(suf_part, "GB")) {
                multiplier = kGibi;
            } else if (iequals(suf_part, "TB")) {
                multiplier = kTebi;
            }
        }

        auto number{std::strtoull(int_part.c_str(), nullptr, 10)};
        number *= multiplier;
        if (!dec_part.empty()) {
            // Use literals, so we don't deal with floats and doubles
            auto base{"1" + std::string(dec_part.size(), '0')};
            auto b{std::strtoul(base.c_str(), nullptr, 10)};
            auto d{std::strtoul(dec_part.c_str(), nullptr, 10)};
            number += multiplier * d / b;
        }
        return number;
    }

    std::string human_size(uint64_t bytes, const char *unit) {
        static const char *suffix[]{"", "K", "M", "G", "T"};
        static const uint32_t items{sizeof(suffix) / sizeof(suffix[0])};
        uint32_t index{0};
        double value{static_cast<double>(bytes)};
        while (value >= kKibi) {
            value /= kKibi;
            if (++index == (items - 1)) {
                break;
            }
        }
        static constexpr size_t kBufferSize{64};
        SILKWORM_THREAD_LOCAL char output[kBufferSize];
        SILKWORM_ASSERT(std::snprintf(output, kBufferSize, "%.02lf %s%s", value, suffix[index], unit) > 0);
        return output;
    }

    size_t prefix_length(ByteView a, ByteView b) {
        size_t len{std::min(a.length(), b.length())};
        for (size_t i{0}; i < len; ++i) {
            if (a[i] != b[i]) {
                return i;
            }
        }
        return len;
    }

    float to_float(const intx::uint256 &n) noexcept {
        static constexpr float k2_64{18446744073709551616.};  // 2^64
        const uint64_t *words{intx::as_words(n)};
        auto res{static_cast<float>(words[3])};
        res = k2_64 * res + static_cast<float>(words[2]);
        res = k2_64 * res + static_cast<float>(words[1]);
        res = k2_64 * res + static_cast<float>(words[0]);
        return res;
    }

//
    silkworm_ByteView silkworm_zeroless_view(silkworm_ByteView data) {
        auto cpp_view = silkworm::ByteView(data.data, data.length);
        auto result = silkworm::zeroless_view(cpp_view);
        return {result.data(), result.length()};
    }

    bool silkworm_has_hex_prefix(const char *s, size_t length) {
        return silkworm::has_hex_prefix(std::string_view(s, length));
    }

    bool silkworm_is_valid_hex(const char *s, size_t length) {
        return silkworm::is_valid_hex(std::string_view(s, length));
    }

    bool silkworm_is_valid_hash(const char *s, size_t length) {
        return silkworm::is_valid_hash(std::string_view(s, length));
    }

    bool silkworm_is_valid_address(const char *s, size_t length) {
        return silkworm::is_valid_address(std::string_view(s, length));
    }

    char *silkworm_to_hex(silkworm_ByteView bytes, bool with_prefix) {
        auto cpp_view = silkworm::ByteView(bytes.data, bytes.length);
        std::string result = silkworm::to_hex(cpp_view, with_prefix);
        char *c_str = new char[result.length() + 1];
        std::strcpy(c_str, result.c_str());
        return c_str;
    }

    char *silkworm_abridge(const char *input, size_t input_length, size_t max_length) {
        std::string_view sv(input, input_length);
        std::string result = silkworm::abridge(sv, max_length);
        char *c_str = new char[result.length() + 1];
        std::strcpy(c_str, result.c_str());
        return c_str;
    }

    int silkworm_decode_hex_digit(char ch) {
        auto result = silkworm::decode_hex_digit(ch);
        return result ? static_cast<int>(*result) : -1;
    }

    silkworm_ByteView silkworm_from_hex(const char *hex, size_t length) {
        std::string_view sv(hex, length);
        auto result = silkworm::from_hex(sv);
        if (!result) {
            return {nullptr, 0};
        }
        // Note: This is dangerous as the data might be deallocated.
        // Consider changing the API to return a silkworm_Bytes instead.
        return {result->data(), result->size()};
    }

    uint64_t silkworm_parse_size(const char *sizestr) {
        auto result = silkworm::parse_size(sizestr);
        return result ? *result : 0;
    }

    char *silkworm_human_size(uint64_t bytes, const char *unit) {
        std::string result = silkworm::human_size(bytes, unit);
        char *c_str = new char[result.length() + 1];
        std::strcpy(c_str, result.c_str());
        return c_str;
    }

    bool silkworm_iequals(const char *a, size_t a_length, const char *b, size_t b_length) {
        std::string_view sv_a(a, a_length);
        std::string_view sv_b(b, b_length);
        return silkworm::iequals(sv_a, sv_b);
    }

    size_t silkworm_prefix_length(silkworm_ByteView a, silkworm_ByteView b) {
        auto cpp_view_a = silkworm::ByteView(a.data, a.length);
        auto cpp_view_b = silkworm::ByteView(b.data, b.length);
        return silkworm::prefix_length(cpp_view_a, cpp_view_b);
    }

    void silkworm_keccak256(silkworm_ByteView view, uint8_t result[32]) {
        auto cpp_view = silkworm::ByteView(view.data, view.length);
        auto hash = silkworm::keccak256(cpp_view);
        std::memcpy(result, hash.bytes, 32);
    }

    uint64_t silkworm_from_string_sci(const char *str) {
        // This is a simplified version, as the original function returns uint256
        return silkworm::from_string_sci<uint64_t>(str);
    }

    float silkworm_to_float(uint64_t value) {
        // This is a simplified version, as the original function takes uint256
        return static_cast<float>(value);
    }

// Function to free memory allocated by C interface functions
    void silkworm_free_string(char *str) {
        delete[] str;
    }

}  // namespace silkworm
