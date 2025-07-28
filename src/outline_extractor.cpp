#include "outline_extractor.h"
#include <poppler-document.h>
#include <poppler-page.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <filesystem>
#include <codecvt>
#include <locale>

using json = nlohmann::json;

// Improved heading detection with more patterns and keywords
bool is_heading(const std::string& line) {
    std::regex h1_regex(R"(^\d+\.\\s+.*)");      // "1. Introduction ..."
    std::regex h2_regex(R"(^\d+\.\\d+\\s+.*)");   // "2.1 Intended Audience ..."
    std::vector<std::string> keywords = {
        "Revision History", "Table of Contents", "Acknowledgements", "References",
        "Introduction", "Business Outcomes", "Content", "Trademarks", "Documents and Web Sites",
        "Syllabus", "Audience", "Career Paths", "Learning Objectives", "Entry Requirements",
        "Structure and Course Duration", "Keeping It Current"
    };
    if (std::regex_match(line, h1_regex)) return true;
    if (std::regex_match(line, h2_regex)) return true;
    for (const auto& kw : keywords) {
        // Case-insensitive search
        std::string lower_line = line;
        std::transform(lower_line.begin(), lower_line.end(), lower_line.begin(), ::tolower);
        std::string lower_kw = kw;
        std::transform(lower_kw.begin(), lower_kw.end(), lower_kw.begin(), ::tolower);
        if (lower_line.find(lower_kw) != std::string::npos) return true;
    }
    // All-caps or title-case lines
    if (!line.empty() && (std::all_of(line.begin(), line.end(), [](char c){ return std::isupper(c) || std::isspace(c); }) ||
        (std::isupper(line[0]) && std::count(line.begin(), line.end(), ' ') > 0))) {
        return true;
    }
    return false;
}

std::string heading_level(const std::string& line) {
    std::regex h1_regex(R"(^\d+\.\\s+.*)");
    std::regex h2_regex(R"(^\d+\.\\d+\\s+.*)");
    if (std::regex_match(line, h1_regex)) return "H1";
    if (std::regex_match(line, h2_regex)) return "H2";
    return "H1";
}

// Helper function to remove invalid UTF-8 characters
std::string sanitize_utf8(const std::string& input) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::u16string utf16;
    try {
        utf16 = convert.from_bytes(input);
    } catch (...) {
        // Remove invalid characters by ignoring errors
        utf16.clear();
        for (size_t i = 0; i < input.size(); ++i) {
            if ((input[i] & 0x80) == 0) { // ASCII
                utf16.push_back(input[i]);
            }
        }
    }
    return convert.to_bytes(utf16);
}

#include <algorithm>

// Extract title from the first page by analyzing multiple lines and scoring candidates
std::string extract_title(poppler::document* doc) {
    if (!doc || doc->pages() == 0) return "Overview";
    auto page = doc->create_page(0);
    if (!page) return "Overview";
    auto text = page->text();
    std::string text_str = text.to_latin1();
    std::string sanitized_text = sanitize_utf8(text_str);
    std::istringstream iss(sanitized_text);
    std::string line;
    std::vector<std::string> candidates;
    while (std::getline(iss, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (!line.empty()) {
            candidates.push_back(line);
        }
    }
    if (candidates.empty()) return "Overview";

    // Score candidates: prefer longer lines with mixed case and no excessive punctuation
    auto score = [](const std::string& s) {
        int score = 0;
        if (s.length() > 10) score += 10;
        if (std::any_of(s.begin(), s.end(), [](char c){ return std::islower(c); })) score += 5;
        if (std::any_of(s.begin(), s.end(), [](char c){ return std::isupper(c); })) score += 5;
        int punct_count = std::count_if(s.begin(), s.end(), [](char c){ return std::ispunct(c); });
        if (punct_count < s.length() / 10) score += 5;
        return score;
    };

    std::string best = candidates[0];
    int best_score = score(best);
    for (const auto& c : candidates) {
        int s = score(c);
        if (s > best_score) {
            best = c;
            best_score = s;
        }
    }
    return best;
}

json extract_outline(const std::string& pdf_path) {
    std::filesystem::path path(pdf_path);
    if (!std::filesystem::exists(path)) {
        std::cerr << "PDF file does not exist: " << pdf_path << std::endl;
        return json();
    }

    auto doc = poppler::document::load_from_file(pdf_path);
    if (!doc) {
        std::cerr << "Failed to open PDF: " << pdf_path << std::endl;
        return json();
    }

    json result;
    result["title"] = extract_title(doc);
    result["outline"] = json::array();

    for (int i = 0; i < doc->pages(); ++i) {
        auto page = doc->create_page(i);
        if (!page) continue;
        auto text = page->text();
        std::string text_str = text.to_latin1();
        std::string sanitized_text = sanitize_utf8(text_str);
        std::istringstream iss(sanitized_text);
        std::string line;
        while (std::getline(iss, line)) {
            // Aggressive whitespace trimming
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (is_heading(line)) {
                result["outline"].push_back({
                    {"level", heading_level(line)},
                    {"text", line},
                    {"page", i + 1}
                });
            }
        }
    }

    return result;
}
