#include "file_utils.h"
#include "logger.h"
#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

void write_json(const std::string& path, const nlohmann::json& j) {
    std::ofstream ofs(path);
    ofs << "{\n";
    ofs << "    \"title\": " << j["title"].dump() << ",\n";
    ofs << "    \"outline\": " << j["outline"].dump(4) << "\n";
    ofs << "}\n";
    ofs.close();
}
