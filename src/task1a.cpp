#include "task1a.h"
#include "utils/logger.h"
#include "outline_extractor.h"
#include "utils/file_utils.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>

using json = nlohmann::json;
namespace fs = std::filesystem;

void task1a() {
    log("Running Task 1A with PDFium-based outline extraction...");

    std::string input_dir = "input";
    std::string output_dir = "output";

    if (!fs::exists(input_dir)) {
        log("Input directory does not exist: " + input_dir);
        return;
    }
    if (!fs::exists(output_dir)) {
        fs::create_directory(output_dir);
    }

    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (entry.path().extension() == ".pdf") {
            std::string pdf_path = entry.path().string();
            std::string json_path = output_dir + "/" + entry.path().stem().string() + ".json";
            log("Processing PDF: " + pdf_path);

            // Extract outline using PDFium-based function
            json outline_json = extract_outline(pdf_path);

            // Post-process outline to enhance structure (example: sort by page, group by level)
            if (outline_json.contains("outline") && outline_json["outline"].is_array()) {
                auto& outline_array = outline_json["outline"];
                std::sort(outline_array.begin(), outline_array.end(), [](const json& a, const json& b) {
                    return a["page"].get<int>() < b["page"].get<int>();
                });
            }

            // Write JSON output
            write_json(json_path, outline_json);
            log("Outline JSON written to: " + json_path);
        }
    }

    log("Task 1A completed successfully.");
}
