#pragma once

#include <rsfs/io/RSBuffer.hpp>

#include <array>

namespace rsfs
{
    /**
     * A static class that is responsible for handling the compression
     * and decompression of archives in the RuneScape filesystem.
     */
    class Compression
    {
    public:
        /**
         * Decompresses a buffer.
         * @param buf   The compressed buffer.
         * @return      The decompressed buffer.
         */
        static RSBuffer decompress(RSBuffer& buf);
    };
}