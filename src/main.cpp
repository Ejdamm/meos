#include <iostream>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

int main() {
    std::cout << "MeOS Modernization" << std::endl;
    std::cout << "JSON version: " << NLOHMANN_JSON_VERSION_MAJOR << "." << NLOHMANN_JSON_VERSION_MINOR << "." << NLOHMANN_JSON_VERSION_PATCH << std::endl;
    std::cout << "SQLite version: " << sqlite3_libversion() << std::endl;
    return 0;
}
