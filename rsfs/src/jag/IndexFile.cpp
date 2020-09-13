#include <rsfs/compression/Compression.hpp>
#include <rsfs/jag/IndexFile.hpp>

#include <glog/logging.h>

#include <cstring>

using namespace rsfs;

/**
 * The size of an entry in an index file.
 */
constexpr auto ENTRY_SIZE = 6;

/**
 * The settings flags
 */
constexpr const auto FLAG_NAMED     = 0x1u;
constexpr const auto FLAG_WHIRLPOOL = 0x2u;

/**
 * Creates an index with a specific metadata stream.
 * @param stream    The metadata file stream for this index.
 * @param dataFile  The main data file.
 * @param id        The data of this index.
 */
IndexFile::IndexFile(std::ifstream stream, DataFile* dataFile, size_t id)
    : stream_(std::move(stream)), dataFile_(dataFile), id_(id)
{
    // Calculate the number of entries
    entryCount_ = stream_.tellg() / ENTRY_SIZE;
    stream_.seekg(std::ios::beg);
}

/**
 * Destroys the resources used by this index.
 */
IndexFile::~IndexFile()
{
    for (auto&& archive: archives_)
        delete archive.second;
}

/**
 * Reads the entry data for a specific id from this index.
 * @param id    The entry id.
 * @return      The entry data.
 */
IndexEntry IndexFile::read(size_t id)
{
    stream_.seekg(id * ENTRY_SIZE, std::ios::beg);
    char tmp[ENTRY_SIZE];

    auto bytesRead = stream_.readsome(tmp, ENTRY_SIZE);
    if (bytesRead != ENTRY_SIZE)
    {
        throw std::runtime_error("Short read");
    }

    RSBuffer buf(tmp, ENTRY_SIZE);
    auto length = buf.readTriByte();
    auto sector = buf.readTriByte();

    return { length, sector };
}

/**
 * Parses the data for this index from a decompressed buffer.
 * @param buf   The buffer.
 */
void IndexFile::load(RSBuffer& buf)
{
    protocol_ = buf.readByte();

    // If the protocol isn't valid, throw an error
    if (protocol_ < 5 || protocol_ > 7)
    {
        throw std::runtime_error("Unsupported protocol");
    }

    // A helper function to read a "smart" data-type
    auto readSmart = [&](RSBuffer& buf) { return protocol_ >= 7 ? buf.readSmart() : buf.readShort(); };

    // Read the revision, if applicable
    if (protocol_ >= 6)
        revision_ = buf.readInt();

    // Read the settings mask
    auto settings = buf.readByte() & 0xFFu;
    named_        = (FLAG_NAMED & settings) != 0;
    whirlpool_    = (FLAG_WHIRLPOOL & settings) != 0;

    // Read the number of archives
    auto archiveCount = readSmart(buf);
    std::vector<ArchiveData> archiveData;
    archiveData.resize(archiveCount);

    // The last archive id
    size_t lastArchiveId = 0;
    for (auto i = 0; i < archiveCount; i++)
    {
        size_t id         = lastArchiveId += readSmart(buf);
        archiveData.at(i) = ArchiveData{ .id = id };
    }

    // If this is a named index, we need to read the name hashes
    if (named_)
    {
        for (auto&& archive: archiveData)
            archive.nameHash = buf.readInt();
    }

    // If the archives have a whirlpool digest
    if (whirlpool_)
    {
        for (auto&& archive: archiveData)
        {
            auto data = buf.readBytes(WHIRLPOOL_SIZE);
            std::memcpy(archive.whirlpool.data(), data.begin(), WHIRLPOOL_SIZE);
        }
    }

    // Read the checksums for each archive
    for (auto&& archive: archiveData)
        archive.crc = buf.readInt();

    // Read the revisions for each archive
    for (auto&& archive: archiveData)
        archive.revision = buf.readInt();

    // Read the file count for each archive
    for (auto&& archive: archiveData)
    {
        archive.fileCount = readSmart(buf);
        archive.files.resize(archive.fileCount);
    }

    // Read the file ids for each archive
    for (auto&& archive: archiveData)
    {
        size_t lastFileId = 0;
        for (auto i = 0; i < archive.fileCount; i++)
        {
            auto id             = lastFileId += readSmart(buf);
            archive.files.at(i) = FileData{ .id = id };
        }
    }

    // Read the name hash for each file
    if (named_)
    {
        for (auto&& archive: archiveData)
        {
            for (auto&& file: archive.files)
                file.nameHash = buf.readInt();
        }
    }

    // Create the archives
    for (auto&& archive: archiveData)
        archives_[archive.id] = new Archive(archive);
}

/**
 * Gets the buffer data for a specific archive in this index.
 * @param archive   The archive id.
 * @return          The archive data.
 */
RSBuffer IndexFile::readArchive(size_t archive)
{
    auto entry = read(archive);
    return dataFile_->read(id_, archive, entry.sector, entry.length);
}

/**
 * Gets an archive with a specific id
 * @param archiveId The archive id
 * @return          The archive
 */
Archive& IndexFile::getArchive(size_t archiveId)
{
    auto* archive = archives_[archiveId];
    assert(archive);

    if (!archive->loaded())
    {
        auto data         = readArchive(archiveId);
        auto decompressed = Compression::decompress(data);
        archive->read(decompressed);
    }
    return *archive;
}

/**
 * Gets the data for a specific file in an archive.
 * @param archive   The archive id.
 * @param file      The file id.
 * @return          The file data.
 */
RSBuffer IndexFile::data(size_t archiveId, int32_t fileId)
{
    auto archive = getArchive(archiveId);
    return archive.getFileData(fileId);
}