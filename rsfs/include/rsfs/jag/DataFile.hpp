#pragma once

#include <rsfs/io/RSBuffer.hpp>

#include <fstream>

namespace rsfs
{
    /**
     * Represents the main RuneScape data file, which holds all of the archive
     * and file data.
     */
    class DataFile
    {
    public:
        /**
         * Creates a data file interface from a file stream.
         * @param stream    The stream.
         */
        explicit DataFile(std::ifstream stream);

        /**
         * Reads an entry from the data file.
         * @param index     The index to read from.
         * @param archive   The archive in the index.
         * @param sector    The sector in the data file.
         * @param length    The number of bytes to read.
         * @return          The buffer that was read
         */
        RSBuffer read(size_t index, size_t archive, size_t sector, size_t length);

    private:
        /**
         * The file stream.
         */
        std::ifstream stream_;

        /**
         * The length of the file.
         */
        size_t length_{ 0 };
    };
}