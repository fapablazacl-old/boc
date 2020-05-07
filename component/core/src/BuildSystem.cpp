
#include <bok/core/BuildSystem.hpp>

#include <vector>
#include <string>
#include <bok/core/Compiler.hpp>
#include <bok/core/Linker.hpp>
#include <bok/core/BuildCache.hpp>
#include <bok/core/Component.hpp>
#include <bok/core/Package.hpp>


namespace bok {
    BuildSystem::BuildSystem(Package *package, BuildCache *buildCache, Listener *listener) {
        this->package = package;
        this->buildCache = buildCache;
        this->listener = listener;
    }


    void BuildSystem::build(const Compiler &compiler, const Linker linker) {
        for (Component *component : package->getComponents()) {
            this->build(compiler, linker, component);
        }
    }


    void BuildSystem::build(const Compiler &compiler, const Linker linker, const Component *component) {
        std::vector<std::string> objects;

        for (const std::string &source : component->getSources()) {
            if (! compiler.isCompilable(source)) {
                continue;
            }
            
            const std::string sourceFile = component->getPackage()->getPath() + component->getPath() + source;
            const CompileOutput output = compiler.compile(sourceFile);

            if (buildCache->sourceNeedsRebuild(sourceFile)) {
                if (listener) {
                    listener->receiveOutput(output);
                    buildCache->sourceBuilt(sourceFile);
                }
            }

            objects.push_back(output.objectFile);
        }

        const LinkerOutput output = linker.link(component->getName(), component->getPackage()->getPath() + component->getPath() + component->getName(), objects);

        if (listener) {
            listener->receiveOutput(output);
        }
    }

}