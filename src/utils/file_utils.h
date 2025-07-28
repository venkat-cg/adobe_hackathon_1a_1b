#pragma once
#include <string>
#include <nlohmann/json.hpp>

void write_json(const std::string& path, const nlohmann::json& j);
