#include <rsfs/compression/Compression.hpp>
#include <rsfs/compression/CompressionType.hpp>
#include <zlib.h>

#include <boost/crc.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>

using namespace rsfs;

/**
 * The length of the compression header
 */
constexpr const auto COMPRESSION_HEADER_LENGTH = 4;

/**
 * The BZIP2 header.
 *
 * BZ = Magic constant
 * h = BZIP2 ('H'uffman coding)
 * 1 = Block size
 */
constexpr const auto BZIP2_HEADER = "BZh1";

/**
 * Decompresses a buffer.
 * @param buf   The compressed buffer.
 * @return      The decompressed buffer.
 */
RSBuffer Compression::decompress(RSBuffer& buf)
{
    using namespace std;
    using namespace boost::iostreams;

    auto type           = static_cast<CompressionType>(buf.readByte());
    auto compressedSize = buf.readInt();

    // If the compression type is nothing, return the data block
    if (type == NONE)
    {
        auto range = buf.readRange(compressedSize);
        return RSBuffer(range.begin(), compressedSize);
    }

    // The length of the decompressed data
    auto decompressedSize = buf.readInt();

    // The current checksum of the data
    boost::crc_32_type checksum;

    // The compressed data
    RSBuffer compressed(compressedSize + COMPRESSION_HEADER_LENGTH);

    // Write the BZIP2 header if applicable
    if (type == BZIP2)
    {
        compressed.writeBytes(BZIP2_HEADER, 4);
    }

    // Write the bytes to decompress
    compressed.writeBytes(buf.readRange(compressedSize));

    // Update the checksum
    checksum.process_block(compressed.begin() + COMPRESSION_HEADER_LENGTH, compressed.end());

    // If there are still bytes to be read
    if (buf.getRemaining() >= 2)
    {
        auto revision = buf.readShort();
        assert(revision != INT16_MAX);
    }

    // Prepare to decompress the payload
    filtering_streambuf<input> in;
    std::stringstream out;

    // Push the decompressor
    if (type == BZIP2)
        in.push(bzip2_decompressor());
    else if (type == GZIP)
    {
        zlib_params params;  // Configure Zlib to disregard the GZIP header
        params.window_bits = 16 + MAX_WBITS;
        in.push(zlib_decompressor(params));
    }

    // Push the data to decompress, and copy it to the output buffer
    in.push(compressed.readRange(compressedSize + COMPRESSION_HEADER_LENGTH));
    copy(in, out);

    // Copy the decompressed data to a buffer
    auto decompressedData = out.str();
    RSBuffer decompressed(decompressedData.data(), decompressedSize);
    return decompressed;
}