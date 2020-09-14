#include <rsfs/io/RSBuffer.hpp>

#include <glog/logging.h>

#include <sstream>

using namespace rsfs;

/**
 * Initialises a buffer from a file stream.
 * @param stream    The file stream to read from.
 */
RSBuffer::RSBuffer(std::ifstream& stream)
{
    // Seek to the beginning of the stream
    stream.seekg(0, std::ios::beg);

    // Allocate the stream to the buffer
    buf_ = std::vector<char>((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
}

/**
 * Initialises an empty buffer with a fixed size.
 * @param size  The desired size of the buffer.
 */
RSBuffer::RSBuffer(size_t size)
{
    buf_.reserve(size);
}

/**
 * Moves the reader to a specified position in the buffer.
 * @param pos   The new reader position.
 */
void RSBuffer::seek(size_t pos)
{
    assert(pos < buf_.size());
    readerIndex_ = pos;
}

/**
 * Initialises a buffer from an array.
 * @param buf   The pointer to the start of the array
 * @param size  The size of the array
 */
RSBuffer::RSBuffer(const char* buf, size_t size)
{
    buf_.resize(size);
    for (size_t i = 0; i < size; i++)
    {
        buf_.at(i) = buf[i];
    }
}

/**
 * Reads the reader index of this buffer.
 * @return  This buffer.
 */
RSBuffer& RSBuffer::resetReaderIndex()
{
    readerIndex_ = 0;
    return *this;
}

/**
 * Resizes this buffer.
 * @param length    The new length.
 */
void RSBuffer::resize(size_t length)
{
    buf_.resize(length);
}

/**
 * Writes a single byte to the buffer.
 * @param value The byte value.
 */
void RSBuffer::writeByte(char value)
{
    buf_.push_back(value);
}

/**
 * Writes a series of bytes to the buffer.
 * @param buf   The source array.
 * @param size  The size of the array.
 */
void RSBuffer::writeBytes(const char* buf, size_t size)
{
    for (auto i = 0; i < size; i++)
        writeByte(buf[i]);
}

/**
 * Writes a range of bytes to the buffer.
 * @param range The range of bytes.
 */
void RSBuffer::writeBytes(boost::iterator_range<const char*> range)
{
    for (char it: range)
        writeByte(it);
}

/**
 * Writes an integer value to the buffer.
 * @param value The value to write.
 */
void RSBuffer::writeInt(int32_t value)
{
    writeByte((value >> 24u) & 0xFFu);
    writeByte((value >> 16u) & 0xFFu);
    writeByte((value >> 8u) & 0xFFu);
    writeByte(value & 0xFFu);
}

/**
 * Reads a single byte from the buffer.
 * @return  The value.
 */
uint8_t RSBuffer::readByte()
{
    return static_cast<uint8_t>(buf_.at(readerIndex_++));
}

/**
 * Reads a series of bytes from the buffer.
 * @param size  The number of bytes to read.
 * @return      The buffer that was read.
 */
RSBuffer RSBuffer::readBytes(size_t size)
{
    auto position = readerIndex_;
    readerIndex_ += size;

    return { &buf_[position], size };
}

/**
 * Reads a two-byte integer from the buffer.
 * @return  The value.
 */
uint16_t RSBuffer::readShort()
{
    return (readByte() << 8u) + readByte();
}

/**
 * Reads a three-byte integer from the buffer.
 * @return  The value.
 */
uint32_t RSBuffer::readTriByte()
{
    return (readByte() << 16u) + (readByte() << 8u) + readByte();
}

/**
 * Reads a four-byte integer from the buffer.
 * @return  The value.
 */
uint32_t RSBuffer::readInt()
{
    return (readByte() << 24u) + (readByte() << 16u) + (readByte() << 8u) + readByte();
}

/**
 * Reads either a short or an integer, depending on if the first byte is 0 or not.
 * @return  The smart integer.
 */
uint32_t RSBuffer::readSmart()
{
    return peek() >= 0 ? readShort() & 0xFFFFu : readInt() & 0x7FFFFFFFu;
}

/**
 * Reads a string from the buffer.
 * @return  The value.
 */
std::string RSBuffer::readString()
{
    std::stringstream stream;
    while (getRemaining() != 0)
    {
        auto b = readByte();
        if (b == 0)
            break;
        stream << static_cast<char>(b);
    }
    return stream.str();
}

/**
 * Reads a range of bytes from the buffer.
 * @param length    The number of bytes to read.
 * @return          The range.
 */
boost::iterator_range<const char*> RSBuffer::readRange(size_t length)
{
    auto range = boost::make_iterator_range(&buf_[readerIndex_], &buf_[readerIndex_ + length]);
    readerIndex_ += length;
    return range;
}