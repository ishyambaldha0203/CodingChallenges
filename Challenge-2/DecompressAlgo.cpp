/**************************************************************************************************
 * @file DecompressAlgo.cpp
 *
 * @brief This file contains string decompressing algorithm.
 **************************************************************************************************/

#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>

#include <sstream>

using namespace std;

bool Decompress(const std::string& inStr, std::string& outStr)
{
    int inputStrLen = inStr.size();
	int lastIndex = inputStrLen - 1;
    int N = 0; 							// Initialize repeat count.
    bool repeatSequence = false; 		// Flag to check if we are in a repeat sequence.
    std::stringstream outStrStream; 	// Create an output string stream.
    std::stringstream subStringStream; 	// String stream to hold the substring to be repeated.

    // Iterate over the input string.
    for (int i = 0 ; i < inputStrLen ; i++)
    {          
        // If the current character is an escape character.
        if((inStr[i] == '\\'))
        {
            // If the next character is a valid escape sequence.
            if (i < lastIndex && (inStr[i+1] == '\\' || inStr[i+1] == '[' || inStr[i+1] == ']'))
            {
				// Jump on to the next character to fetch actual character to be appended to output.
                i++;

                // If we are in a repeat sequence, append to the substring stream.
                if(repeatSequence)
                {
                    subStringStream << inStr[i];
                }
                else // Else, append to the output string stream.
                {
                    outStrStream << inStr[i];
                }
            }
            else // If the next character is not a valid escape sequence, return false.
            {
                return false;
            }
        }
        // If the current character is a digit and the next character is an opening bracket.
        else if(std::isdigit(inStr[i]) && (i < lastIndex && inStr[i+1] == '['))
        {
            N = inStr[i] - '0'; // Set the repeat count.
            repeatSequence = true; // Set the flag to indicate that we are in a repeat sequence.
            
            // Jump digit and opening bracket will be skipped by main for loop ++i.
            i = i + 1;
        }
        // If the current character is a closing bracket.
        else if(inStr[i] == ']')
        {
            // If we are in a repeat sequence.
            if(repeatSequence)
            {
                // Get the substring to be repeated.
                std::string subString = subStringStream.str();
                
                // Clear the substring stream for the next repeat sequence.
                subStringStream.str(std::string());

                // Repeat the substring N times and append to the output string.
                for(int z = 0 ; z < N ; z++)
                {
                    outStrStream << subString;
                }

                // Reset the repeat sequence flag and repeat count.
                repeatSequence = false;
                N = 0;
            }
            else // If we are not in a repeat sequence, return false.
            {
                return false;
            }
        }
        // If the current character is an opening bracket, return false.
        else if(inStr[i] == '[')
        {
           return false;
        }
        else // For all other characters
        {
            // If we are in a repeat sequence, append to the substring stream.
            if(repeatSequence)
            {
                subStringStream << inStr[i];
            }
            else // Else, append to the output string stream.
            {
                outStrStream << inStr[i];
            }
        }
   }
   
   // If closing bracket is never encountered, return false.
   if(repeatSequence)
   {
       return false;
   }
   
   // Set the output string to the contents of the output string stream.
   outStr = outStrStream.str();

   return true;
}

/* ===================================================================== */

void DecompressTest()
{
   // Place a marker after the end of the string to help detect overruns
   #define TEST_STRING(s) s "\0*********************\0\0\0\0\0\0\0"

   struct
   {
      const char* compressedStr;
      const char* expectedStr;
      bool expectedResult;
   }

   testCases[] =
   {
      { TEST_STRING(""),                  "",               true  },
      { TEST_STRING("Hello"),             "Hello",          true  },
      { TEST_STRING("5[x]"),              "xxxxx",          true  },
      { TEST_STRING("15[x]"),             "1xxxxx",         true  },
      { TEST_STRING("0[x]0"),             "0",              true  },
      { TEST_STRING("!9[]01"),            "!01",            true  },
      { TEST_STRING("\\\\a"),             "\\a",            true  },
      { TEST_STRING("\\[3[-]\\]"),        "[---]",          true  },
      { TEST_STRING("123He2[l]o321"),     "123Hello321",    true  },
      { TEST_STRING("0[hello]"),          "",               true  },
      { TEST_STRING("a5b2[cd]3[ef]g"),    "a5bcdcdefefefg", true  },
      { TEST_STRING("1[12]2[\\[X\\]]"),   "12[X][X]",       true  },
      { TEST_STRING("\\"),                "",   false }, // Invalid escape character at end of string
      { TEST_STRING("\\a"),               "",   false }, // Invalid escape character at start of string
      { TEST_STRING("he\\llo"),           "",   false }, // Invalid escape character in middle of string
      { TEST_STRING("hel1234567[lo"),     "",   false }, // No ending square bracket
      { TEST_STRING("hello]"),            "",   false }, // No starting square bracket
      { TEST_STRING("[hello]"),           "",   false }, // No repeat count given
      { TEST_STRING("["),                 "",   false }, // No repeat count, no ending square bracket
      { TEST_STRING("]"),                 "",   false }, // No starting square bracket
      { TEST_STRING("0[1[x]]"),           "",   false }, // Nested repeat sequences not allowed
   };

   int testFailed = 0;
   int n = sizeof(testCases) / sizeof(testCases[0]);
   for(int i=0; i < n; ++i)
   {
      bool testPassed = true;
      std::string actualStr;
      bool actualResult = Decompress(testCases[i].compressedStr, actualStr);

      if(actualResult != testCases[i].expectedResult)
      {
         testFailed++;
         testPassed = false;

      }
      else if(actualResult && actualStr != testCases[i].expectedStr)
      {
         testFailed++;
         testPassed = false;
      }

      printf("%d) Decompress(\"%s\") -> \"%s\"(%s) - %s\n",
                i+1,
                testCases[i].compressedStr,
                actualStr.c_str(),
                actualResult ? "TRUE" : "FALSE",
                testPassed ? "PASS" : "****");
   }

   if(testFailed == 0)
   {
      printf("\nAll tests passed\n");
   }
   else
   {
      printf("\n%d test%s did not pass\n",
            testFailed, (testFailed == 1 ? "" : "s"));
   }
}

int main (int, char**)
{
   DecompressTest();
}
