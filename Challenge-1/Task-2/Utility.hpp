/**
 * @file Utility.hpp
 * @brief This file contains the Utility class with static helper functions.
 */

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

/**
 * @class Utility
 *
 * @brief A class that provides static utility functions.
 */
namespace Utility
{
    /**
     * @brief Gets the current time as a string.
     *
     * @return The current time as a string in the format "YYYY-MM-DD HH:MM:SS".
     */
    static std::string getCurrentTime()
    {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%F %T");
        return ss.str();
    }

    /**
     * @brief Logs a message with the current time and function name.
     *
     * @param functionName The name of the function from which the log is being made.
     * @param message The log message.
     */
    [[maybe_unused]]static void log(const std::string& functionName, const std::string& message)
    {
        std::cout << "[" << getCurrentTime() << "][" << functionName << "] " << message << std::endl;
    }

    /**
     * @brief Logs a message with the current time, file name, and function name.
     *
     * @param fileName The name of the file from which the log is being made.
     * @param functionName The name of the function from which the log is being made.
     * @param message The log message.
     */
    static void log(const std::string& fileName, const std::string& functionName, const std::string& message)
    {
        std::cout << "[" << getCurrentTime() << "][" << fileName << "][" << functionName << "] " << message << std::endl;
    }

    #define LOG(message) Utility::log(__FILE__, __func__, (message))
};

#endif // UTILITY_HPP
