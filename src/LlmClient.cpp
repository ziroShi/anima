#include "LlmClient.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static size_t write_cb(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* response = static_cast<std::string*>(userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}

LlmClient::LlmClient(const char* api_key) : apikey_(api_key) {}

std::string LlmClient::sendMessage(const std::string& systemPrompt,const std::vector<Turn>& history) {
    json messages_json = json::array();
    for (const Turn& t : history) {
        messages_json.push_back({{"role", t.role}, {"content", t.content}});
    }
    json body = {
        {"model", "claude-haiku-4-5"},
        {"max_tokens", 1024},
        {"system", systemPrompt},
        {"messages", messages_json}
    };

    std::string body_str = body.dump();

    struct curl_slist* headers = nullptr;      
    headers = curl_slist_append(headers, "content-type: application/json");
    headers = curl_slist_append(headers, "anthropic-version: 2023-06-01");
    std::string auth = std::string("x-api-key: ") + apikey_;
    headers = curl_slist_append(headers, auth.c_str());

    std::string response;
    CURLcode result = CURLE_FAILED_INIT;
    
    CurlHandle handle;
    CURL* curl = handle.get();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.anthropic.com/v1/messages");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body_str.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        result = curl_easy_perform(curl);

        curl_slist_free_all(headers);
    }

    if (result != CURLE_OK) {
        return std::string("curl failed: ") + curl_easy_strerror(result);
    } 

    try {
        json reply = json::parse(response);
        if (reply.contains("content")) {
            std::string text = reply["content"][0]["text"];
            return text;
        } else if (reply.contains("error")) {
            return "API error: " + reply["error"]["message"].get<std::string>();
        }else {
            return "Unexpected response: " + response;
        }
    } catch (const json::exception& e) {
        return std::string("Failed to parse JSON: ") + e.what();
    }
}
