# <img src="Resources/Icon128.png" alt="Prompt Blueprint Logo" width="32" height="32" style="vertical-align: middle;"> PromptBlueprint: LLM Integration for Unreal Engine

**PromptBlueprint** is an Unreal Engine plugin that enables seamless communication with **Large Language Models (LLMs)** directly from **Blueprints** and **C++**.

It allows you to send prompts to **any LLM API endpoint** (such as OpenRouter, OpenAI-compatible APIs, or custom servers) and receive responses asynchronously via callbacks.

This makes it ideal for building **AI-driven gameplay systems**, NPC dialogue, developer tools, procedural generation, and in-editor virtual assistants—**without writing complex networking code**.

---

## ✨ Features

* ✅ **Blueprint & C++ Accessible:** Communicate with LLMs directly from your game logic.
* ✅ **Universal API Support:** Send prompts to **any LLM URL** (OpenAI, OpenRouter, custom servers).
* ✅ **Asynchronous Requests:** Non-blocking HTTP requests keep your game running smoothly.
* ✅ **Simple Callback System:** Clear success and failure handling for every API call.
* ✅ **JSON Field Extraction:** Easily parse responses using ordered key paths (e.g., `choices[0].message.content`).
* ✅ **Versatile Use:** Ideal for AI chat, tool-calling, gameplay logic, and automation.
* ✅ **Lightweight & Modular:** Built on native Unreal Engine components for performance.

---

## 🛠 Supported Use Cases

* **AI-Powered NPC Dialogue Systems** (dynamic, context-aware conversations)
* **Developer Assistant Tools** inside Unreal Editor (e.g., generating code snippets, summarizing documentation)
* **Procedural Generation** of story beats, quests, or dialogue trees at runtime.
* **QA Bots and Debugging Assistants** for in-editor testing.
* **Runtime Prompt-Based AI Logic** for emergent gameplay.

---

## ⚙️ Installation

1.  Clone or download this repository.
2.  Place the plugin inside your **Unreal Engine Project's** `Plugins` directory:
    ```
    <YourProjectName>/Plugins/PromptBlueprint/
    ```
3.  Ensure the plugin is enabled in your Unreal Editor's **Plugins** window.
4.  If building from source, you may need to add `"PromptBlueprint"` to the `PublicDependencyModuleNames` in your project's `Build.cs` file.

---

## 🚀 Getting Started (Blueprint Example)

The core functionality is exposed through a single, easy-to-use **Blueprint node**: **`Send LLM Request`**.

### 1. Configure the Request

On any Blueprint graph (e.g., in a Widget, Actor, or Blueprint Function Library):

* **Target URL:** The API endpoint for your LLM (e.g., `https://api.openai.com/v1/chat/completions`).
* **API Key:** Your authorization token (often passed in the `Authorization` header).
* **Model Name:** The specific model you want to use (e.g., `gpt-4o`, `llama3-70b`).
* **Prompt/Messages:** The text input for the LLM.

### 2. Connect the Callbacks

The node executes **asynchronously** and will branch to one of two pins once the network request is complete:

| Pin | Description |
| :--- | :--- |
| **On Success** | Fires when a response is received. Provides the raw **Response JSON** and the extracted **Content String**. |
| **On Failure** | Fires on network errors, invalid URLs, or HTTP error codes. Provides the **Error Message**. |



### 3. Extracting the Content

The `Send LLM Request` node automatically handles extracting the useful response content.

* **Extraction Path:** Use a key path like `choices[0].message.content` (standard for OpenAI-compatible APIs) to directly retrieve the final generated text into a simple **Content String** variable. This avoids manual JSON parsing for the most common use case.

---

## 💡 C++ Integration

For C++ users, the functionality is exposed via the **`UPromptBlueprintFunctionLibrary`** class.

```cpp
// Example C++ call
FString URL = TEXT("Your API URL");
FString Key = TEXT("Your API Key");
FString Payload = TEXT("{\"model\": \"gpt-3.5-turbo\", \"messages\": [{\"role\": \"user\", \"content\": \"What is Unreal Engine?\"}]}");
FString ContentPath = TEXT("choices[0].message.content");

// Function signature (simplified)
UPromptBlueprintFunctionLibrary::SendLLMRequest(
    URL,
    Key,
    Payload,
    ContentPath,
    // Success/Failure handling via delegates in C++
    ...
);
