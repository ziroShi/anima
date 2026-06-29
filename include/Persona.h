#pragma once
#include <string>

class Persona {

    public:
        explicit Persona(const std::string& path);
        std::string systemPrompt() const;

    private:
        std::string name_;
        std::string traits_;
        std::string tone_;
};