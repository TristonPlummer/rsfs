#pragma once
#include <rsfs/io/RSBuffer.hpp>

namespace rsfs
{
    /**
     * Represents a file inside an archive.
     */
    struct FileData
    {
        /**
         * The id of the file.
         */
        size_t id{ 0 };

        /**
         * The name hash of the file.
         */
        size_t nameHash{ 0 };

        /**
         * The buffer representing the contents of the file.
         */
        RSBuffer contents{ 0 };
    };
}