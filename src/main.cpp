#include "utils/logger.h"
#include "utils/file_utils.h"
#include "task1a.h"
#include "task1b.h"
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>

namespace fs = std::filesystem;

int main() {
    log("Starting PDF outline extraction...");

    std::string input_dir = "/app/input";
    std::string output_dir = "/app/output";

    if (!fs::exists(input_dir)) {
        log("Input directory does not exist: " + input_dir);
        return 1;
    }
    if (!fs::exists(output_dir)) {
        fs::create_directory(output_dir);
    }

    // Scan input directory for PDFs and JSON files
    std::vector<fs::path> pdf_files;
    std::vector<fs::path> json_files;

    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (entry.is_regular_file()) {
            if (entry.path().extension() == ".pdf") {
                pdf_files.push_back(entry.path());
            } else if (entry.path().extension() == ".json") {
                json_files.push_back(entry.path());
            }
        }
    }

    if (pdf_files.size() == 1 && json_files.empty()) {
        log("Single PDF detected, running Task 1A");
        task1a();
    } else if (pdf_files.size() > 1 && !json_files.empty()) {
        log("Multiple PDFs and JSON input detected, running Task 1B");
        // For simplicity, use the first JSON file found as input
        std::string json_input_path = json_files[0].string();
        std::string json_output_path = output_dir + "/challenge1b_output.json";
        task1b(json_input_path, json_output_path);
    } else {
        log("Input conditions do not match Task 1A or Task 1B requirements.");
        return 1;
    }

    log("Processing completed.");
    return 0;
}
