#pragma once

#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

// Native JSON parser to replace nlohmann dependency
// Supports the subset of JSON needed for our configuration files

namespace native_json {

class JsonValue;

using JsonObject = std::map<std::string, std::shared_ptr<JsonValue>>;
using JsonArray = std::vector<std::shared_ptr<JsonValue>>;
using JsonVariant = std::variant<std::string, double, bool, JsonObject, JsonArray>;

class JsonValue {
private:
    JsonVariant value_;

public:
    explicit JsonValue(const std::string& str)
        : value_(str)
    {
    }
    explicit JsonValue(double num)
        : value_(num)
    {
    }
    explicit JsonValue(bool b)
        : value_(b)
    {
    }
    explicit JsonValue(const JsonObject& obj)
        : value_(obj)
    {
    }
    explicit JsonValue(const JsonArray& arr)
        : value_(arr)
    {
    }

    bool is_string() const { return std::holds_alternative<std::string>(value_); }
    bool is_number() const { return std::holds_alternative<double>(value_); }
    bool is_bool() const { return std::holds_alternative<bool>(value_); }
    bool is_object() const { return std::holds_alternative<JsonObject>(value_); }
    bool is_array() const { return std::holds_alternative<JsonArray>(value_); }

    const std::string& as_string() const { return std::get<std::string>(value_); }
    double as_number() const { return std::get<double>(value_); }
    bool as_bool() const { return std::get<bool>(value_); }
    const JsonObject& as_object() const { return std::get<JsonObject>(value_); }
    const JsonArray& as_array() const { return std::get<JsonArray>(value_); }

    // Conversion operators for nlohmann compatibility
    operator std::string() const
    {
        if (is_string())
            return as_string();
        if (is_number())
            return std::to_string(as_number());
        if (is_bool())
            return as_bool() ? "true" : "false";
        return "";
    }

    operator int() const
    {
        if (is_number())
            return static_cast<int>(as_number());
        if (is_string()) {
            try {
                return std::stoi(as_string());
            } catch (...) {
                return 0;
            }
        }
        return 0;
    }

    operator long() const
    {
        if (is_number())
            return static_cast<long>(as_number());
        if (is_string()) {
            try {
                return std::stol(as_string());
            } catch (...) {
                return 0;
            }
        }
        return 0;
    }

    operator double() const
    {
        if (is_number())
            return as_number();
        if (is_string()) {
            try {
                return std::stod(as_string());
            } catch (...) {
                return 0.0;
            }
        }
        return 0.0;
    }

    operator bool() const
    {
        if (is_bool())
            return as_bool();
        if (is_string()) {
            const std::string& s = as_string();
            return s == "true" || s == "1" || s == "yes";
        }
        if (is_number())
            return as_number() != 0.0;
        return false;
    }

    // nlohmann-like interface
    bool contains(const std::string& key) const
    {
        if (!is_object())
            return false;
        const auto& obj = as_object();
        return obj.find(key) != obj.end();
    }

    std::shared_ptr<JsonValue> operator[](const std::string& key) const
    {
        if (!is_object())
            return nullptr;
        const auto& obj = as_object();
        auto it = obj.find(key);
        return (it != obj.end()) ? it->second : nullptr;
    }

    std::shared_ptr<JsonValue> at(const std::string& key) const
    {
        return operator[](key);
    }

    template <typename T>
    T value(const std::string& key, const T& default_value) const
    {
        auto val = operator[](key);
        if (!val)
            return default_value;

        if constexpr (std::is_same_v<T, std::string>) {
            return val->is_string() ? val->as_string() : default_value;
        } else if constexpr (std::is_same_v<T, int>) {
            return static_cast<T>(*val);
        } else if constexpr (std::is_same_v<T, double>) {
            return static_cast<T>(*val);
        } else if constexpr (std::is_same_v<T, bool>) {
            return static_cast<T>(*val);
        }
        return default_value;
    }

    // Iterator support for arrays and objects
    class iterator {
    private:
        const JsonValue* parent_;
        size_t index_;
        JsonObject::const_iterator obj_it_;
        bool is_array_;

    public:
        iterator(const JsonValue* parent, size_t index)
            : parent_(parent)
            , index_(index)
            , is_array_(true)
        {
        }

        iterator(const JsonValue* parent, JsonObject::const_iterator it)
            : parent_(parent)
            , obj_it_(it)
            , is_array_(false)
        {
        }

        std::shared_ptr<JsonValue> operator*() const
        {
            if (is_array_) {
                const auto& arr = parent_->as_array();
                return (index_ < arr.size()) ? arr[index_] : nullptr;
            } else {
                return obj_it_->second;
            }
        }

        iterator& operator++()
        {
            if (is_array_) {
                ++index_;
            } else {
                ++obj_it_;
            }
            return *this;
        }

        bool operator!=(const iterator& other) const
        {
            if (is_array_ != other.is_array_)
                return true;
            if (is_array_) {
                return index_ != other.index_;
            } else {
                return obj_it_ != other.obj_it_;
            }
        }
    };

    iterator begin() const
    {
        if (is_array()) {
            return iterator(this, 0);
        } else if (is_object()) {
            return iterator(this, as_object().begin());
        }
        return end();
    }

    iterator end() const
    {
        if (is_array()) {
            return iterator(this, as_array().size());
        } else if (is_object()) {
            return iterator(this, as_object().end());
        }
        return iterator(this, 0);
    }
};

class JsonParser {
private:
    std::string content_;
    size_t pos_;

