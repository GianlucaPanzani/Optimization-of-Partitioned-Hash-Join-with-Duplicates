#include "results.hpp"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

namespace {

struct JsonValue {
    using Object = std::map<std::string, JsonValue>;

    JsonValue() : value(Object{}) {}
    JsonValue(Object object) : value(std::move(object)) {}
    JsonValue(std::string string) : value(std::move(string)) {}
    JsonValue(const char* string) : value(std::string(string)) {}
    JsonValue(double number) : value(number) {}

    bool is_object() const {
        return std::holds_alternative<Object>(value);
    }

    bool is_string() const {
        return std::holds_alternative<std::string>(value);
    }

    Object& as_object() {
        if (!is_object()) {
            throw std::runtime_error("Expected a JSON object");
        }
        return std::get<Object>(value);
    }

    const Object& as_object() const {
        if (!is_object()) {
            throw std::runtime_error("Expected a JSON object");
        }
        return std::get<Object>(value);
    }

    const std::string& as_string() const {
        if (!is_string()) {
            throw std::runtime_error("Expected a JSON string");
        }
        return std::get<std::string>(value);
    }

    double as_number() const {
        if (!std::holds_alternative<double>(value)) {
            throw std::runtime_error("Expected a JSON number");
        }
        return std::get<double>(value);
    }

    std::variant<Object, std::string, double> value;
};

class JsonParser {
public:
    explicit JsonParser(const std::string& input) : input_(input) {}

    JsonValue parse() {
        JsonValue result = parse_value();
        skip_whitespace();
        if (!eof()) {
            throw std::runtime_error("Unexpected trailing content in JSON input");
        }
        return result;
    }

private:
    JsonValue parse_value() {
        skip_whitespace();
        const char current = peek();
        if (current == '{') {
            return JsonValue(parse_object());
        }
        if (current == '"') {
            return JsonValue(parse_string());
        }
        if (current == '-' || std::isdigit(static_cast<unsigned char>(current))) {
            return JsonValue(parse_number());
        }

        throw std::runtime_error("Unsupported JSON value at position " + std::to_string(pos_));
    }

    JsonValue::Object parse_object() {
        expect('{');
        skip_whitespace();

        JsonValue::Object object;
        if (consume('}')) {
            return object;
        }

        while (true) {
            skip_whitespace();
            const std::string key = parse_string();
            skip_whitespace();
            expect(':');
            object[key] = parse_value();
            skip_whitespace();

            if (consume('}')) {
                break;
            }
            expect(',');
        }

        return object;
    }

    std::string parse_string() {
        expect('"');
        std::string result;

        while (!eof()) {
            const char current = input_[pos_++];
            if (current == '"') {
                return result;
            }

            if (current == '\\') {
                if (eof()) {
                    throw std::runtime_error("Unterminated escape sequence in JSON string");
                }

                const char escaped = input_[pos_++];
                switch (escaped) {
                    case '"':
                    case '\\':
                    case '/':
                        result.push_back(escaped);
                        break;
                    case 'b':
                        result.push_back('\b');
                        break;
                    case 'f':
                        result.push_back('\f');
                        break;
                    case 'n':
                        result.push_back('\n');
                        break;
                    case 'r':
                        result.push_back('\r');
                        break;
                    case 't':
                        result.push_back('\t');
                        break;
                    default:
                        throw std::runtime_error("Unsupported JSON escape sequence");
                }
                continue;
            }

            result.push_back(current);
        }

        throw std::runtime_error("Unterminated JSON string");
    }

    double parse_number() {
        const std::size_t start = pos_;

        if (peek() == '-') {
            ++pos_;
        }

        if (!std::isdigit(static_cast<unsigned char>(peek()))) {
            throw std::runtime_error("Invalid JSON number at position " + std::to_string(pos_));
        }
        while (std::isdigit(static_cast<unsigned char>(peek()))) {
            ++pos_;
        }

        if (peek() == '.') {
            ++pos_;
            if (!std::isdigit(static_cast<unsigned char>(peek()))) {
                throw std::runtime_error("Invalid JSON number at position " + std::to_string(pos_));
            }
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                ++pos_;
            }
        }

        if (peek() == 'e' || peek() == 'E') {
            ++pos_;
            if (peek() == '+' || peek() == '-') {
                ++pos_;
            }
            if (!std::isdigit(static_cast<unsigned char>(peek()))) {
                throw std::runtime_error("Invalid JSON exponent at position " + std::to_string(pos_));
            }
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                ++pos_;
            }
        }

