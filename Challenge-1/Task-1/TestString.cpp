/**************************************************************************************************
 * @file TestString.cpp
 *
 * @brief This file is used to test the UserDefined::String class.
 * 
 * Note: I am ready to implement Unit Test as well using GTest and GMock framework.
 **************************************************************************************************/

#include "String.hpp"
#include <cassert>
#include <sstream>

namespace
{
    void printTestOutput(const char* testName, const UserDefined::String& str)
    {
        std::cout << testName << ": str = \"" << str << "\", length = " << str.length() << std::endl;
    }
}

int main() {
    // Test default constructor
    UserDefined::String s1;
    printTestOutput("Default constructor", s1);
    assert(s1.length() == 0);

    // Test parameterized constructor
    UserDefined::String s2("Hello");
    printTestOutput("Parameterized constructor", s2);
    assert(s2.length() == 5);
    assert(std::strcmp(s2.c_str(), "Hello") == 0);

    // Test parameterized constructor that takes a std::vector<char>
    std::vector<char> vec = {'W', 'o', 'r', 'l', 'd'};
    UserDefined::String s22(vec);
    printTestOutput("Parameterized constructor with vector<char>", s22);
    assert(s22.length() == 5);
    assert(std::strcmp(s22.c_str(), "World") == 0);

    // Test copy constructor
    UserDefined::String s3(s2);
    printTestOutput("Copy constructor", s3);
    assert(s3.length() == 5);
    assert(std::strcmp(s3.c_str(), "Hello") == 0);

    // Test move constructor
    UserDefined::String s4(std::move(s3));
    printTestOutput("Move constructor", s4);
    assert(s4.length() == 5);
    assert(std::strcmp(s4.c_str(), "Hello") == 0);

    // Test copy assignment operator
    UserDefined::String s5 = s2;
    printTestOutput("Copy assignment", s5);
    assert(s5.length() == 5);
    assert(std::strcmp(s5.c_str(), "Hello") == 0);

    // Test move assignment operator
    UserDefined::String s6 = std::move(s5);
    printTestOutput("Move assignment", s6);
    assert(s6.length() == 5);
    assert(std::strcmp(s6.c_str(), "Hello") == 0);

    // Test operator+
    UserDefined::String s7 = s2 + s6;
    printTestOutput("Operator+", s7);
    assert(s7.length() == 10);
    assert(std::strcmp(s7.c_str(), "HelloHello") == 0);

    // Test operator==
    bool equal = s2 == s6;
    std::cout << "Operator==: s2 is " << (equal ? "" : "not ") << "equal to s6" << std::endl;
    assert(equal);

    // Test operator<<
    std::ostringstream oss;
    oss << s2;
    std::cout << "Operator<<: oss = \"" << oss.str() << "\"" << std::endl;
    assert(oss.str() == "Hello");

    // Test operator>>
    std::istringstream iss("Hello");
    UserDefined::String s8;
    iss >> s8;
    printTestOutput("Operator>>", s8);
    assert(s8.length() == 5);
    assert(std::strcmp(s8.c_str(), "Hello") == 0);

    return 0;
}
