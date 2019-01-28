// library headers
#include <gtest/gtest.h>

// local headers
#include "../src/desktopfilereader.h"
#include "linuxdeploy/desktopfile/exceptions.h"

using namespace linuxdeploy::desktopfile;

class DesktopFileReaderTest : public ::testing::Test {
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(DesktopFileReaderTest, testDefaultConstructor) {
    DesktopFileReader reader;
    EXPECT_TRUE(reader.isEmpty());
}

TEST_F(DesktopFileReaderTest, testPathConstructor) {
    const std::string path = "/dev/null";

    DesktopFileReader reader(path);
    EXPECT_TRUE(reader.isEmpty());

    EXPECT_THROW(DesktopFileReader("/no/such/file/or/directory"), IOError);
}

TEST_F(DesktopFileReaderTest, testStreamConstructor) {
    std::stringstream ss;
    DesktopFileReader reader(ss);

    EXPECT_TRUE(reader.isEmpty());
}

TEST_F(DesktopFileReaderTest, testPathConstructorWithEmptyPath) {
    EXPECT_THROW(DesktopFileReader(""), IOError);
}

TEST_F(DesktopFileReaderTest, testPathConstructorWithNonExistingPath) {
    EXPECT_THROW(DesktopFileReader("/no/such/path/42"), IOError);
}

TEST_F(DesktopFileReaderTest, testEqualityAndInequalityOperators) {
    {
        DesktopFileReader emptyReader;
        EXPECT_TRUE(emptyReader == emptyReader);
        EXPECT_FALSE(emptyReader != emptyReader);
    }

    {
        // make sure that files with different paths are recognized as different
        DesktopFile nullFile("/dev/null");
        DesktopFile fileWithoutPath;

        EXPECT_TRUE(nullFile != fileWithoutPath);
        EXPECT_FALSE(nullFile == fileWithoutPath);
    }

    {
        // make sure that files with different contents are recognized as different
        DesktopFile fileWithContents;
        fileWithContents.setEntry("Desktop Entry", DesktopFileEntry("test", "test"));

        DesktopFile emptyFile;

        EXPECT_TRUE(emptyFile != fileWithContents);
        EXPECT_FALSE(emptyFile == fileWithContents);
    }
}

TEST_F(DesktopFileReaderTest, testCopyConstructor) {
    {
        const std::string path = "/dev/null";

        DesktopFileReader reader(path);
        EXPECT_TRUE(reader.isEmpty());

        DesktopFileReader copy = reader;
        EXPECT_TRUE(copy.isEmpty());

        EXPECT_EQ(reader, copy);
    }

    {
        // make sure that contents are copied, too
        DesktopFile file;
        file.setEntry("Desktop Entry", DesktopFileEntry("test", "test"));

        std::stringstream ss;
        file.save(ss);

        DesktopFileReader reader(ss);
        DesktopFileReader copy(reader);

        EXPECT_TRUE(reader.data() == copy.data());
        EXPECT_TRUE(reader == copy);
    }
}

TEST_F(DesktopFileReaderTest, testCopyAssignmentConstructor) {
    const std::string path = "/dev/null";

    DesktopFileReader reader;
    EXPECT_TRUE(reader.isEmpty());

    DesktopFileReader otherReader(path);
    EXPECT_TRUE(otherReader.isEmpty());

    reader = otherReader;
    EXPECT_EQ(reader.path(), path);

    // test self-assignment
    reader = reader;
}

TEST_F(DesktopFileReaderTest, testMoveAssignmentConstructor) {
    const std::string path = "/dev/null";

    DesktopFileReader reader;
    EXPECT_TRUE(reader.isEmpty());

    DesktopFileReader otherReader(path);
    EXPECT_TRUE(otherReader.isEmpty());

    reader = std::move(otherReader);
    EXPECT_EQ(reader.path(), path);

    // test self-assignment
    reader = std::move(reader);
}

TEST_F(DesktopFileReaderTest, testParseSimpleDesktopFile) {
    std::stringstream ss;
    ss << "[Desktop Entry]" << std::endl
       << "Version=1.0" << std::endl
       << "Name=name" << std::endl
       << "Exec=exec" << std::endl
       << "Icon=icon" << std::endl
       << "Type=Application" << std::endl
       << "Categories=Utility;Multimedia;" << std::endl;

    DesktopFileReader reader;
    reader = DesktopFileReader(ss);

    auto section = reader["Desktop Entry"];
    EXPECT_FALSE(section.empty());
    EXPECT_EQ(section.size(), 6);

    EXPECT_NEAR(section["Version"].asDouble(), 1.0, 0.000001);
    EXPECT_EQ(section["Name"].value(), "name");
    EXPECT_EQ(section["Exec"].value(), "exec");
    EXPECT_EQ(section["Icon"].value(), "icon");
    EXPECT_EQ(section["Type"].value(), "Application");
    EXPECT_EQ(section["Name"].value(), "name");
    EXPECT_EQ(section["Categories"].parseStringList(), std::vector<std::string>({"Utility", "Multimedia"}));
}

TEST_F(DesktopFileReaderTest, testParseSimpleDesktopFileWithComments) {
    std::stringstream ss;
    ss << "[Desktop Entry]" << std::endl
       << "Version=1.0" << std::endl
       << "Name=name" << std::endl
       << "# a comment" << std::endl
       << "Exec=exec" << std::endl
       << "Icon=icon" << std::endl
       << "Type=Application" << std::endl
       << "# another comment" << std::endl
       << "Categories=Utility;Multimedia;" << std::endl;

    DesktopFileReader reader;
    reader = DesktopFileReader(ss);

    auto section = reader["Desktop Entry"];
    EXPECT_FALSE(section.empty());
    EXPECT_EQ(section.size(), 6);

    // TODO: check for comments in data
    // right now, they're just discarded, but they shall be preserved in the right positions in the future

    EXPECT_NEAR(section["Version"].asDouble(), 1.0f, 0.000001);
    EXPECT_EQ(section["Name"].value(), "name");
    EXPECT_EQ(section["Exec"].value(), "exec");
    EXPECT_EQ(section["Icon"].value(), "icon");
    EXPECT_EQ(section["Type"].value(), "Application");
    EXPECT_EQ(section["Name"].value(), "name");
    EXPECT_EQ(section["Categories"].parseStringList(), std::vector<std::string>({"Utility", "Multimedia"}));
}

TEST_F(DesktopFileReaderTest, testParseFileGetNonExistingSection) {
    std::stringstream ss;
    ss << "[Desktop Entry]" << std::endl;

    DesktopFileReader reader;
    reader = DesktopFileReader(ss);

    EXPECT_THROW(reader["Non-existing Section"], UnknownSectionError);
}

TEST_F(DesktopFileReaderTest, testParseFileMissingSectionHeader) {
    std::stringstream ss;
    ss << "Name=name" << std::endl
       << "Exec=exec" << std::endl
       << "Icon=icon" << std::endl
       << "Type=Application" << std::endl
       << "Categories=Utility;Multimedia;" << std::endl;

    DesktopFileReader reader;
    EXPECT_THROW(reader = DesktopFileReader(ss), ParseError);
}

TEST_F(DesktopFileReaderTest, testParseFileEmptyKey) {
    std::stringstream ss;
    ss << "[Desktop Entry]" << std::endl
       << "=name" << std::endl
       << "Exec=exec" << std::endl
       << "Icon=icon" << std::endl
       << "Type=Application" << std::endl
       << "Categories=Utility;Multimedia;" << std::endl;

    EXPECT_THROW(DesktopFileReader reader = DesktopFileReader(ss), ParseError);
}

TEST_F(DesktopFileReaderTest, testParseFileMissingDelimiterInLine) {
    {
        std::stringstream ss;
        ss << "[Desktop Entry]" << std::endl
           << "Exec" << std::endl;

        DesktopFileReader reader;
        EXPECT_THROW(reader = DesktopFileReader(ss), ParseError);
    }

    {
        std::stringstream ss;
        ss << "[Desktop Entry]" << std::endl
           << "Name name" << std::endl;

        DesktopFileReader reader;
        EXPECT_THROW(reader = DesktopFileReader(ss), ParseError);
    }
}

TEST_F(DesktopFileReaderTest, testParseFile) {
    std::stringstream ss;
    ss << "[Desktop Entry]" << std::endl
       << "Name name" << std::endl
       << "Exec" << std::endl;

    DesktopFileReader reader;
    EXPECT_THROW(reader = DesktopFileReader(ss), ParseError);
}

TEST_F(DesktopFileReaderTest, testParseFileMultipleDelimitersInLine) {
    // TODO: verify that ==Name would be a legal value according to desktop file specification
    std::stringstream ss;
    ss << "[Desktop Entry]" << std::endl
       << "Name===name" << std::endl;

    DesktopFileReader reader;
    ASSERT_NO_THROW(reader = DesktopFileReader(ss));
}

TEST_F(DesktopFileReaderTest, testParseFileWithLeadingAndTrailingWhitespaceInLines) {
    std::stringstream ss;
    ss << "[Desktop Entry]" << std::endl
       << "Name= name" << std::endl
       << "Exec =exec" << std::endl;

    DesktopFileReader reader(ss);

    auto section = reader["Desktop Entry"];
    EXPECT_FALSE(section.empty());

    EXPECT_EQ(section["Name"].value(), "name");
    EXPECT_EQ(section["Exec"].value(), "exec");
}

TEST_F(DesktopFileReaderTest, testDataGetter) {
    std::stringstream ss;
    ss << "[Desktop Entry]" << std::endl
       << "Name= name" << std::endl
       << "Exec =exec" << std::endl;

    DesktopFileReader reader(ss);

    auto section = reader["Desktop Entry"];
    EXPECT_FALSE(section.empty());

    auto data = reader.data();

    auto expected = DesktopFile::section_t({
        {"Name", DesktopFileEntry("Name", "name")},
        {"Exec", DesktopFileEntry("Exec", "exec")},
    });

    EXPECT_EQ(data["Desktop Entry"], expected);
}

TEST_F(DesktopFileReaderTest, testParseLinesWithMultipleSpaces) {
    std::stringstream ss;
    ss << "[Desktop Entry]" << std::endl
       << "Name= What a great  name    " << std::endl;

    DesktopFileReader reader(ss);

    auto section = reader["Desktop Entry"];
    EXPECT_FALSE(section.empty());

    EXPECT_EQ(section["Name"].value(), "What a great  name");
}

TEST_F(DesktopFileReaderTest, testReadBrokenSectionHeaderMissingClosingBracket) {
    {
        std::stringstream ins;
        ins << "[Desktop Entry" << std::endl
            << "test=test" << std::endl;

        EXPECT_THROW(DesktopFileReader reader(ins), ParseError);
    }

    // also test for brokenness in a later section, as the first section is normally treated specially
    {
        std::stringstream ins;
        ins << "[Desktop Entry]" << std::endl
            << "test=test" << std::endl
            << "[Another Section" << std::endl;

        EXPECT_THROW(DesktopFileReader reader(ins), ParseError);
    }
}

TEST_F(DesktopFileReaderTest, testReadBrokenSectionHeaderTooManyClosingBrackets) {
    {
        std::stringstream ins;
        ins << "[Desktop Entry]]" << std::endl
            << "test=test" << std::endl;

        EXPECT_THROW(DesktopFileReader reader(ins), ParseError);
    }

    // also test for brokenness in a later section, as the first section is normally treated specially
    {
        std::stringstream ins;
        ins << "[Desktop Entry]" << std::endl
            << "test=test" << std::endl
            << "[Another Section]]" << std::endl;

        EXPECT_THROW(DesktopFileReader reader(ins), ParseError);
    }
}

TEST_F(DesktopFileReaderTest, testReadBrokenSectionHeaderTooManyOpeningBrackets) {
    {
        std::stringstream ins;
        ins << "[[Desktop Entry]" << std::endl
            << "test=test" << std::endl;

        EXPECT_THROW(DesktopFileReader reader(ins), ParseError);
    }

    // also test for brokenness in a later section, as the first section is normally treated specially
    {
        std::stringstream ins;
        ins << "[Desktop Entry]" << std::endl
            << "test=test" << std::endl
            << "[[Another Section]";

        EXPECT_THROW(DesktopFileReader reader(ins), ParseError);
    }
}

TEST_F(DesktopFileReaderTest, testReadBrokenSectionMissingOpeningBracket) {
    {
        std::stringstream ins;
        ins << "Desktop Entry]" << std::endl
            << "test=test" << std::endl;
        EXPECT_THROW(DesktopFileReader reader(ins), ParseError);
    }

    // also test for brokenness in a later section, as the first section is normally treated specially
    {
        std::stringstream ins;
        ins << "[Desktop Entry]" << std::endl
            << "test=test" << std::endl
            << "Another Section]" << std::endl;

        EXPECT_THROW(DesktopFileReader reader(ins), ParseError);
    }
}

// FIXME: introduce proper localization support
TEST_F(DesktopFileReaderTest, testReadLocalizedEntriesWithoutProperLocalizationSupport) {
    // lang_COUNTRY.ENCODING@MODIFIER
    // https://standards.freedesktop.org/desktop-entry-spec/latest/ar01s05.html
    for (std::string locale : {"de", "de_DE", "ca@valencia", "de_DE.UTF-8"}) {
        std::stringstream ss;
        ss << "[Desktop Entry]" << std::endl
           << "Name=name" << std::endl
           << "Name[" << locale << "]=name" << std::endl
           << "Exec=exec" << std::endl;

        DesktopFileReader reader;

        EXPECT_NO_THROW(reader = DesktopFileReader(ss)) << "tested locale string: " << locale;

        auto section = reader["Desktop Entry"];
        EXPECT_FALSE(section.empty());

        auto data = reader.data();

        auto expected = DesktopFile::section_t({
            {"Name",     DesktopFileEntry("Name", "name")},
            // FIXME: revise after introduction of localization support
            {"Name[" + locale + "]", DesktopFileEntry("Name[" + locale + "]", "name")},
            {"Exec",     DesktopFileEntry("Exec", "exec")},
        });

        EXPECT_EQ(data["Desktop Entry"], expected) << "tested locale string: " << locale;
    }
}

TEST_F(DesktopFileReaderTest, testBrokenLocalizedKeys) {
    for (std::string key : {"test]de[", "test[de]]", "test[[de]", "test[]de", "test[de", "testde]"}) {
        std::stringstream ins;
        ins << "[Desktop Entry]" << std::endl
            << key << "=test" << std::endl;

        EXPECT_THROW(DesktopFileReader reader(ins), ParseError) << "key: " << key;
    }
}
