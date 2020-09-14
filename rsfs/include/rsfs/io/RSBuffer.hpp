#pragma once
#include <boost/range/iterator_range.hpp>

#include <fstream>
#include <sstream>
#include <vector>

namespace rsfs
{
    /**
     * A RuneScape specific byte buffer implementation.
     */
    class RSBuffer
    {
    public:
        /**
         * Initialises a buffer from a filestream.
         * @param stream    The file stream to read from.
         */
        explicit RSBuffer(std::ifstream& stream);

        /**
         * Initialises an empty buffer with a fixed size.
         * @param size  The desired size of the buffer.
         */
        explicit RSBuffer(size_t size = 512);

        /**
         * Initialises a buffer from an array.
         * @param buf   The pointer to the start of the array
         * @param size  The size of the array
         */
        RSBuffer(const char* buf, size_t size);

        /**
         * Moves the reader to a specified position in the buffer.
         * @param pos   The new reader position
         */
        void seek(size_t pos);

        /**
         * Resets the reader index of this buffer.
         * @return  This buffer
         */
        RSBuffer& resetReaderIndex();

        /**
         * Resizes this buffer.
         * @param length    The new length.
         */
        void resize(size_t length);

        /**
         * Writes a single byte to the buffer
         * @param value     The value to write
         */
        void writeByte(char value);

        /**
         * Writes a series of bytes to the buffer.
         * @param buf   The source array.
         * @param size  The size of the array.
         */
        void writeBytes(const char* buf, size_t size);

        /**
         * Writes a range of bytes to the buffer.
         * @param range The range of bytes.
         */
        void writeBytes(boost::iterator_range<const char*> range);

        /**
         * Writes an integer to the buffer.
         * @param value The value to write.
         */
        void writeInt(int32_t value);

        /**
         * Gets the value at the current offset, but doesn't advance the reader.
         * @return  The current byte value
         */
        [[nodiscard]] char peek() const
        {
            return buf_.at(readerIndex_);
        }

        /**
         * Reads a single byte from the buffer.
         * @return  The value.
         */
        [[nodiscard]] uint8_t readByte();

        /**
         * Reads a series of bytes from the buffer.
         * @param size  The number of bytes to read.
         * @return      The buffer that was read.
         */
        [[nodiscard]] RSBuffer readBytes(size_t size);

        /**
         * Reads a two-byte integer from the buffer.
         * @return  The value.
         */
        [[nodiscard]] uint16_t readShort();

        /**
         * Reads a three-byte integer from the buffer.
         * @return  The value.
         */
        [[nodiscard]] uint32_t readTriByte();

        /**
         * Reads a four-byte integer from the buffer.
         * @return  The value.
         */
        [[nodiscard]] uint32_t readInt();

        /**
         * Reads either a short or an integer, depending on if the first byte is 0 or not.
         * @return  The smart integer.
         */
        [[nodiscard]] uint32_t readSmart();

        /**
         * Reads a string from the buffer.
         * @return  The string.
         */
        [[nodiscard]] std::string readString();

        /**
         * Gets the size of the buffer.
         * @return  The size of the buffer
         */
        [[nodiscard]] size_t getSize() const
        {
            return buf_.size();
        }

        /**
         * Reads a range of bytes from the buffer.
         * @param length    The number of bytes to read.
         * @return          The range.
         */
        [[nodiscard]] boost::iterator_range<const char*> readRange(size_t length);

        /**
         * Get a pointer to the beginning of the buffer.
         * @return  The beginning of the buffer.
         */
        [[nodiscard]] const char* begin() const
        {
            return buf_.begin().base();
        }

        /**
         * Gets a pointer to the end of the buffer.
         * @return  The end of the buffer.
         */
        [[nodiscard]] const char* end() const
        {
            return buf_.end().base();
        }

        /**
         * Gets the remaining number of bytes that can be read.
         * @return  The remaining number of bytes.
         */
        [[nodiscard]] size_t getRemaining() const
        {
            return buf_.size() - readerIndex_;
        }

    private:
        /**
         * The internal buffer
         */
        std::vector<char> buf_;

        /**
         * The index of the reader
         */
        size_t readerIndex_{ 0 };
    };
}