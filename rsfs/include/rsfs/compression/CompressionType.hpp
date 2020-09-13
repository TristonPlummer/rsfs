#pragma once

namespace rsfs
{
    /**
     * Represents a type of compression used for data in the RuneScape file system.
     */
    enum CompressionType : uint8_t
    {
        NONE,
        BZIP2,
        GZIP,
    };
}