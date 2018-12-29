// system
#include <sstream>

// library headers
#include <gtest/gtest.h>


// local
#include "linuxdeploy/desktopfile/desktopexecentrytokenizer.h"


using namespace linuxdeploy::desktopfile;

TEST(DesktopExecEntryTokenizerTest, iterateSingleSectionEntry) {
    DesktopExecEntryTokenizer tokenizer("echo");
    ASSERT_TRUE(tokenizer.next());
}

TEST(DesktopExecEntryTokenizerTest, iterateSimpleEntry) {
    DesktopExecEntryTokenizer tokenizer("echo %F");
    ASSERT_TRUE(tokenizer.next());

    ASSERT_EQ(tokenizer.section(), "echo");
    ASSERT_EQ(tokenizer.sectionBegin(), 0);
    ASSERT_EQ(tokenizer.sectionSize(), 4);

    ASSERT_TRUE(tokenizer.next());

    ASSERT_EQ(tokenizer.section(), "%F");
    ASSERT_EQ(tokenizer.sectionBegin(), 5);
    ASSERT_EQ(tokenizer.sectionSize(), 2);

    ASSERT_FALSE(tokenizer.next());
}

TEST(DesktopExecEntryTokenizerTest, iterateQuotedEntry) {
    DesktopExecEntryTokenizer tokenizer("\"/opt/custom apps/app\" %F --force=\"Rouge \\$1\"");

    ASSERT_TRUE(tokenizer.next());

    ASSERT_EQ(tokenizer.section(), "\"/opt/custom apps/app\"");
    ASSERT_EQ(tokenizer.sectionBegin(), 0);
    ASSERT_EQ(tokenizer.sectionSize(), 22);

    ASSERT_TRUE(tokenizer.next());

    ASSERT_EQ(tokenizer.section(), "%F");
    ASSERT_EQ(tokenizer.sectionBegin(), 23);
    ASSERT_EQ(tokenizer.sectionSize(), 2);

    ASSERT_TRUE(tokenizer.next());

    ASSERT_EQ(tokenizer.section(), "--force=\"Rouge \\$1\"");
    ASSERT_EQ(tokenizer.sectionBegin(), 26);
    ASSERT_EQ(tokenizer.sectionSize(), 19);

    ASSERT_FALSE(tokenizer.next());
}
