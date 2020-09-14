#pragma once

#include <rsfs/jag/DataFile.hpp>
#include <rsfs/jag/IndexData.hpp>
#include <rsfs/jag/IndexFile.hpp>

#include <array>
#include <fstream>
#include <string_view>
#include <vector>

namespace rsfs
{
    /**
     * Represents the RuneScape virtual filesystem, and offers an interface for retrieving data from specific
     * archives or files within an index.
     */
    class RSFileSystem
    {
    public:
        /**
         * Initialises the RuneScape filesystem.
         * @param path  The path to the RuneScape data files.
         */
        explicit RSFileSystem(const std::string_view& path);

        /**
         * Handles the destruction of this filesystem.
         */
        ~RSFileSystem();

        /**
         * Gets an index with a specified id.
         * @param id    The index id.
         * @return      The index.
         */
        [[nodiscard]] IndexFile& getIndex(size_t id) const;

        /**
         * Builds the checksum table for this file system.
         * @param whirlpool If we should include a whirlpool digest in this checksum table.
         */
        void buildChecksumTable(bool whirlpool = true);

        /**
         * Get the checksum table for this file system.
         * @return  The checksum table.
         */
        [[nodiscard]] RSBuffer checksumTable() const;

    private:
        /**
         * The file stream to the asset data file.
         */
        DataFile* dataFile_;

        /**
         * The metadata index.
         */
        IndexFile* metadataIndex_;

        /**
         * Loads all of the cache indices into memory.
         */
        void loadIndices();

        /**
         * Gets an index with a specified id
         * @param id    The id of the index.
         * @return      The index file.
         */
        [[nodiscard]] RSBuffer readIndex(size_t id) const;

        /**
         * The number of indices in the filesystem.
         */
        size_t indexCount_{ 0 };

        /**
         * The array of indices.
         */
        std::vector<IndexFile*> indices_;

        /**
         * The checksum table buffer.
         */
        RSBuffer checksumTable_{ 0 };
    };
}