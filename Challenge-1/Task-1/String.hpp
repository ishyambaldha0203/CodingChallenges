/**************************************************************************************************
 * @file String.hpp
 *
 * @brief A simple string class.
 *
 * This file contains the definition of a simple string class that provides
 * basic string functionality similar to std::string. It supports dynamic
 * resizing and implements the RAII idiom.
 *
 **************************************************************************************************/

#ifndef STRING_HPP
#define STRING_HPP

#include <iostream>
#include <cstring>
#include <memory>
#include <algorithm>
#include <vector>

namespace UserDefined
{
    /**
     * @class String
     * @brief A simple string class.
     *
     * This class provides basic string functionality similar to std::string.
     * It supports dynamic resizing and implements the RAII idiom.
     */
    class String
    {
    public:

        // #region Constructors/Destruction

        /**
         * @brief Default constructor.
         *
         * Constructs an empty string.
         */
        String()
            : _strLength(0)
            , _strData(std::make_unique<char[]>(1))
        {
            _strData[0] = '\0';
        }

        /**
         * @brief Parameterized constructor.
         *
         * Constructs a string from a C-string.
         *
         * @param inputString The C-string to construct from.
         */
        String(const char* inputString)
            : _strLength(std::strlen(inputString))
            , _strData(std::make_unique<char[]>(_strLength + 1))
        {
            std::copy(inputString, inputString + _strLength, _strData.get());

            _strData[_strLength] = '\0';
        }

        /**
         * @brief Parameterized constructor.
         *
         * Constructs a string from a std::vector<char>.
         *
         * @param inputVector The std::vector<char> to construct from.
         */
        String(const std::vector<char>& inputVector)
            : _strLength(inputVector.size())
            , _strData(std::make_unique<char[]>(_strLength + 1))
        {
            std::copy(inputVector.begin(), inputVector.end(), _strData.get());
            _strData[_strLength] = '\0';
        }


        /**
         * @brief Copy constructor.
         *
         * Constructs a string as a copy of another string.
         *
         * @param sourceString The string to copy.
         */
        String(const String& sourceString)
            : _strLength(sourceString._strLength)
            , _strData(std::make_unique<char[]>(_strLength + 1))
        {
            std::copy(sourceString._strData.get(), sourceString._strData.get() + _strLength, _strData.get());

            _strData[_strLength] = '\0';
        }

        /**
         * @brief Move constructor.
         *
         * Constructs a string by taking ownership of the data of another string.
         *
         * @param sourceString The string to move.
         */
        String(String&& sourceString) noexcept
            : _strLength(sourceString._strLength)
            , _strData(std::move(sourceString._strData))
        {
            sourceString._strLength = 0;
        }

        /**
         * @brief Destroy the String object
         */
        ~String() = default;

        /**
         * @brief Copy assignment operator.
         *
         * Replaces the contents of the string with a copy of another string.
         *
         * @param sourceString The string to copy.
         * @return A reference to the string.
         */
        String& operator=(const String& sourceString)
        {
            if (this != &sourceString)
            {
                _strLength = sourceString._strLength;
                _strData = std::make_unique<char[]>(_strLength + 1);

                std::copy(sourceString._strData.get(), sourceString._strData.get() + _strLength, _strData.get());
                _strData[_strLength] = '\0';
            }
            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * Replaces the contents of the string by moving the data of another string.
         *
         * @param sourceString The string to move.
         * @return A reference to the string.
         */
        String& operator=(String&& sourceString) noexcept
        {
            if (this != &sourceString)
            {
                _strData = std::move(sourceString._strData);
                _strLength = sourceString._strLength;
                sourceString._strLength = 0;
            }
            return *this;
        }

        // #endregion

        // #region Overloaded Operators

        /**
         * @brief Addition (append) operator.
         *
         * Appends another string to the string.
         *
         * @param appendString The string to append.
         * @return A new string that is the concatenation of the string and the other string.
         */
        String operator+(const String& appendString)
        {
            String resultString;

            resultString._strLength = _strLength + appendString._strLength;
            resultString._strData = std::make_unique<char[]>(resultString._strLength + 1);

            std::copy(_strData.get(), _strData.get() + _strLength, resultString._strData.get());
            std::copy(appendString._strData.get(), appendString._strData.get() + appendString._strLength, resultString._strData.get() + _strLength);
            resultString._strData[resultString._strLength] = '\0';

            return resultString;
        }

        /**
         * @brief Comparison operator.
         *
         * Compares the string to another string for equality.
         *
         * @param compareString The string to compare to.
         * @return true if the strings are equal, false otherwise.
         */
        bool operator==(const String& compareString) const
        {
            return _strLength == compareString._strLength && std::equal(_strData.get(), _strData.get() + _strLength, compareString._strData.get());
        }

        // #endregion

        // #region Public Methods

        /**
         * @brief Gets the length of the string.
         *
         * @return The length of the string.
         */
        std::size_t length() const
        {
            return _strLength;
        }

        /**
         * @brief Gets the string as a C-string. Equivalent to c_str() from std::string.
         *
         * @return The string as a C-string.
         */
        const char* c_str() const
        {
            return _strData.get();
        }

        // #endregion

        /**
         * @brief Inserts the string into an output stream.
         *
         * @param outputStream The output stream.
         * @param outputString The string to insert.
         *
         * @return The output stream.
         */
        friend std::ostream& operator<<(std::ostream& outputStream, const String& outputString);

        /**
         * @brief Extracts a string from an input stream.
         *
         * @param inputStream The input stream.
         * @param inputString The string to extract to.
         *
         * @return The input stream.
         */
        friend std::istream& operator>>(std::istream& inputStream, String& inputString);

    private:
        std::size_t _strLength;              ///< The length of the string.
        std::unique_ptr<char[]> _strData;    ///< The string data.
    };

    std::ostream& operator<<(std::ostream& outputStream, const String& outputString)
    {
        outputStream << outputString.c_str();
        return outputStream;
    }

    std::istream& operator>>(std::istream& inputStream, String& inputString)
    {
        char ch;
        std::size_t len = 0;
        std::size_t capacity = 16;
        char* buffer = new char[capacity];

        while (inputStream.get(ch) && ch != '\n')
        {
            if (len == capacity - 1)
            {
                capacity *= 2;
                char* newBuffer = new char[capacity];
                std::copy(buffer, buffer + len, newBuffer);
                delete[] buffer;
                buffer = newBuffer;
            }

            buffer[len] = ch;
            len++;
        }

        buffer[len] = '\0';
        inputString._strLength = len;
        inputString._strData.reset(buffer);

        return inputStream;
    }
}

#endif // STRING_HPP
