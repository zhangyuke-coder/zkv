#pragma once 
#include <stdint.h>
#include <unordered_set>
#include <vector>
#include <unordered_map>
namespace zkv {
    
struct TableManifest {
    uint32_t level;
    uint64_t crc_sum;
};
struct Manifest
{
    std::vector<std::unordered_set<TableManifest>> level_tables_map;
    std::unordered_map<uint64_t, TableManifest> table_levels_map;
    int32_t creations = 0;
    int32_t deletions = 0;
    void Clear() {
        level_tables_map.clear();
        table_levels_map.clear();
        creations = 0;
        deletions = 0;
    }
};
 
class ManifestHandler {
public:
    ManifestHandler(const std::string& db_path);
    bool OpenManifestFile();
    bool ReWrite();
    //
    bool AddChanges(const std::string& input);
    bool AddTableMeta(int32_t level, int64_t sst_id);
    bool RevertToManifest(const std::unordered_set<uint64_t>& delete_sst_ids);
    const Manifest& GetManifest() { return manifest_; }

private:
    void CreateNewManifestFile();
    bool RecoverFromReWriteManifestFile();
    bool ReplayManifestFile();
private:
    Manifest manifest_;
    std::string db_path_;
};

}

