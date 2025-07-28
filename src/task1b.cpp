#include "task1b.h"
#include "utils/logger.h"
#include "utils/file_utils.h"
#include "outline_extractor.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <chrono>

using json = nlohmann::json;
namespace fs = std::filesystem;

void task1b(const std::string& json_input_path, const std::string& json_output_path) {
    log("Running Task 1B...");

    // Read input JSON file
    std::ifstream input_file(json_input_path);
    if (!input_file.is_open()) {
        log("Failed to open input JSON file: " + json_input_path);
        return;
    }

    json input_json;
    try {
        input_file >> input_json;
    } catch (const std::exception& e) {
        log(std::string("Failed to parse input JSON: ") + e.what());
        return;
    }
    input_file.close();

    // Prepare output JSON structure
    json output_json;
    output_json["metadata"]["input_documents"] = json::array();
    output_json["metadata"]["persona"] = input_json["persona"]["role"];
    output_json["metadata"]["job_to_be_done"] = input_json["job_to_be_done"]["task"];
    output_json["metadata"]["processing_timestamp"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    output_json["extracted_sections"] = json::array();
    output_json["subsection_analysis"] = json::array();

    // Process each document in input JSON
    if (!input_json.contains("documents") || !input_json["documents"].is_array()) {
        log("Input JSON does not contain valid 'documents' array.");
        return;
    }

    // Placeholder for advanced processing using llmodel
    // For each document, extract outline and perform persona-based content analysis
    for (const auto& doc : input_json["documents"]) {
        if (!doc.contains("filename")) {
            log("Document entry missing 'filename'. Skipping.");
            continue;
        }
        std::string pdf_filename = doc["filename"];
        std::string pdf_path = fs::path(fs::path(json_input_path).parent_path()) / pdf_filename;

        // Add to input_documents metadata
        output_json["metadata"]["input_documents"].push_back(pdf_filename);

        // Extract outline using existing extract_outline function
        json outline_json = extract_outline(pdf_path);

        // Example: For each outline entry, add to extracted_sections with importance ranking and page number
        if (outline_json.contains("outline") && outline_json["outline"].is_array()) {
            int rank = 1;
            for (const auto& section : outline_json["outline"]) {
                json extracted_section;
                extracted_section["document"] = pdf_filename;
                extracted_section["section_title"] = section.value("title", "");
                extracted_section["importance_rank"] = rank++;
                extracted_section["page_number"] = section.value("page", 0);
                output_json["extracted_sections"].push_back(extracted_section);

                // Add subsection analysis example
                json subsection;
                subsection["document"] = pdf_filename;
                subsection["refined_text"] = section.value("title", "");
                subsection["page_number"] = section.value("page", 0);
                output_json["subsection_analysis"].push_back(subsection);
            }
        }
    }

    // Write output JSON to file
    std::ofstream output_file(json_output_path);
    if (!output_file.is_open()) {
        log("Failed to open output JSON file: " + json_output_path);
        return;
    }
    output_file << output_json.dump(4);
    output_file.close();

    log("Task 1B completed successfully. Output written to: " + json_output_path);
}
