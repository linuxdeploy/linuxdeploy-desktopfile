#pragma once

// system headers
#include <memory>
#include <string>
#include <vector>

namespace linuxdeploy {
    namespace desktopfile {
        class DesktopFileEntry {
        private:
            // opaque data class pattern
            class PrivateData;

            std::shared_ptr<PrivateData> d;

        public:
            // default constructor
            DesktopFileEntry();

            // construct from key and value
            explicit DesktopFileEntry(std::string key, std::string value);

            // copy constructor
            DesktopFileEntry(const DesktopFileEntry& other);

            // copy assignment constructor
            DesktopFileEntry& operator=(const DesktopFileEntry& other);

            // move assignment operator
            DesktopFileEntry& operator=(DesktopFileEntry&& other) noexcept;

            // equality operator
            bool operator==(const DesktopFileEntry& other) const;

            // inequality operator
            bool operator!=(const DesktopFileEntry& other) const;

        public:
            // checks whether a key and value have been set
            bool isEmpty() const;

            // return entry's key
            const std::string& key() const;

            // return entry's value
            const std::string& value() const;

            // sets a new value
            void setValue(const std::string& value);

        public:
            // convert value to integer
            // throws BadLexicalCastError in case of type errors
            int32_t asInt() const;

            // convert value to long
            // throws BadLexicalCastError in case of type errors
            int64_t asLong() const;

            // convert value to double
            // throws BadLexicalCastError in case of type errors
            double asDouble() const;

            // split CSV list value into vector
            // the separator used to split the string is a semicolon as per desktop file spec
            std::vector<std::string> parseStringList() const;
        };
    }
}
