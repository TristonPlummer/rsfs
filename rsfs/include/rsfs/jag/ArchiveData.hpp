#pragma once

#include <rsfs/jag/FileData.hpp>

#include <array>
#include <vector>

/**
 * The size of a whirlpool digest.
 */
constexpr const auto WHIRLPOOL_SIZE = 64;

namespace rsfs
{
    /**
     * Represents the metadata of an archive.
     */
    struct ArchiveData
    {
        /**
         * The id of the archive.
         */
        size_t id;

        /**
         * The name hash of the archive.
         */
        int nameHash;

        /**
         * The checksum of the archive.
         */
        int crc;

        /**
         * The revision of the archive.
         */
        size_t revision;

        /**
         * The number of files in the archive.
         */
        size_t fileCount;

        /**
         * The whirlpool digest of this archive.
         */
        std::array<char, WHIRLPOOL_SIZE> whirlpool;

        /**
         * The vector of file data for this archive.
         */
        std::vector<FileData> files;
    };
}