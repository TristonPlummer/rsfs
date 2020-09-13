#pragma once

namespace rsfs
{
    /**
     * Represents an entry in an index.
     */
    struct IndexEntry
    {
        /**
         * The length of the entry.
         */
        uint32_t length{ 0 };

        /**
         * The sector that the entry can be found in.
         */
        uint32_t sector{ 0 };
    };
}