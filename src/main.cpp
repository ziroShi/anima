#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <future>
#include <chrono>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "LlmClient.h"
#include "Persona.h"
#include "WorkingMemory.h"

enum class Role {
    User,
    Assistant
};

struct Message {
    Role role;
    std::string text;
};

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

std::vector<Message> messages = {};

int main(int, char**) {
    const char* api_key = std::getenv("ANIMA_API_KEY");
    if (api_key == nullptr) { fprintf(stderr, "ANIMA_API_KEY not set\n"); return 1; }

    LlmClient client(api_key);

    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow((int)(1200 * main_scale), (int)(800 * main_scale), "Anima", nullptr, nullptr);
    if (window == nullptr) return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enables Vsync

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui::GetStyle().ScaleAllSizes(main_scale);
    io.FontGlobalScale = main_scale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    char input_buf[256] = "";
    std::future<std::string> reply_future;
    bool waiting = false;

    Persona persona("config/persona.json");
    std::string system = persona.systemPrompt();

    WorkingMemory workingmemory("anima.db");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Anima");

        float reserved = ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("history", ImVec2(0, -reserved), ImGuiChildFlags_Borders);
        for (const Message& m : messages) {
            if (m.role == Role::Assistant) {
                ImVec4 color = ImVec4(0.4f, 0.8f, 1.0f, 1.0f);
                ImGui::TextColored(color, "%s", "Assistant: ");
            } else {
                ImVec4 color = ImVec4(1.0f, 0.5f, 1.0f, 1.0f);
                ImGui::TextColored(color, "%s", "User: ");
            }
            ImGui::SameLine();
            ImGui::TextWrapped("%s", m.text.c_str());
        }
        ImGui::EndChild();

        ImGui::SetKeyboardFocusHere();
        bool send = ImGui::InputText("##input", input_buf, sizeof(input_buf), ImGuiInputTextFlags_EnterReturnsTrue);
        ImGui::SameLine();

        send |= ImGui::Button("Send");

        if (send && input_buf[0] != '\0' && !waiting) {
            std::string msg = input_buf;
            // std::string system = "You are Anima, a warm, upbeat companion. Keep replies short and friendly.";
            messages.push_back({ Role::User, msg});
            workingmemory.saveTurn("user", msg);
            reply_future = std::async(std::launch::async,
                [&client, system, msg]() { return client.sendMessage(system, msg); });
            waiting = true;
            input_buf[0] = '\0';
        }

        if (waiting) {
            if (reply_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                std::string reply = reply_future.get();
                messages.push_back({Role::Assistant, reply});
                workingmemory.saveTurn("assistant", reply);
                waiting = false;
            } else {
                ImGui::TextDisabled("Anima is thinking...");
            }
        }
    

        ImGui::End();

        ImGui::Render();

        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    

    return 0;
}