    void skip_whitespace()
    {
        while (pos_ < content_.length() && std::isspace(content_[pos_])) {
            pos_++;
        }
    }

    void skip_comments()
    {
        skip_whitespace();
        if (pos_ < content_.length() - 1 && content_[pos_] == '/' && content_[pos_ + 1] == '/') {
            // Skip to end of line
            while (pos_ < content_.length() && content_[pos_] != '\n') {
                pos_++;
            }
            skip_whitespace();
        }
    }

    std::string parse_string()
    {
        if (content_[pos_] != '"') {
            throw std::runtime_error("Expected '\"' at position " + std::to_string(pos_));
        }
        pos_++; // Skip opening quote

        std::string result;
        while (pos_ < content_.length() && content_[pos_] != '"') {
            if (content_[pos_] == '\\' && pos_ + 1 < content_.length()) {
                pos_++; // Skip backslash
                switch (content_[pos_]) {
                case 'n':
                    result += '\n';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case '"':
                    result += '"';
                    break;
                default:
                    result += content_[pos_];
                    break;
                }
            } else {
                result += content_[pos_];
            }
            pos_++;
        }

        if (pos_ >= content_.length()) {
            throw std::runtime_error("Unterminated string");
        }
        pos_++; // Skip closing quote
        return result;
    }

    double parse_number()
    {
        size_t start = pos_;
        if (content_[pos_] == '-')
            pos_++;

        while (pos_ < content_.length() && (std::isdigit(content_[pos_]) || content_[pos_] == '.')) {
            pos_++;
        }

        std::string num_str = content_.substr(start, pos_ - start);
        return std::stod(num_str);
    }

    bool parse_bool()
    {
        if (content_.substr(pos_, 4) == "true") {
            pos_ += 4;
            return true;
        } else if (content_.substr(pos_, 5) == "false") {
            pos_ += 5;
            return false;
        }
        throw std::runtime_error("Expected boolean value");
    }

    std::shared_ptr<JsonValue> parse_value()
    {
        skip_comments();

        if (pos_ >= content_.length()) {
            throw std::runtime_error("Unexpected end of input");
        }

        char c = content_[pos_];

        if (c == '"') {
            return std::make_shared<JsonValue>(parse_string());
        } else if (c == '{') {
            return std::make_shared<JsonValue>(parse_object());
        } else if (c == '[') {
            return std::make_shared<JsonValue>(parse_array());
        } else if (c == 't' || c == 'f') {
            return std::make_shared<JsonValue>(parse_bool());
        } else if (c == 'n') {
            // null
            pos_ += 4; // Skip "null"
            return std::make_shared<JsonValue>(std::string(""));
        } else if (std::isdigit(c) || c == '-') {
            return std::make_shared<JsonValue>(parse_number());
        }

        throw std::runtime_error("Unexpected character: " + std::string(1, c));
    }

    JsonObject parse_object()
    {
        if (content_[pos_] != '{') {
            throw std::runtime_error("Expected '{'");
        }
        pos_++; // Skip '{'

        JsonObject result;
        skip_comments();

        if (pos_ < content_.length() && content_[pos_] == '}') {
            pos_++; // Skip '}'
            return result;
        }

        while (pos_ < content_.length()) {
            skip_comments();

            // Parse key
            std::string key = parse_string();

            skip_comments();
            if (pos_ >= content_.length() || content_[pos_] != ':') {
                throw std::runtime_error("Expected ':' after key");
            }
            pos_++; // Skip ':'

            // Parse value
            auto value = parse_value();
            result[key] = value;

            skip_comments();
            if (pos_ >= content_.length()) {
                throw std::runtime_error("Unexpected end of input");
            }

            if (content_[pos_] == '}') {
                pos_++; // Skip '}'
                break;
            } else if (content_[pos_] == ',') {
                pos_++; // Skip ','
            } else {
                throw std::runtime_error("Expected ',' or '}'");
            }
        }

        return result;
    }

    JsonArray parse_array()
    {
        if (content_[pos_] != '[') {
            throw std::runtime_error("Expected '['");
        }
        pos_++; // Skip '['

        JsonArray result;
        skip_comments();

        if (pos_ < content_.length() && content_[pos_] == ']') {
            pos_++; // Skip ']'
            return result;
        }

        while (pos_ < content_.length()) {
            auto value = parse_value();
            result.push_back(value);

            skip_comments();
            if (pos_ >= content_.length()) {
                throw std::runtime_error("Unexpected end of input");
            }

            if (content_[pos_] == ']') {
                pos_++; // Skip ']'
                break;
            } else if (content_[pos_] == ',') {
                pos_++; // Skip ','
                skip_comments();
            } else {
                throw std::runtime_error("Expected ',' or ']'");
            }
        }

        return result;
    }

public:
    std::shared_ptr<JsonValue> parse(const std::string& json_string)
    {
        content_ = json_string;
        pos_ = 0;
        return parse_value();
    }

    std::shared_ptr<JsonValue> parse_file(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return parse(buffer.str());
    }
};

// Convenience function to parse JSON from file
inline std::shared_ptr<JsonValue> parse_file(const std::string& filename)
{
    JsonParser parser;
    return parser.parse_file(filename);
}

// Convenience function to parse JSON from string
inline std::shared_ptr<JsonValue> parse(const std::string& json_string)
{
    JsonParser parser;
    return parser.parse(json_string);
}

} // namespace native_json