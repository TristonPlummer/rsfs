#include <rsfs/jag/Archive.hpp>

using namespace rsfs;

/**
 * Initialises this archive based on the archive's metadata
 * @param data  The metadata
 */
Archive::Archive(ArchiveData data): data_(std::move(data))
{
    // Initialise the file list
    for (auto&& file: data_.files)
        files_[file.id] = file;
}

/**
 * Reads the data for an archive.
 * @param buf   The decompressed archive data.
 */
void Archive::read(RSBuffer& buf)
{
    loaded_ = true;  // Mark this archive as loaded

    // If there is only one file, set it's contents as this buffer.
    auto fileCount = files_.size();
    if (fileCount == 1)
    {
        auto& file    = files_[0];
        file.contents = buf;
        return;
    }

    // The number of chunks is the last byte
    buf.seek(buf.getSize() - 1);
    auto chunks = buf.readByte() & 0xFFu;

    // Seek to the position of the chunk metadata
    buf.seek(buf.getSize() - 1 - chunks * fileCount * 4);
    std::vector<std::vector<int32_t>> chunkSizes(fileCount, std::vector<int32_t>(chunks));
    std::vector<int32_t> fileSizes(fileCount);

    // Read the chunk and file sizes
    for (auto chunk = 0; chunk < chunks; ++chunk)
    {
        auto chunkSize = 0;
        for (auto id = 0; id < fileCount; ++id)
        {
            auto delta = buf.readInt();
            chunkSize += delta;                       // Size of this chunk
            chunkSizes.at(id).at(chunk) = chunkSize;  // Store the size of this chunk
            fileSizes.at(id) += chunkSize;            // Add chunk size to file size
        }
    }

    // File contents are at the beginning of the archive
    buf.seek(0);

    // Prepare to read the file offsets, and their contents
    std::vector<RSBuffer> contents(fileCount, RSBuffer(0));

    // Read the chunks
    for (auto chunk = 0; chunk < chunks; ++chunk)
    {
        for (auto id = 0; id < fileCount; ++id)
        {
            auto size       = chunkSizes.at(id).at(chunk);  // Get the size of the file's chunk
            auto data       = buf.readBytes(size);          // Read the data from the archive
            contents.at(id) = data;                         // Store the data
        }
    }

    // Set the contents of the files
    for (auto i = 0; i < fileCount; i++)
    {
        auto& file    = files_[i];
        file.contents = contents.at(i);
    }
}

/**
 * Gets the data for a specific file
 * @param id    The file id
 * @return      The file data
 */
RSBuffer Archive::getFileData(size_t id) const
{
    return files_.at(id).contents;
}

/**
 * Gets a sorted vector of all the files in this archive.
 * @return  The files in this archive.
 */
std::vector<FileData> Archive::getFiles() const
{
    std::vector<FileData> files;
    std::transform(files_.begin(), files_.end(), std::back_inserter(files), [](auto& kv) { return kv.second; });
    std::sort(files.begin(), files.end(),
              [](const FileData& first, const FileData& second) { return first.id < second.id; });
    return files;
}