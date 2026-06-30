# Anima

A tiny desktop **AI companion** written from scratch in C++. You chat with a character that has a **personality**, a **mood** that shifts as you talk, and a **memory that survives across sessions** — close the app, reopen it, and it remembers your conversation.

Built with Dear ImGui, libcurl, SQLite, and the Anthropic Claude API.

---

## What it is

Anima is a small, focused, open-source companion app: a single ImGui window where you talk to an AI character. It's a learning and portfolio project — every line is hand-written — and a deliberately scoped slice of a larger companion-AI idea.

Three things make it feel alive:

- **Personality** — the character is defined in a config file you can edit without recompiling.
- **Memory** — every turn is saved to a SQLite database and fed back into the conversation, so it remembers you across restarts.
- **Mood** — a 0–100 value that reacts to what you say and visibly bends the character's tone.

And because the network call to the LLM runs on a **background thread**, the window never freezes while it "thinks."

## Features

- 🪟 Native desktop chat UI (Dear ImGui), with scrolling history and color-coded messages
- 🎭 Configurable persona loaded from `config/persona.json` — change the character without touching code
- 🧠 Persistent memory (SQLite) — conversations are remembered across app restarts
- 🎚️ Dynamic mood (0–100) driven by simple keyword sentiment, shown live in the UI and injected into the prompt
- ⚡ Non-blocking: the LLM request runs on a worker thread (`std::async` / `std::future`), so the UI stays at 60fps
- 🔒 API key read from an environment variable — never hardcoded, never committed

## Tech stack

| Concern | Library |
|---|---|
| Language / build | C++17, CMake (Ninja) |
| UI | [Dear ImGui](https://github.com/ocornut/imgui) (GLFW + OpenGL3 backends) |
| HTTP | [libcurl](https://curl.se/libcurl/) |
| JSON | [nlohmann/json](https://github.com/nlohmann/json) |
| Storage | [SQLite3](https://sqlite.org) (amalgamation, vendored) |
| LLM | [Anthropic Claude API](https://docs.claude.com) (`claude-haiku-4-5`) |

## Architecture

```
include/        # public headers (LlmClient.h, Persona.h, WorkingMemory.h, Mood.h, Turn.h)
src/            # implementation
  main.cpp        # window + render loop + orchestration (assembles prompt, fires the call, updates state)
  LlmClient.cpp   # builds the request, POSTs to the chat API via libcurl, parses the reply
  Persona.cpp     # loads persona.json -> builds the system prompt
  WorkingMemory.cpp  # SQLite: save a turn, fetch the recent ones
  Mood.cpp        # one 0-100 value -> a label and a prompt modifier; reacts to messages
third_party/    # vendored: Dear ImGui, SQLite amalgamation
config/         # persona.json
```

**The core idea — how "memory" actually works:** there's no magic. On each turn the app (1) **saves** the user message and the reply to the database, and (2) before each request, **loads the recent turns and feeds them back** into the `messages` array sent to the model. Because that store is a file on disk, the recall survives the program closing. Personality comes from a top-level `system` prompt assembled from the persona plus the current mood.

## Build & run

Built and tested on **Windows with MSYS2 / MinGW-w64** (GCC 14, CMake, Ninja). The code is portable; on other platforms install the equivalent packages and adjust the commands.

**1. Install the dependencies** (in an MSYS2 MINGW64 shell):
```sh
pacman -S mingw-w64-x86_64-glfw mingw-w64-x86_64-curl mingw-w64-x86_64-nlohmann-json
```
Dear ImGui and SQLite are vendored in `third_party/`, so there's nothing to install for those.

**2. Set your Anthropic API key** as an environment variable (PowerShell):
```powershell
$env:ANIMA_API_KEY = "sk-ant-..."     # current session
# or, to persist across new terminals:
setx ANIMA_API_KEY "sk-ant-..."
```

**3. Configure, build, and run:**
```sh
cmake -S . -B build -G Ninja
cmake --build build
./build/anima.exe
```

Edit `config/persona.json` and relaunch to change the character — no rebuild required.

## License

MIT — see [LICENSE](LICENSE).
