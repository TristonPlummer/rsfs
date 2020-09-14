#pragma once

#include <rsfs/io/RSBuffer.hpp>
#include <rsfs/jag/Archive.hpp>
#include <rsfs/jag/ArchiveData.hpp>
#include <rsfs/jag/DataFile.hpp>
#include <rsfs/jag/IndexEntry.hpp>

#include <array>
#include <fstream>
#include <map>

namespace rsfs
{
    /**
     * Represents an index in the RuneScape file system. An index acts as a container for multiple file archives.
     */
    class IndexFile
    {
    public:
        /**
         * Creates an index with a specific metadata stream.
         * @param stream    The metadata file stream for this index.
         * @param dataFile  The main data file.
         * @param id        The id of this index.
         */
        IndexFile(std::ifstream stream, DataFile* dataFile, size_t id);

        /**
         * Destroys the resources used by this index.
         */
        ~IndexFile();

        /**
         * Reads the entry data for a specific id from this index.
         * @param id    The entry id.
         * @return      The entry data.
         */
        IndexEntry read(size_t id);

        /**
         * Parses the data for this index from a decompressed buffer.
         * @param buf   The buffer.
         */
        void load(RSBuffer& buf);

        /**
         * Gets an archive with a specific id
         * @param archiveId The archive id.
         * @return          The archive.
         */
        Archive& getArchive(size_t archiveId);

        /**
         * Gets the buffer data for a specific archive in this index.
         * @param archive   The archive id.
         * @return          The compressed archive data.
         */
        RSBuffer readArchive(size_t archive);

        /**
         * Gets the data for a specific file in an archive.
         * @param archiveId The archive id.
         * @param fileId    The file id.
         * @return          The file data.
         */
        RSBuffer data(size_t archiveId, int32_t fileId = 0);

        /**
         * Gets the id of this index.
         * @return  The id.
         */
        [[nodiscard]] size_t getId() const
        {
            return id_;
        }

        /**
         * Gets the revision of this index.
         * @return  The revision.
         */
        [[nodiscard]] size_t revision() const
        {
            return revision_;
        }

        /**
         * Gets the number of archives in this index.
         * @return  The number of archives.
         */
        [[nodiscard]] size_t archiveCount() const
        {
            return archives_.size();
        }

        /**
         * Gets the number of metadata entries.
         * @return  The number of entries.
         */
        [[nodiscard]] size_t entryCount() const
        {
            return entryCount_;
        }

    private:
        /**
         * The stream to this index's metadata file.
         */
        std::ifstream stream_;

        /**
         * The main data file.
         */
        DataFile* dataFile_;

        /**
         * The number of metadata entries.
         */
        size_t entryCount_{ 0 };

        /**
         * The id of this index.
         */
        size_t id_{ 0 };

        /**
         * The protocol of this index.
         */
        size_t protocol_{ 0 };

        /**
         * The revision of this index.
         */
        size_t revision_{ 0 };

        /**
         * If archives in this index are given a name hash.
         */
        bool named_{ false };

        /**
         * If archives in this index contain a whirlpool digest.
         */
        bool whirlpool_{ false };
        
        /**
         * The map of archive ids to the archive instance.
         */
        std::map<size_t, Archive*> archives_;
    };
}