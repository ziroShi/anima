#pragma once

#include <string>
#include <curl/curl.h>

class LlmClient {

    public:
        explicit LlmClient(const char* api_key);
        std::string sendMessage(const std::string& systemPrompt  ,const std::string& userMessage);

    private:
        std::string apikey_;
};

class CurlHandle {
    public:
        CurlHandle() : curl_(curl_easy_init()) {}
        ~CurlHandle() { if (curl_) curl_easy_cleanup(curl_); }

        CURL* get() const { return curl_; }
    private: 
        CURL* curl_;
};