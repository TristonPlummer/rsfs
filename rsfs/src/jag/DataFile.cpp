#include <rsfs/jag/DataFile.hpp>

using namespace rsfs;

/**
 * The size of a data sector.
 */
constexpr const auto SECTOR_SIZE = 520;

/**
 * The size of a sector with a small header
 */
constexpr const auto SMALL_HEADER_SIZE = 8;

/**
 * The size of a sector with a large header (to support archive ids > 65535)
 */
constexpr const auto LARGE_HEADER_SIZE = 10;

/**
 * Initialises the data file based on the file stream.
 * @param stream    The input stream
 */
DataFile::DataFile(std::ifstream stream): stream_(std::move(stream))
{
    // Set the length of the data file
    length_ = stream_.tellg();
}

/**
 * Reads an entry from the data file.
 * @param index     The index id.
 * @param archive   The archive id.
 * @param sector    The sector.
 * @param length    The length of data.
 * @return          The read data.
 */
RSBuffer DataFile::read(size_t index, size_t archive, size_t sector, size_t length)
{
    // Validate the sector input
    if (sector <= 0 || length_ / SECTOR_SIZE < sector)
    {
        throw std::runtime_error("Sector out of bounds");
    }

    // The temporary buffer to read into
    char tmp[SECTOR_SIZE];
    RSBuffer buffer(length);

    // Read the data, starting from the sector specified
    for (int part = 0, readByteCount = 0, nextSector; length > readByteCount; sector = nextSector)
    {
        // Seek to the sector
        stream_.seekg(SECTOR_SIZE * sector, std::ios::beg);

        // If we should read this as a large sector
        auto largeSector = archive > 0xFFFF;
        auto headerSize  = largeSector ? LARGE_HEADER_SIZE : SMALL_HEADER_SIZE;
        auto dataSize    = SECTOR_SIZE - headerSize;

        // Read the sector into the temporary buffer
        auto bytesRead = stream_.readsome(tmp, SECTOR_SIZE);
        if (bytesRead != SECTOR_SIZE)
        {
            throw std::runtime_error("Short read");
        }

        // Wrap the bytes in a RSBuffer
        RSBuffer buf(tmp, SECTOR_SIZE);

        // Read the header
        auto currentArchive = largeSector ? buf.readInt() : buf.readShort();
        auto currentPart    = buf.readShort() & 0xFFFF;
        nextSector          = buf.readTriByte();
        auto currentIndex   = buf.readByte() & 0xFFu;

        // Write the bytes into our buffer
        buffer.writeBytes(&tmp[headerSize], dataSize);
        readByteCount += dataSize;
        ++part;
    }

    buffer.resize(length);
    return buffer;
}