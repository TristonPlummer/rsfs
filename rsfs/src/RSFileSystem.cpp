#include <rsfs/RSFileSystem.hpp>
#include <rsfs/compression/Compression.hpp>

#include <glog/logging.h>

#include <cassert>
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