#include "BloomHasher.hpp"
#include "HashFunctions.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

const std::string RESET = "\033[0m";
const std::string RED = "\033[1;31m";
const std::string GREEN = "\033[1;32m";
const std::string YELLOW = "\033[1;33m";
const std::string BLUE = "\033[1;34m";
const std::string WHITE = "\033[1;37m";
const std::string CYAN = "\033[1;36m";

const std::string DEFAULT_DATASET_FILE = "rockyou_subset_36.txt";
const std::string DEFAULT_CANDIDATES_FILE = "candidates.txt";

double jaccard_threshold = 0.30;
double dice_threshold = 0.45;
double cosine_threshold = 0.50;

void print_banner();
void create_default_files();
std::vector<std::string> load_passwords(const std::string& filename);
void run_original_test();
void run_batch_test(const std::string& dataset_file, const std::string& candidates_file);
void run_interactive_test(const std::vector<std::string>& dataset);
void view_dataset(const std::vector<std::string>& dataset);
void show_threshold_info();

int main() {
    create_default_files();
    
    std::vector<std::string> dataset = load_passwords(DEFAULT_DATASET_FILE);
    
    int choice = 0;
    do {
        print_banner();
        std::cout << BLUE << "=========================================================\n" << RESET;
        std::cout << WHITE << "                  MAIN MENU\n" << RESET;
        std::cout << BLUE << "=========================================================\n" << RESET;
        std::cout << CYAN << " 1." << RESET << " Run Password Similarity Batch Test (rockyou_subset_36.txt vs candidates.txt)\n";
        std::cout << CYAN << " 2." << RESET << " Run Interactive Password Tester\n";
        std::cout << CYAN << " 3." << RESET << " Run Original Bigram Test Module (from assignment starter)\n";
        std::cout << CYAN << " 4." << RESET << " View Similarity Threshold & Justification\n";
        std::cout << CYAN << " 5." << RESET << " View Loaded Password Dataset (" << dataset.size() << " passwords)\n";
        std::cout << CYAN << " 6." << RESET << " Exit\n";
        std::cout << BLUE << "---------------------------------------------------------\n" << RESET;
        std::cout << WHITE << "Enter your choice (1-6): " << RESET;
        
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) {
                std::cout << "\nEOF reached. Exiting...\n";
                break;
            }
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            choice = 0;
            continue;
        }
        std::cout << "\n";
        
        dataset = load_passwords(DEFAULT_DATASET_FILE);
        
        switch (choice) {
            case 1:
                run_batch_test(DEFAULT_DATASET_FILE, DEFAULT_CANDIDATES_FILE);
                break;
            case 2:
                run_interactive_test(dataset);
                break;
            case 3:
                run_original_test();
                break;
            case 4:
                show_threshold_info();
                break;
            case 5:
                view_dataset(dataset);
                break;
            case 6:
                std::cout << GREEN << "Exiting. Goodbye!\n" << RESET;
                break;
            default:
                std::cout << RED << "Invalid choice. Use 1-6.\n" << RESET;
                break;
        }
        
        if (choice == 6) break;
        
        std::cout << "\nPress Enter to return to menu...";
        std::cin.ignore(10000, '\n');
        if (std::cin.eof()) break;
        std::cin.get();
        if (std::cin.eof()) break;
        
    } while (true);
    
    return 0;
}

void print_banner() {
    std::cout << CYAN << "=========================================================\n" << RESET;
    std::cout << WHITE << "       CSCI262 - PASSWORD SIMILARITY BLOOM FILTER\n" << RESET;
    std::cout << CYAN << "=========================================================\n" << RESET;
}

void create_default_files() {
    std::ifstream f1(DEFAULT_DATASET_FILE);
    if (!f1.good()) {
        std::ofstream out(DEFAULT_DATASET_FILE);
        out << "password\n";
        out << "mueller1\n";
        out << "qwertyui\n";
        out << "admin123\n";
        out << "security\n";
        out << "sunshine\n";
        out << "princess\n";
        out << "football\n";
        out.close();
    }
    
    std::ifstream f2(DEFAULT_CANDIDATES_FILE);
    if (!f2.good()) {
        std::ofstream out(DEFAULT_CANDIDATES_FILE);
        out << "password\n";
        out << "pa$$word\n";
        out << "passward\n";
        out << "mueller1\n";
        out << "mue11er1\n";
        out << "admin123\n";
        out << "admin!23\n";
        out << "football\n";
        out << "f00tball\n";
        out << "securepwd123\n";
        out << "hello98765\n";
        out.close();
    }
}

std::vector<std::string> load_passwords(const std::string& filename) {
    std::vector<std::string> passwords;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << RED << "Error opening " << filename << "\n" << RESET;
        return passwords;
    }
    std::string line;
    while (std::getline(file, line)) {

        while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' ' || line.back() == '\t')) {
            line.pop_back();
        }
        while (!line.empty() && (line.front() == ' ' || line.front() == '\t')) {
            line.erase(0, 1);
        }
        if (!line.empty() && line[0] != '#') {
            passwords.push_back(line);
        }
    }
    return passwords;
}

