// system headers
#include <cassert>
#include <fstream>

// local headers
#include "linuxdeploy/desktopfile/exceptions.h"
#include "linuxdeploy/desktopfile/desktopfile.h"
#include "desktopfilereader.h"
#include "desktopfilewriter.h"

namespace linuxdeploy {
    namespace desktopfile {
        class DesktopFile::PrivateData {
            public:
                std::string path;
                sections_t data;

            public:
                PrivateData() = default;

                void copyData(const std::shared_ptr<PrivateData>& other) {
                    path = other->path;
                    data = other->data;
                }

        public:
            bool isEmpty() const {
                return data.empty();
            }
        };

        DesktopFile::DesktopFile() : d(std::make_shared<PrivateData>()) {}

        DesktopFile::DesktopFile(const std::string& path) : DesktopFile() {
            // if the file doesn't exist, an exception shall be thrown
            // otherwise, a user cannot know for sure whether a file was actually read (would need to check this
            // manually beforehand
            {
                std::ifstream ifs(path);
                if (!ifs) {
                    throw IOError("Could not find file " + path);
                }
            }

            // will throw exceptions in case of issues
            read(path);
        };

        DesktopFile::DesktopFile(std::istream& is) : DesktopFile() {
            // will throw exceptions in case of issues
            read(is);
        };

        // copy constructor
        DesktopFile::DesktopFile(const DesktopFile& other) : DesktopFile() {
            d->copyData(other.d);
        }

        // copy assignment constructor
        DesktopFile& DesktopFile::operator=(const DesktopFile& other) {
            if (this != &other) {
                d->copyData(other.d);
            }

            return *this;
        }

        // move assignment operator
        DesktopFile& DesktopFile::operator=(DesktopFile&& other) noexcept {
            if (this != &other) {
                d = other.d;
                other.d = nullptr;
            }
            return *this;
        }

        void DesktopFile::read(const std::string& path) {
            setPath(path);

            // clear data before reading a new file
            clear();

            DesktopFileReader reader(path);
            d->data = std::move(reader.data());
        }

        void DesktopFile::read(std::istream& is) {
            // clear data before reading a new file
            clear();

            DesktopFileReader reader(is);
            d->data = reader.data();
        }

        std::string DesktopFile::path() const {
            return d->path;
        }

        void DesktopFile::setPath(const std::string& path) {
            d->path = path;
        }

        bool DesktopFile::isEmpty() const {
            return d->isEmpty();
        }

        void DesktopFile::clear() {
            d->data.clear();
        }

        bool DesktopFile::save() const {
            return save(d->path);
        }

        bool DesktopFile::save(const std::string& path) const {
            DesktopFileWriter writer(d->data);
            writer.save(path);

            return true;
        }

        bool DesktopFile::save(std::ostream& os) const {
            DesktopFileWriter writer(d->data);
            writer.save(os);

            return true;
        }

        bool DesktopFile::entryExists(const std::string& section, const std::string& key) const {
            auto it = d->data.find(section);
            if (it == d->data.end())
                return false;

            return (it->second.find(key) != it->second.end());
        }

        bool DesktopFile::setEntry(const std::string& section, const DesktopFileEntry& entry) {
            // check if value exists -- used for return value
            auto rv = entryExists(section, entry.key());

            d->data[section][entry.key()] = entry;

            return rv;
        }

        bool DesktopFile::setEntry(const std::string& section, DesktopFileEntry&& entry) {
            // check if value exists -- used for return value
            auto rv = entryExists(section, entry.key());

            d->data[section][entry.key()] = entry;

            return rv;
        }

        bool DesktopFile::getEntry(const std::string& section, const std::string& key, DesktopFileEntry& entry) const {
            if (!entryExists(section, key))
                return false;

            entry = d->data[section][key];

            // make sure keys are equal
            assert(key == entry.key());

            return true;
        }

        bool DesktopFile::validate() const {
            // FIXME: call desktop-file-validate
            return true;
        }

        bool operator==(const DesktopFile& first, const DesktopFile& second) {
            return first.d->path == second.d->path && first.d->data == second.d->data;
        }

        bool operator !=(const DesktopFile& first, const DesktopFile& second) {
            return !operator==(first, second);
        }
    }
}



