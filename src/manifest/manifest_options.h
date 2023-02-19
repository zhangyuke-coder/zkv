#pragma once
#include <string>
#include <stdint.h>
namespace zkv {
namespace ManifestOptions {
    static const std::string kManifestName = "MANIFEST";
    static const std::string kManifestRewriteFilename = "REWRITEMANIFEST";
    static constexpr uint32_t kManifestDeletionsRewriteThreshold = 10000;
    static constexpr uint32_t kManifestDeletionsRatio = 10;
}
}