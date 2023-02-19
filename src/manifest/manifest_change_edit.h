#pragma once
#include <string>
#include <stdint.h>
#include <vector>
namespace zkv {
struct Manifest;
enum ManifestChangeOpType {
    kCreate = 0,
    kDelete = 1
};
struct ManifestChange {
    uint64_t id;
    uint32_t level;
    ManifestChangeOpType manifest_change_type = ManifestChangeOpType::kCreate;
};

class ManifestChangeEdit {
public:
    void ParseFromManifest(const Manifest& manifest);

    void EncodeTo(const std::vector<ManifestChange>& manifest_changes,
                std::string* out);
    void DecodeTo(const std::string& input);
    void ApplyChangeSet(Manifest& manifest);
    const std::vector<ManifestChange>& GetManifestChanages() {
        return manifest_changes_;
    }
private:
    std::vector<ManifestChange> manifest_changes_;
};
}