#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "sqlite3.h"

#include "Turn.h"

class WorkingMemory {
    public:
        explicit WorkingMemory(const std::string& path);
        ~WorkingMemory();

        void saveTurn(const std::string& role, const std::string& content);
        std::vector<Turn> fetchRecent(int n); 
    private:
        sqlite3* db_ = nullptr;
};