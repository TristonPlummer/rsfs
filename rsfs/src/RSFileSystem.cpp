#include <rsfs/RSFileSystem.hpp>
#include <rsfs/compression/Compression.hpp>

#include <boost/crc.hpp>
#include <glog/logging.h>

#include <cassert>
#include <crypto++/whrlpool.h>
#include <sstream>

using namespace rsfs;

/**
 * The name of the data file.
 */
constexpr auto DATA_NAME = "main_file_cache.dat2";

/**
 * The name of an index file, without the trailing id.
 */
constexpr auto INDEX_NAME = "main_file_cache.idx";

/**
 * The id of the metadata index file.
 */
constexpr auto METADATA_INDEX = 255;

/**
 * Initialises the RuneScape filesystem.
 * @param path  The path to the RuneScape data files.
 */
RSFileSystem::RSFileSystem(const std::string_view& path): indices_({})
{
    // A helper function used to get the path to an index file with a specified id
    auto getIndexFile = [path](auto id) {
        std::stringstream stream;
        stream << path << INDEX_NAME << id;
        return stream.str();
    };

    // The flags to open a file with
    auto fileFlags = std::ios::in | std::ios::binary | std::ios::ate;

    // Open the data file
    std::stringstream stream;
    stream << path << DATA_NAME;
    dataFile_ = new DataFile(std::ifstream(stream.str(), fileFlags));

    // Parse the metadata index
    metadataIndex_ = new IndexFile(std::ifstream(getIndexFile(METADATA_INDEX), fileFlags), dataFile_, METADATA_INDEX);
    indexCount_    = metadataIndex_->entryCount();

    // Parse the other indices
    indices_.reserve(indexCount_);
    for (size_t idx = 0; idx < indexCount_; idx++)
    {
        auto* index = new IndexFile(std::ifstream(getIndexFile(idx), fileFlags), dataFile_, idx);
        indices_.push_back(index);
    }

    // Load the indices
    loadIndices();
}

/**
 * Handles the destruction of this filesystem.
 */
RSFileSystem::~RSFileSystem()
{
    for (auto* index: indices_)
        delete index;
    delete metadataIndex_;
    delete dataFile_;
}

/**
 * Gets an index with a specified id
 * @param id    The id of the index.
 * @return      The index file.
 */
RSBuffer RSFileSystem::readIndex(size_t id) const
{
    assert(indices_.size() > id);
    auto entry = metadataIndex_->read(id);
    return dataFile_->read(METADATA_INDEX, id, entry.sector, entry.length);
}

/**
 * Loads all of the indices in the filesystem.
 */
void RSFileSystem::loadIndices()
{
    for (auto&& index: indices_)
    {
        // Read the data for the index.
        RSBuffer data     = readIndex(index->getId());
        auto decompressed = Compression::decompress(data.resetReaderIndex());

        // Load the data from the index
        index->load(decompressed);
    }
}

/**
 * Gets an index with a specified id.
 * @param id    The id of the index
 * @return      The index
 */
IndexFile& RSFileSystem::getIndex(size_t id) const
{
    auto* idx = indices_.at(id);
    assert(idx);
    return *idx;
}

/**
 * Builds the checksum table for this file system.
 * @param whirlpool If we should calculate the whirlpool digests.
 */
void RSFileSystem::buildChecksumTable(bool whirlpool)
{
    auto entryCount = metadataIndex_->entryCount();

    // Calculate the length of the checksum table
    auto length = 1 + entryCount * 8;
    if (whirlpool)
        length += 1 + WHIRLPOOL_SIZE + (entryCount * WHIRLPOOL_SIZE);

    // The outgoing buffer to write to
    RSBuffer out(length);

    // If we are to include a whirlpool digest, the first byte must represent the number of entries
    out.writeByte(entryCount);

    // Encode the individual index entries
    for (auto i = 0; i < entryCount; i++)
    {
        auto buf  = readIndex(i);
        auto& idx = getIndex(i);

        // Generate the checksum of the buffer.
        boost::crc_32_type checksum;
        checksum.process_block(buf.begin(), buf.end());

        // Write the checksum and revision
        out.writeInt(checksum.checksum());
        out.writeInt(idx.revision());

        // Include the digest of the compressed index data.
        if (whirlpool)
        {
            // The digest of the index buffer.
            std::array<char, WHIRLPOOL_SIZE> digest{ 0 };

            // Calculate the whirlpool digest.
            CryptoPP::Whirlpool hash;
            hash.Update(reinterpret_cast<const byte*>(buf.begin()), buf.getSize());
            hash.Final(reinterpret_cast<byte*>(digest.data()));

            // Write the digest
            out.writeBytes(digest);
        }
    }

    // Calculate a digest of the checksum table
    if (whirlpool)
    {
        // The digest of the table so far
        std::array<char, WHIRLPOOL_SIZE> digest{ 0 };

        // Calculate the digest
        CryptoPP::Whirlpool hash;
        hash.Update(reinterpret_cast<const byte*>(out.begin()), out.getSize());
        hash.Final(reinterpret_cast<byte*>(digest.data()));

        // Write the digest of the checksum table
        out.writeByte(0);
        out.writeBytes(digest);
    }

    // Save the checksum table
    checksumTable_ = out;
}

/**
 * Get the checksum table for this file system.
 * @return  The checksum table
 */
RSBuffer RSFileSystem::checksumTable() const
{
    return checksumTable_;
}