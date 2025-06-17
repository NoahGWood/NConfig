/**
 * @file NConfig.h
 * @author Noah G. Wood (@NoahGWood)
 * @brief NConfig class for configuration handling.
 * @version 0.1
 * @date 2025-05-27
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_set>

// Trim helper
inline static std::string trim(const std::string& str) {
    size_t begin = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (begin == std::string::npos || end == std::string::npos) return "";
    return str.substr(begin, end - begin + 1);
}

class NConfig {
public:
    bool load(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file) return false;

        std::string line, current_section;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;

            if (line.front() == '[' && line.back() == ']') {
                current_section = trim(line.substr(1, line.size() - 2));
                continue;
            }

            auto sep = line.find('=');
            if (sep == std::string::npos) continue;

            std::string key = trim(line.substr(0, sep));
            std::string value = trim(line.substr(sep + 1));

            if (!current_section.empty())
                key = current_section + "." + key;

            m_Data[key] = value;
        }
        return true;
    }

    bool save(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file) return false;

        std::string last_section;
        for (const auto& [key, value] : m_Data) {
            auto dot = key.find('.');
            std::string section = (dot != std::string::npos) ? key.substr(0, dot) : "";
            std::string subkey = (dot != std::string::npos) ? key.substr(dot + 1) : key;

            if (section != last_section) {
                if (!last_section.empty()) file << "\n";
                if (!section.empty()) file << "[" << section << "]\n";
                last_section = section;
            }

            file << subkey << " = " << value << "\n";
        }
        return true;
    }

    inline std::vector<std::string> keys() const
    {
        std::vector<std::string> results;
        for(const auto& [k,_] : m_Data) results.push_back(k);
        return results;
    }

    inline std::vector<std::string> sections() const
    {
        std::unordered_set<std::string> sections;
        for(const auto& [k,_] : m_Data){
            auto dot = k.find('.');
            if (dot != std::string::npos) sections.insert(k.substr(0, dot));
        }
        return {sections.begin(), sections.end()};
    }

    inline std::vector<std::string> keys_in(const std::string& section) const
    {
        std::vector<std::string> result;
        std::string prefix = section + ".";
        for (const auto& [k, _] : m_Data) {
            if (k.compare(0, prefix.size(), prefix) == 0)
                result.push_back(k.substr(prefix.size()));
        }
        return result;
    }
    inline bool has_key(const std::string& key) const
    {
        return m_Data.find(key) != m_Data.end();
    }

    template<typename T>
    T get(const std::string& key, const T& fallback) const;

    template<typename T>
    void set(const std::string& key, const T& value);

    template<typename T>
    std::vector<T> get_list(const std::string& key, const std::vector<T>& fallback = {}, char delimiter = ',') const {
        auto it = m_Data.find(key);
        if (it == m_Data.end()) return fallback;

        std::vector<T> results;
        std::stringstream ss(it->second);
        std::string item;

        while (std::getline(ss, item, delimiter)) {
            item = trim(item);
            std::stringstream itemStream(item);
            T value;
            if (itemStream >> value) results.push_back(value);
        }
        return results;
    }

    template<typename T>
    void set_list(const std::string& key, const std::vector<T>& values, char delimiter = ',') {
        std::ostringstream joined;
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) joined << delimiter;
            joined << values[i];
        }
        m_Data[key] = joined.str();
    }

private:
    std::unordered_map<std::string, std::string> m_Data;
};

// Specializations
template<>
inline std::string NConfig::get<std::string>(const std::string& key, const std::string& fallback) const {
    auto it = m_Data.find(key);
    return it != m_Data.end() ? it->second : fallback;
}

template<>
inline void NConfig::set<std::string>(const std::string& key, const std::string& value) {
    m_Data[key] = value;
}

template<>
inline void NConfig::set<bool>(const std::string& key, const bool& value) {
    m_Data[key] = value ? "true" : "false";
}

template<>
inline bool NConfig::get<bool>(const std::string& key, const bool& fallback) const {
    auto it = m_Data.find(key);
    if (it != m_Data.end()) {
        std::string val = trim(it->second);
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        if (val == "true" || val == "1" || val == "yes" || val == "on") return true;
        if (val == "false" || val == "0" || val == "no" || val == "off") return false;
    }
    return fallback;
}

#define NCONFIG_DEFINE_GET_SET(Type, ToStrFunc, FromStrFunc)                          \
template<> inline Type NConfig::get<Type>(const std::string& key, const Type& fallback) const { \
    try { return FromStrFunc(get<std::string>(key, "")); } catch (...) { return fallback; }     \
}                                                                                   \
template<> inline void NConfig::set<Type>(const std::string& key, const Type& value) {           \
    m_Data[key] = ToStrFunc(value);                                                  \
}

NCONFIG_DEFINE_GET_SET(int, std::to_string, std::stoi)
NCONFIG_DEFINE_GET_SET(unsigned int, std::to_string, std::stoul)
NCONFIG_DEFINE_GET_SET(long, std::to_string, std::stol)
NCONFIG_DEFINE_GET_SET(long long, std::to_string, std::stoll)
NCONFIG_DEFINE_GET_SET(float, std::to_string, std::stof)
NCONFIG_DEFINE_GET_SET(double, std::to_string, std::stod)

#define NCONFIG_DEFINE_VECTOR_GET_SET(Type) \
template<> inline std::vector<Type> NConfig::get<std::vector<Type>>(const std::string& key, const std::vector<Type>& fallback) const { \
    return get_list<Type>(key, fallback); \
} \
template<> inline void NConfig::set<std::vector<Type>>(const std::string& key, const std::vector<Type>& value) { \
    set_list<Type>(key, value); \
}

NCONFIG_DEFINE_VECTOR_GET_SET(std::string)
NCONFIG_DEFINE_VECTOR_GET_SET(int)
NCONFIG_DEFINE_VECTOR_GET_SET(unsigned int)
NCONFIG_DEFINE_VECTOR_GET_SET(long)
NCONFIG_DEFINE_VECTOR_GET_SET(long long)
NCONFIG_DEFINE_VECTOR_GET_SET(float)
NCONFIG_DEFINE_VECTOR_GET_SET(double)
NCONFIG_DEFINE_VECTOR_GET_SET(bool)
