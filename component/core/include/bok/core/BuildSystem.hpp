
#ifndef __BOK_BUILDSYSTEM_HPP__
#define __BOK_BUILDSYSTEM_HPP__

namespace bok {
    class Package;
    class Compiler;
    class Component;
    class BuildCache;
    class Linker;
    struct CompileOutput;
    struct LinkerOutput;

    class BuildSystem {
    public:
        class Listener {
        public:
            virtual ~Listener() {}
            
            virtual void receiveOutput(const CompileOutput &output) = 0;

            virtual void receiveOutput(const LinkerOutput &output) = 0;
        };

    public:
        explicit BuildSystem(Package *package, BuildCache *buildCache, Listener *listener = nullptr);

        void build(const Compiler &compiler, const Linker linker);

    private:
        void build(const Compiler &compiler, const Linker linker, const Component *component);

    private:
        Package *package = nullptr;
        BuildCache *buildCache = nullptr;
        Listener *listener = nullptr;
    };
}

#endif
