
#pragma once 

#include <string>
#include <optional>
#include <map>
#include <fstream>


namespace bok {
    class BuildCache {
    private:
        enum DATA_LOCATION {
            DL_CACHE,
            DL_FILESYSTEM
        };


    public:
        explicit BuildCache(const std::string &cacheFile);

        ~BuildCache();

        void sourceBuilt(const std::string &sourceFile);

        bool sourceNeedsRebuild(const std::string &sourceFile) const;

    private:
        void loadCache();

        void saveCache();

        void appendEntryToCache(const std::string &sourceFile, const time_t modifiedTime);

        std::optional<time_t> getModifiedTime(const char *fileName, DATA_LOCATION location) const;

    private:
        std::string cacheFile;
        std::map<std::string, time_t> sourceCache;
        std::fstream fsOutput;
    };
}
