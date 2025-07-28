#include "json_utils.h"
#include "logger.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

void parse_json_example() {
    log("Parsing JSON...");
    std::ifstream file("config/model_config.json");
    if (!file.is_open()) {
        log("Failed to open JSON file.");
        return;
    }
    try {
        json j;
        file >> j;
        log("JSON parsed successfully.");
        // You can add code here to process the JSON object 'j'
    } catch (const std::exception& e) {
        log(std::string("JSON parsing error: ") + e.what());
    }
}
