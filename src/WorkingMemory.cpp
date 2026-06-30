#include "WorkingMemory.h"
#include <cstdio>
#include <ctime>

WorkingMemory::WorkingMemory(const std::string& path) {
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        fprintf(stderr, "Can't open DB: %s\n", sqlite3_errmsg(db_));
        return;
    }

    const char* sql = 
        "CREATE TABLE IF NOT EXISTS workingmemory ("
        "   id      INTEGER PRIMARY KEY,"
        "   role    TEXT,"
        "   content Text,"
        "   ts      INTEGER"
        ");";
    if (sqlite3_exec(db_, sql, nullptr, nullptr, nullptr) != SQLITE_OK) {
        fprintf(stderr, "Can't create table: %s\n", sqlite3_errmsg(db_));
    }
}

WorkingMemory::~WorkingMemory() {
    sqlite3_close(db_);
}

void WorkingMemory::saveTurn(const std::string& role, const std::string& content) {
    const char* sql = "INSERT INTO workingmemory (role, content, ts) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        fprintf(stderr, "prepare failed: %s\n", sqlite3_errmsg(db_));
        return;
    }

    sqlite3_bind_text(stmt, 1, role.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 3, (sqlite3_int64)std::time(nullptr));

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "insert failed: %s\n", sqlite3_errmsg(db_));
    }

    sqlite3_finalize(stmt);
}

std::vector<Turn> WorkingMemory::fetchRecent(int n) {
    std::vector<Turn> turns;
    const char* sql = "SELECT role, content From workingmemory ORDER by id DESC LIMiT ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        fprintf(stderr, "prepare failed: %s\n", sqlite3_errmsg(db_));
        return turns;
    }
    sqlite3_bind_int(stmt, 1, n);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Turn t;
        t.role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        t.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        turns.push_back(t);
    }
    sqlite3_finalize(stmt);
    std::reverse(turns.begin(), turns.end());
    return turns;
}