        return std::stod(input_.substr(start, pos_ - start));
    }

    void skip_whitespace() {
        while (!eof() && std::isspace(static_cast<unsigned char>(input_[pos_]))) {
            ++pos_;
        }
    }

    void expect(char expected) {
        if (!consume(expected)) {
            throw std::runtime_error(
                std::string("Expected '") + expected + "' at position " + std::to_string(pos_)
            );
        }
    }

    bool consume(char expected) {
        if (peek() == expected) {
            ++pos_;
            return true;
        }
        return false;
    }

    char peek() const {
        return eof() ? '\0' : input_[pos_];
    }

    bool eof() const {
        return pos_ >= input_.size();
    }

    const std::string& input_;
    std::size_t pos_ = 0;
};

std::string escape_json_string(const std::string& input) {
    std::string escaped;
    escaped.reserve(input.size());

    for (const char current : input) {
        switch (current) {
            case '"':
                escaped += "\\\"";
                break;
            case '\\':
                escaped += "\\\\";
                break;
            case '\b':
                escaped += "\\b";
                break;
            case '\f':
                escaped += "\\f";
                break;
            case '\n':
                escaped += "\\n";
                break;
            case '\r':
                escaped += "\\r";
                break;
            case '\t':
                escaped += "\\t";
                break;
            default:
                escaped.push_back(current);
                break;
        }
    }

    return escaped;
}

void write_json(const JsonValue& value, std::ostream& out, int indent_level = 0) {
    if (value.is_object()) {
        const JsonValue::Object& object = value.as_object();
        if (object.empty()) {
            out << "{}";
            return;
        }

        const std::string indent(indent_level * 4, ' ');
        const std::string child_indent((indent_level + 1) * 4, ' ');

        out << "{\n";
        bool first = true;
        for (const auto& [key, child] : object) {
            if (!first) {
                out << ",\n";
            }
            first = false;
            out << child_indent << "\"" << escape_json_string(key) << "\": ";
            write_json(child, out, indent_level + 1);
        }
        out << "\n" << indent << "}";
        return;
    }

    if (value.is_string()) {
        out << "\"" << escape_json_string(value.as_string()) << "\"";
        return;
    }

    const std::streamsize old_precision = out.precision();
    out << std::setprecision(17) << value.as_number();
    out.precision(old_precision);
}

JsonValue load_json_object(const std::string& json_path) {
    std::ifstream in(json_path);
    if (!in) {
        throw std::runtime_error("Cannot open JSON file for reading: " + json_path);
    }

    const std::string content{
        std::istreambuf_iterator<char>(in),
        std::istreambuf_iterator<char>()
    };

    if (content.empty()) {
        return JsonValue(JsonValue::Object{});
    }

    JsonParser parser(content);
    JsonValue root = parser.parse();
    if (!root.is_object()) {
        throw std::runtime_error("Top-level JSON value must be an object");
    }
    return root;
}

JsonValue& ensure_object_member(JsonValue::Object& object, const std::string& key) {
    JsonValue& value = object[key];
    if (!value.is_object()) {
        value = JsonValue(JsonValue::Object{});
    }
    return value;
}

} // namespace

double compute_throughput(std::uint64_t total_elements, double partition_time_seconds) {
    if (partition_time_seconds <= 0.0) {
        throw std::invalid_argument("partition_time_seconds must be > 0");
    }
    return static_cast<double>(total_elements) / partition_time_seconds;
}


void update_results_json(
    const std::string& json_path,
    const std::string& executable_name,
    std::uint64_t N,
    std::uint32_t P,
    double throughput,
    double t,
    const std::string& checksum,
    const std::string& hash_name
) {
    JsonValue root(JsonValue::Object{});

    if (std::filesystem::exists(json_path)) {
        root = load_json_object(json_path);
    }

    const std::string top_key = std::to_string(N) + " " + std::to_string(P); // keys format: "N P"
    const std::string hash_key = hash_name.empty() ? "unspecified" : hash_name;

    JsonValue& top_level = ensure_object_member(root.as_object(), top_key);
    JsonValue& hash_level = ensure_object_member(top_level.as_object(), hash_key);
    JsonValue& executable_level = ensure_object_member(hash_level.as_object(), executable_name);
    JsonValue::Object& metrics = executable_level.as_object();

    metrics["throughput"] = JsonValue(throughput);
    metrics["time"] = JsonValue(t);
    metrics["checksum"] = JsonValue(checksum);
    metrics["hash"] = JsonValue(hash_name);

    const std::filesystem::path output_path(json_path);
    const std::filesystem::path parent = output_path.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }

    std::ofstream out(json_path);
    if (!out) {
        throw std::runtime_error("Cannot open JSON file for writing: " + json_path);
    }

    write_json(root, out);
    out << "\n";
}
