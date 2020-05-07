
#include <bok/core/BuildCache.hpp>

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>


namespace bok {
    BuildCache::BuildCache(const std::string &cacheFile) {
        this->cacheFile = cacheFile;
        this->loadCache();

        fsOutput.open (cacheFile.c_str(), std::ios_base::out);
        if (! fsOutput.is_open()) {
            return;
        }
    }


    BuildCache::~BuildCache() {
        this->saveCache();
    }


    void BuildCache::sourceBuilt(const std::string &sourceFile) {
        const time_t modifiedTime = this->getModifiedTime(sourceFile.c_str(), DL_FILESYSTEM).value();

        sourceCache.insert({sourceFile, modifiedTime});

        this->appendEntryToCache(sourceFile, modifiedTime);
    }


    bool BuildCache::sourceNeedsRebuild(const std::string &sourceFile) const {
        std::cout << "Cheking " << sourceFile << std::endl;

        const auto cachedTimestamp = this->getModifiedTime(sourceFile.c_str(), DL_CACHE);
        const auto currentTimestamp = this->getModifiedTime(sourceFile.c_str(), DL_FILESYSTEM);

        if (!cachedTimestamp.has_value() || !currentTimestamp.has_value()) {
            std::cout << "Must build (HARD): " << true << std::endl;
            std::cout << "    cachedTimestamp: " << cachedTimestamp.has_value() << std::endl;
            std::cout << "    currentTimestamp: " << currentTimestamp.has_value() << std::endl;

            return true;
        }

        std::cout << "Must build: " << (cachedTimestamp.value() != currentTimestamp.value()) << std::endl;
        return cachedTimestamp.value() != currentTimestamp.value();
    }


    void BuildCache::loadCache() {
        std::fstream fs(cacheFile.c_str(), std::ios_base::in);

        if (! fs.is_open()) {
            return;
        }

        std::string line;

        while (!fs.eof()) {
            std::getline(fs, line);

            std::cout << "Parsing line: " << line << std::endl;

            size_t pos = line.find(':');

            if (pos == std::string::npos) {
                std::cout << "    Separator not found: " << line << std::endl;
                continue;
            }

            const std::string key = line.substr(0, pos);
            const std::string value_str = line.substr(pos + 1, line.size());
            const time_t value = static_cast<time_t>(std::atol(line.substr(pos + 1, line.size()).c_str()));
            const long long_value = std::atol(line.substr(pos + 1, line.size()).c_str());

            std::cout << "    key: " << key << std::endl;
            std::cout << "    value_str: " << value_str << std::endl;
            std::cout << "    value: " << value << std::endl;
            std::cout << "    long_value: " << long_value << std::endl;

            sourceCache.insert({key, value});
        }

        // 
        std::cout << "Loaded cache: " << std::endl;
        for (const auto &pair : sourceCache) {
            std::cout << "    " << "\"" << pair.first << "\":" << pair.second << std::endl;
        }
    }


    void BuildCache::saveCache() {
        std::fstream fs {cacheFile.c_str(), std::ios_base::out};

        if (! fs.is_open()) {
            return;
        }

        for (const auto &pair : sourceCache) {
            fs << pair.first << ":" << pair.second << std::endl;
        }
    }


    void BuildCache::appendEntryToCache(const std::string &sourceFile, const time_t modifiedTime) {
        // std::cout << "appendEntryToCache: " << sourceFile << ":" << modifiedTime << std::endl;

        fsOutput  << sourceFile << ":" << modifiedTime << std::endl;
        fsOutput.flush();
    }


    std::optional<time_t> BuildCache::getModifiedTime(const char *fileName, DATA_LOCATION location) const {
        switch (location) {
            case DL_CACHE: {
                if (auto it = sourceCache.find(fileName); it != sourceCache.end()) {
                    return it->second;
                }

                break;
            }

            case DL_FILESYSTEM: {
                struct stat result;

                if (stat(fileName, &result) == 0) {
                    auto modified_time = result.st_mtim;

                    return modified_time.tv_sec;
                }

                break;
            }
        }

        return {};
    }
}