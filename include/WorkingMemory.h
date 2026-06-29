#pragma once
#include <string>
#include <vector>
#include "sqlite3.h"

struct Turn {
    std::string role;
    std::string content;
};

class WorkingMemory {
    public:
        explicit WorkingMemory(const std::string& path);
        ~WorkingMemory();

        void saveTurn(const std::string& role, const std::string& content);
        std::vector<Turn> fetchRecent(int n); 
    private:
        sqlite3* db_ = nullptr;
};