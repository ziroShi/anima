#include "Mood.h"
#include <algorithm>
#include <cctype>

std::string Mood::label() const {
    if (value_ < 33) return "grumpy";
    if (value_ < 66) return "neutral";
    return "cheerful";
}

void Mood::nudge(int delta) {
    value_ += delta;
    if (value_ < 0) value_ = 0;
    if (value_ > 100) value_ = 100;
}

void Mood::observe(const std::string& userMessage) {
    std::string m = userMessage;
    std::transform(m.begin(), m.end(), m.begin(), ::tolower);

    const char* positive[] = {"thank", "love", "great", "awesome", "good", "nice", "lol"};
    const char* negative[] = {"hate", "stupid", "dumb", "shut up", "idiot", "annoying"};

    for (const char* w : positive)
        if (m.find(w) != std::string::npos) { nudge(+10); return; }
    for (const char* w : negative)
        if (m.find(w) != std::string::npos) { nudge(-10); return; }
}