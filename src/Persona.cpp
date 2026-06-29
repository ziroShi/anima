#include "Persona.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Persona::Persona(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
    }
    json data = json::parse(file);
    name_ = data["name"].get<std::string>();
    traits_ = data["traits"].get<std::string>();
    tone_ = data["tone"].get<std::string>();
}

std::string Persona::systemPrompt() const {
    return "You are" + name_ + ", " + traits_ + ". Tone: " + tone_ + ".";
}