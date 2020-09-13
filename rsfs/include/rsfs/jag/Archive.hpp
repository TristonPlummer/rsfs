#pragma once
#include <rsfs/io/RSBuffer.hpp>
#include <rsfs/jag/ArchiveData.hpp>
#include <rsfs/jag/FileData.hpp>

#include <map>
#include <vector>

namespace rsfs
{
    /**
     * Represents an archive inside an index. An archive is responsible for containing a number of individual files.
     */
    class Archive
    {
    public:
        /**
         * Initialises this archive based on it's metadata.
         * @param data  The archive's metadata.
         */
        explicit Archive(ArchiveData data);

        /**
         * Reads the data for an archive.
         * @param buf   The decompressed archive data.
         */
        void read(RSBuffer& buf);

        /**
         * Gets a sorted vector of all the files in this archive.
         * @return  The files in this archive.
         */
        [[nodiscard]] std::vector<FileData> getFiles() const;

        /**
         * Gets the data for a specific file
         * @param id    The file id.
         * @return      The file data.
         */
        [[nodiscard]] RSBuffer getFileData(size_t id) const;

        /**
         * Checks if this archive has been loaded.
         * @return  If the archive has been loaded.
         */
        [[nodiscard]] bool loaded() const
        {
            return loaded_;
        }

        /**
         * Gets the name hash of this archive.
         * @return  The name hash.
         */
        [[nodiscard]] int nameHash() const
        {
            return data_.nameHash;
        }

        /**
         * Gets the CRC32 checksum of this archive.
         * @return  The checksum.
         */
        [[nodiscard]] int checksum() const
        {
            return data_.crc;
        }

        /**
         * Gets the revision of this archive.
         * @return  The revision.
         */
        [[nodiscard]] size_t revision() const
        {
            return data_.revision;
        }

    private:
        /**
         * The archive meta data.
         */
        ArchiveData data_;

        /**
         * If this archive has been loaded.
         */
        bool loaded_{ false };

        /**
         * A map of file ids to the file data.
         */
        std::map<size_t, FileData> files_;
    };
}