#pragma once
#include <string>
namespace yas
{
    class Compression
    {
    public:
        virtual int compress(const std::string& in,std::string& out) = 0;
        virtual int decompress(const std::string& in,std::string& out) = 0;
    };
} // namespace yas