#pragma once
#ifndef PCH_HPP
#define PCH_HPP

    #include "mimalloc/mimalloc-new-delete.h" // @note https://github.com/microsoft/mimalloc

    #include <cstring>
    #include <string>
    #include <unordered_map>
    #include <vector>
    #include <algorithm>
    #include <ranges>
    #include <chrono>
    #include <thread>
    #include <fstream>
    #include <sstream>
    #include <type_traits>
    
    // Add using declarations for common types
    using std::string;
    using std::vector;
    using std::stringstream;
    using std::is_same_v;
    using std::decay_t;
    using std::is_arithmetic_v;
    using std::to_string;
    
    // Check for std::format support
    #if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
        #include <format>
    #else
        // Simple format fallback using sprintf-style formatting
        #include <cstdio>
        
        namespace std {
            template<typename... Args>
            string format(const string& fmt, Args&&... args) {
                // Simple implementation - convert all args to strings and use basic replacement
                auto to_string_helper = [](auto&& arg) -> string {
                    if constexpr (is_same_v<decay_t<decltype(arg)>, string>) {
                        return arg;
                    } else if constexpr (is_same_v<decay_t<decltype(arg)>, const char*>) {
                        return string(arg);
                    } else if constexpr (is_arithmetic_v<decay_t<decltype(arg)>>) {
                        return to_string(arg);
                    } else {
                        stringstream ss;
                        ss << arg;
                        return ss.str();
                    }
                };
                
                vector<string> str_args = {to_string_helper(args)...};
                string result = fmt;
                size_t pos = 0;
                size_t arg_idx = 0;
                
                // Replace {} with actual values
                while ((pos = result.find("{}", pos)) != string::npos && arg_idx < str_args.size()) {
                    result.replace(pos, 2, str_args[arg_idx]);
                    pos += str_args[arg_idx].length();
                    arg_idx++;
                }
                
                // Replace {0}, {1}, etc. with actual values
                for (size_t i = 0; i < str_args.size(); ++i) {
                    string placeholder = "{" + to_string(i) + "}";
                    pos = 0;
                    while ((pos = result.find(placeholder, pos)) != string::npos) {
                        result.replace(pos, placeholder.length(), str_args[i]);
                        pos += str_args[i].length();
                    }
                }
                
                return result;
            }
        }
    #endif
    
    // Add chrono namespace aliases for compatibility
    using std::chrono::steady_clock;
    using std::chrono::duration_cast;
    
    #include "sqlite/sqlite3.h" // @version SHA: a67c71224f5821547040b637aad7cddf4ef0778a (24/6/25)
    #include "enet/enet.h" // @version SHA: 657eaf97d9d335917c58484a4a4b5e03838ebd8e (13/11/24)
    #include "ssl/openssl/ssl.h" // @version SHA: 7bdc0d13d2b9ce1c1d0ec1f89dacc16e5d045314 (26/6/25)

    #include "database/items.hpp"
    #include "database/peer.hpp"
    #include "database/world.hpp"
    #include "packet/packet.hpp"

#if defined(_MSC_VER)
    /* cause MSVC does not know 'zu' when the compiler(MSBuild) does... */
    #pragma warning(disable: 4455)
    constexpr std::size_t operator"" zu(std::size_t size) {
        return size;
    }
#endif

#endif