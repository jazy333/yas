#pragma once
#include "compression.h"

namespace yas {
template <bool delta>
class VariableByteCompression : public Compression {
 public:
  void compress(uint32_t* in, size_t in_size, uint8_t* out,
                size_t& out_size) override {
    uint8_t* start = out;
    uint32_t pre = 0;
    for (size_t i = 0; i < in_size; ++i) {
      uint32_t val = in[i];
      if (delta) {
        val -= pre;
        pre += val;
      }
      if (val < (1U << 7)) {
        *out = val & 0x7F;
        ++out;
      } else if (val < (1U << 14)) {
        *out = static_cast<uint8_t>((val & 0x7F) | (1U << 7));
        ++out;
        *out = static_cast<uint8_t>(val >> 7);
        ++out;
      } else if (val < (1U << 21)) {
        *out = static_cast<uint8_t>((val & 0x7F) | (1U << 7));
        ++out;
        *out = static_cast<uint8_t>(((val >> 7) & 0x7F) | (1U << 7));
        ++out;
        *out = static_cast<uint8_t>(val >> 14);
        ++out;
      } else if (val < (1U << 28)) {
        *out = static_cast<uint8_t>((val & 0x7F) | (1U << 7));
        ++out;
        *out = static_cast<uint8_t>(((val >> 7) & 0x7F) | (1U << 7));
        ++out;
        *out = static_cast<uint8_t>(((val >> 14) & 0x7F) | (1U << 7));
        ++out;
        *out = static_cast<uint8_t>(val >> 21);
        ++out;
      } else {
        *out = static_cast<uint8_t>((val & 0x7F) | (1U << 7));
        ++out;
        *out = static_cast<uint8_t>(((val >> 7) & 0x7F) | (1U << 7));
        ++out;
        *out = static_cast<uint8_t>(((val >> 14) & 0x7F) | (1U << 7));
        ++out;
        *out = static_cast<uint8_t>(((val >> 21) & 0x7F) | (1U << 7));
        ++out;
        *out = static_cast<uint8_t>(val >> 28);
        ++out;
      }
    }
    out_size = out - start;
  }

  uint8_t* decompress(const uint8_t* in, size_t in_size, uint32_t* out,
                      size_t& out_size) override {
    if (in_size == 0) {
      out_size = 0;
      return const_cast<uint8_t*>(in);
    }
    uint32_t pre = 0;
    uint32_t* out_start = out;
    const uint8_t* in_end = in + in_size;
    while (in + 5 < in_end) {
      if (delta) {
        uint8_t c;

        c = in[0];
        *out = c & 0x7F;
        if (c < 0x80) {
          *out += pre;
          pre = *out;
          out++;
          in++;
          continue;
        }

        c = in[1];
        *out = *out | ((c & 0x7F) << 7);
        if (c < 0x80) {
          *out += pre;
          pre = *out;
          out++;
          in += 2;
          continue;
        }

        c = in[2];
        *out = *out | ((c & 0x7F) << 14);
        if (c < 0x80) {
          *out += pre;
          pre = *out;
          out++;
          in += 3;
          continue;
        }

        c = in[3];
        *out = *out | ((c & 0x7F) << 21);
        if (c < 0x80) {
          *out += pre;
          pre = *out;
          out++;
          in += 4;
          continue;
        }

        c = in[4];
        *out = *out | ((c & 0x7F) << 28);
        *out += pre;
        pre = *out;
        out++;
        in += 5;

      } else {
        uint8_t c;

        c = in[0];
        *out = c & 0x7F;
        if (c < 0x80) {
          out++;
          in += 1;
          continue;
        }

        c = in[1];
        *out = *out | ((c & 0x7F) << 7);
        if (c < 0x80) {
          out++;
          in += 2;
          continue;
        }

        c = in[2];
        *out = *out | ((c & 0x7F) << 14);
        if (c < 0x80) {
          out++;
          in += 3;
          continue;
        }

        c = in[3];
        *out = *out | ((c & 0x7F) << 21);
        if (c < 0x80) {
          out++;
          in += 4;
          continue;
        }

        c = in[4];
        *out = *out | ((c & 0x7F) << 28);
        out++;
        in += 5;
      }
    }

    while (in < in_end) {
      uint8_t shift = 0;
      for (uint32_t v = 0; in < in_end; shift += 7) {
        uint8_t c = *in++;
        v |= ((c & 0x7F) << shift);
        if (c <= 0x7F) {
          *out++ = delta ? (pre = v + pre) : v;
          break;
        }
      }
    }

    out_size = out - out_start;
    return const_cast<uint8_t*>(in);
  }
};
}  // namespace yas