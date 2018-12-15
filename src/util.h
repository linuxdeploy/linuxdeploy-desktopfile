#pragma once

// system headers
#include <algorithm>
#include <string>

// local headers
#include "linuxdeploy/desktopfile/exceptions.h"

namespace linuxdeploy {
    namespace desktopfile {
        /**
         * Remove leading characters from string.
         * @param s string to edit
         * @param to_trim character to remove
         */
        static inline bool ltrim(std::string& s, char to_trim = ' ') {
            // TODO: find more efficient way to check whether elements have been removed
            size_t initialLength = s.length();
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [to_trim](int ch) {
                return ch != to_trim;
            }));
            return s.length() < initialLength;
        }

        /**
         * Remove trailing characters from string.
         * @param s string to edit
         * @param to_trim character to remove
         */
        static inline bool rtrim(std::string& s, char to_trim = ' ') {
            // TODO: find more efficient way to check whether elements have been removed
            auto initialLength = s.length();
            s.erase(std::find_if(s.rbegin(), s.rend(), [to_trim](int ch) {
                return ch != to_trim;
            }).base(), s.end());
            return s.length() < initialLength;
        }

        /**
         * Remove leading and trailing characters from string.
         * @param s string to edit
         * @param to_trim character to remove
         */
        static inline bool trim(std::string& s, char to_trim = ' ') {
            // returns true if either modifies s
            auto ltrim_result = ltrim(s, to_trim);
            return rtrim(s, to_trim) && ltrim_result;
        }

        /**
         * Relatively inefficient implementation of a C++ lexical cast.
         * @tparam To type to convert to
         * @tparam From input parameter type
         * @param from value to convert
         * @return converted value
         * @throws BadLexicalCastError in case a type conversion is not possible
         */
        template<typename To, typename From>
        To lexicalCast(From from) {
            To to;

            std::stringstream ss;
            ss << from;

            if (ss.fail())
                throw BadLexicalCastError();

            ss >> to;

            if (ss.fail())
                throw BadLexicalCastError();

            return to;
        }
    }
}
