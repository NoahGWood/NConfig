# NConfig
NConfig is a minimalist `.conf` file parser for C++ apps, using INI-like syntax with enhanced access patterns.

## Features
* `.conf` file support with `[Section]\nKey = Value`
* Fast lookups and safe fallback values
* Easy integrate with CLI and runtime config
* Optional env var overrides

## Example
```conf
[server]
Port = 8080
Host = localhost
```

## Usage
```cpp
NConfig config("app.conf");
int port = config["Server"].get("Port", 80);
std::strign host = config["Server"]["Host"]