void run_original_test() {
    std::cout << WHITE << "Running original test module...\n" << RESET;
    std::vector<std::string> test_bigrams = {" M", "MU", "UE", "EL", "LL", "LE", "ER", "R "};
    for (const auto& bg : test_bigrams) {
        uint64_t f = HashFunctions::sha256Hash(bg);
        uint64_t g = HashFunctions::md5Hash(bg);
        
        std::cout << "Testing Bigram: '" << bg << "'\n";
        std::cout << "--------------------------------------\n";
        std::cout << "f (SHA256 trunc): " << f << "\n";
        std::cout << "g (MD5 trunc)   : " << g << "\n";
        
        auto pos = BloomHasher::generatePositions(bg);
        
        std::cout << "Generated " << pos.size() << " positions:\n[ ";
        for (size_t i = 0; i < pos.size(); i++) {
            std::cout << pos[i] << (i < pos.size() - 1 ? ", " : " ]\n");
        }
        std::cout << "\n";
    }
}

void run_batch_test(const std::string& dataset_file, const std::string& candidates_file) {
    std::vector<std::string> dataset = load_passwords(dataset_file);
    std::vector<std::string> candidates = load_passwords(candidates_file);
    
    if (dataset.empty() || candidates.empty()) {
        std::cout << RED << "Missing passwords or candidates list!\n" << RESET;
        return;
    }
    
    std::cout << WHITE << "Comparing candidates against dataset (rejection threshold Jaccard >= " 
              << std::fixed << std::setprecision(2) << jaccard_threshold << "):\n\n" << RESET;
    
    std::vector<std::vector<bool>> dataset_filters;
    dataset_filters.reserve(dataset.size());
    for (const auto& pwd : dataset) {
        try {
            dataset_filters.push_back(BloomHasher::getPasswordFilter(pwd));
        } catch (const std::invalid_argument& e) {
            std::cerr << RED << "Error in dataset password '" << pwd << "': " << e.what() << RESET << "\n";
            dataset_filters.push_back(std::vector<bool>(1000, false));
        }
    }
    
    std::cout << std::left 
              << std::setw(15) << "Candidate" 
              << std::setw(15) << "Best Match" 
              << std::right 
              << std::setw(10) << "Jaccard" 
              << std::setw(10) << "Dice" 
              << std::setw(10) << "Cosine" 
              << "    " << "Decision" << "\n";
    std::cout << std::string(75, '-') << "\n";
    
    for (const auto& cand : candidates) {
        std::vector<bool> cand_filter;
        bool invalid = false;
        std::string err_msg = "";
        try {
            cand_filter = BloomHasher::getPasswordFilter(cand);
        } catch (const std::invalid_argument& e) {
            invalid = true;
            err_msg = e.what();
        }
        
        double max_j = -1.0;
        double best_d = 0.0;
        double best_c = 0.0;
        std::string best_match = "";
        
        if (!invalid) {
            for (size_t i = 0; i < dataset.size(); i++) {
                double jaccard = BloomHasher::computeJaccard(cand_filter, dataset_filters[i]);
                if (jaccard > max_j) {
                    max_j = jaccard;
                    best_match = dataset[i];
                    best_d = BloomHasher::computeDice(cand_filter, dataset_filters[i]);
                    best_c = BloomHasher::computeCosine(cand_filter, dataset_filters[i]);
                }
            }
        }
        
        bool reject = invalid || (max_j >= jaccard_threshold);
        std::string decision = reject ? (RED + "REJECT" + RESET) : (GREEN + "ACCEPT" + RESET);
        
        std::string display_name = cand;
        size_t null_pos = display_name.find('\0');
        if (null_pos != std::string::npos) {
            display_name = display_name.substr(0, null_pos) + "[\\0]" + display_name.substr(null_pos + 1);
        }

        std::cout << std::left 
                  << std::setw(15) << (display_name.length() > 14 ? display_name.substr(0, 11) + "..." : display_name)
                  << std::setw(15) << (invalid ? "INVALID INPUT" : (best_match.length() > 14 ? best_match.substr(0, 11) + "..." : best_match))
                  << std::right << std::fixed << std::setprecision(4)
                  << std::setw(10) << (invalid ? 0.0 : max_j) 
                  << std::setw(10) << (invalid ? 0.0 : best_d) 
                  << std::setw(10) << (invalid ? 0.0 : best_c) 
                  << "    " << decision << "\n";
    }
    std::cout << std::string(75, '-') << "\n";
}

