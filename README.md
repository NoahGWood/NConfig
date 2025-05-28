# NConfig

**NConfig** is a minimalist `.conf` file parser for C++ apps, using INI-like syntax with enhanced access patterns.

## ✨ Features

- Easy `.conf` file parsing: supports `[section] key = value` format
- Type-safe `get<T>()` and `set<T>()` functions
- Built-in support for:
  - `int`, `unsigned int`, `long`, `long long`, `float`, `double`, `bool`, `std::string`
  - `std::vector<T>` of the above types
- Config saving and loading
- Fallback values for missing keys
- Section/key discovery via `keys()`, `sections()`, and `keys_in()`
---
## 🔧 Example Usage

```cpp
#include "NConfig.h"

NConfig config;
config.load("settings.conf");

// Set values
config.set("window.width", 1280);
config.set("window.fullscreen", true);
config.set("player.names", std::vector<std::string>{"Alice", "Bob"});

// Get values
int width = config.get("window.width", 800);
bool fullscreen = config.get("window.fullscreen", false);
auto names = config.get<std::vector<std::string>>("player.names", {});

// Save config
config.save("settings.conf");
```
---
## 🔍 Utility Functions
```cpp 
config.has_key("audio.volume");               // true/false
config.keys();                                // All keys
config.sections();                            // All section names
config.keys_in("audio");                      // All keys under 'audio'
```
---
## 🧪 Testing
NConfig comes with [`NTest`](github.com/NoahGWood/NTest) unit tests to verify correctness. Run:
```bash
./build.sh
```
## 📄 License
[TBD]
