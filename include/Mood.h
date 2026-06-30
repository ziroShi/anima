#pragma once
#include <string>

class Mood {
    public:
        int value() const { return value_; }
        std::string label() const;
        void nudge(int data);
        void observe(const std::string& userMessage);

    private:
        int value_ = 50;
};