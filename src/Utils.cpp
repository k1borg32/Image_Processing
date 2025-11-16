#include "Utils.h"
#include <cctype>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <string>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace {

std::string ensureBmpExtension(const std::string& path) {
    if (path.size() >= 4) {
        std::string ext = path.substr(path.size() - 4);
        for (char& ch : ext) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        if (ext == ".bmp") {
            return path;
        }
    }
    return path + ".bmp";
}

void createDirectories(const std::string& absolutePath) {
    const std::size_t pos = absolutePath.find_last_of("/\\");
    if (pos == std::string::npos) return;
    std::string dir = absolutePath.substr(0, pos);
    if (dir.empty()) return;

    std::size_t cursor = 0;
#ifdef _WIN32
    if (dir.size() >= 2 && dir[1] == ':') {
        cursor = 2;
    }
#endif
    while (true) {
        std::size_t next = dir.find_first_of("/\\", cursor);
        std::string sub = dir.substr(0, next);
        if (!sub.empty()) {
#ifdef _WIN32
            if (!(sub.size() == 2 && sub[1] == ':')) {
                _mkdir(sub.c_str());
            }
#else
            if (sub != "/") {
                mkdir(sub.c_str(), 0755);
            }
#endif
        }
        if (next == std::string::npos) {
            break;
        }
        cursor = next + 1;
    }
#ifdef _WIN32
    _mkdir(dir.c_str());
#else
    if (dir != "/") {
        mkdir(dir.c_str(), 0755);
    }
#endif
}

}

std::string resolveAbsolutePath(const std::string& path) {
    std::string withExt = ensureBmpExtension(path);
#ifdef _WIN32
    char buffer[_MAX_PATH];
    if (_fullpath(buffer, withExt.c_str(), _MAX_PATH) != nullptr) {
        return std::string(buffer);
    }
    return withExt;
#else
    char buffer[PATH_MAX];
    if (realpath(withExt.c_str(), buffer) != nullptr) {
        return std::string(buffer);
    }
    char cwd[PATH_MAX];
    if (getcwd(cwd, PATH_MAX) != nullptr) {
        std::string combined = std::string(cwd);
        if (!combined.empty() && combined.back() != '/') combined.push_back('/');
        combined += withExt;
        return combined;
    }
    return withExt;
#endif
}

void saveAsBMP(const CImg<unsigned char>& img, const std::string& outputPath) {
    std::string absPath = resolveAbsolutePath(outputPath);
    createDirectories(absPath);
    img.save_bmp(absPath.c_str());
}
