#pragma once
#include <string>
#include <nlohmann/json.hpp>

nlohmann::json extract_outline(const std::string& pdf_path);