#include <sstream>
#include "linuxdeploy/desktopfile/desktopfile.h"

int main() {
    std::stringstream ss;
    ss << "[Desktop Entry]" << std::endl
       << "no spaces in key=foo" << std::endl;

    linuxdeploy::desktopfile::DesktopFile file(ss);
}