void run_interactive_test(const std::vector<std::string>& dataset) {
    if (dataset.empty()) return;
    
    std::cin.ignore(10000, '\n');
    std::string input;
    std::cout << WHITE << "Enter candidate password: " << RESET;
    std::getline(std::cin, input);
    
    if (input.empty()) return;
    
    std::cout << "\n";
    std::cout << YELLOW << "--- Analysis for: '" << input << "' ---\n" << RESET;
    
    std::string safe_input = input;
    size_t null_pos = safe_input.find('\0');
    if (null_pos != std::string::npos) {
        safe_input = safe_input.substr(0, null_pos) + "[\\0]" + safe_input.substr(null_pos + 1);
    }
    std::string padded = " " + safe_input + " ";
    std::cout << "Padded: " << padded << "\n";
    
    std::vector<bool> cand_filter;
    try {
        cand_filter = BloomHasher::getPasswordFilter(input);
    } catch (const std::invalid_argument& e) {
        std::cout << RED << "ERROR: " << e.what() << RESET << "\n";
        return;
    }
    size_t bits_set = 0;
    for (bool b : cand_filter) if (b) bits_set++;
    
    for (size_t i = 0; i < padded.length() - 1; i++) {
        std::string bg = padded.substr(i, 2);
        auto pos = BloomHasher::generatePositions(bg);
        std::cout << "  Bigram '" << bg << "': [ ";
        for (size_t j = 0; j < pos.size(); j++) {
            std::cout << pos[j] << (j < pos.size() - 1 ? ", " : " ]\n");
        }
    }
    std::cout << "Filter Density: " << bits_set << " / 1000 bits set\n\n";
    
    std::cout << std::left 
              << std::setw(15) << "Common Password" 
              << std::right 
              << std::setw(10) << "Jaccard" 
              << std::setw(10) << "Dice" 
              << std::setw(10) << "Cosine" 
              << "    " << "Decision" << "\n";
    std::cout << std::string(60, '-') << "\n";
    
    bool reject = false;
    std::string matched_pwd = "";
    double max_j = -1.0;
    
    for (const auto& pwd : dataset) {
        std::vector<bool> pwd_filter = BloomHasher::getPasswordFilter(pwd);
        double jaccard = BloomHasher::computeJaccard(cand_filter, pwd_filter);
        double dice = BloomHasher::computeDice(cand_filter, pwd_filter);
        double cosine = BloomHasher::computeCosine(cand_filter, pwd_filter);
        
        if (jaccard > max_j) {
            max_j = jaccard;
            matched_pwd = pwd;
        }
        
        bool is_similar = (jaccard >= jaccard_threshold);
        if (is_similar) reject = true;
        
        std::string decision = is_similar ? (RED + "REJECT" + RESET) : (GREEN + "ACCEPT" + RESET);
        
        std::cout << std::left 
                  << std::setw(15) << (pwd.length() > 14 ? pwd.substr(0, 11) + "..." : pwd)
                  << std::right << std::fixed << std::setprecision(4)
                  << std::setw(10) << jaccard 
                  << std::setw(10) << dice 
                  << std::setw(10) << cosine 
                  << "    " << decision << "\n";
    }
    std::cout << std::string(60, '-') << "\n";
    
    if (reject) {
        std::cout << RED << "REJECTED: Too similar to '" << matched_pwd 
                  << "' (J = " << std::fixed << std::setprecision(4) << max_j << ")\n" << RESET;
    } else {
        std::cout << GREEN << "ACCEPTED: Distinct password.\n" << RESET;
    }
}

void view_dataset(const std::vector<std::string>& dataset) {
    std::cout << WHITE << "Loaded Passwords:\n" << RESET;
    for (size_t i = 0; i < dataset.size(); i++) {
        std::vector<bool> f = BloomHasher::getPasswordFilter(dataset[i]);
        size_t set = 0;
        for (bool b : f) if (b) set++;
        std::cout << "  " << (i + 1) << ". " << std::setw(15) << std::left << dataset[i] 
                  << " (" << set << " bits set)\n";
    }
}

void show_threshold_info() {
    std::cout << YELLOW << "--- Threshold Justification ---\n" << RESET;
    std::cout << "Current Jaccard Threshold: " << jaccard_threshold << "\n\n";
    std::cout << "Justification:\n"
              << "  - Identical passwords score 1.00\n"
              << "  - Single character variations (e.g. password vs pa$$word) score 0.50 - 0.80\n"
              << "  - Double variations (e.g. football vs f00tball) score 0.35 - 0.55\n"
              << "  - Entirely different passwords score < 0.15\n\n"
              << "Entering a value here changes the filter rejection cutoff.\n"
              << "Enter new Jaccard threshold (0.05 - 0.95) or 0 to keep: ";
    double val;
    if (std::cin >> val) {
        if (val >= 0.05 && val <= 0.95) {
            jaccard_threshold = val;
            std::cout << "Updated to " << jaccard_threshold << "\n";
        }
    } else {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
}


