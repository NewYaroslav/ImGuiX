#include <cctype>
#include <cstddef>

namespace ImGuiX::Utils::detail {

    // Returns true if quote at position 'pos' is escaped by an odd number of backslashes.
    inline bool is_escaped_quote(const std::string& s, std::size_t pos) noexcept {
        // Preconditions: pos < s.size() && s[pos] == '"'
        std::size_t backslash_count = 0;
        while (pos > 0 && s[--pos] == '\\') {
            ++backslash_count;
        }
        return (backslash_count % 2) == 1;
    }

    // Replace every non-newline char with space; keep '\n' and '\r' intact.
    inline void append_spaces_preserve_newlines(
            std::string& out,
            const std::string& src,
            std::size_t from,
            std::size_t to_exclusive) {
        out.reserve(out.size() + (to_exclusive - from));
        for (std::size_t i = from; i < to_exclusive; ++i) {
            const char c = src[i];
            out.push_back((c == '\n' || c == '\r') ? c : ' ');
        }
    }

} // namespace ImGuiX::Utils::detail

namespace ImGuiX::Utils {

    std::string strip_json_comments(
            const std::string& json_string,
            const bool with_whitespace) noexcept {
        enum class Comment : unsigned char { None, Line, Block };

        const std::size_t n = json_string.size();
        if (n == 0) return {};

        std::string out;
        out.reserve(n);

        bool in_string = false;
        Comment comment = Comment::None;

        // Track the start of the current "uncommented chunk".
        std::size_t chunk_begin = 0;

        // For recognizing '#' as a comment only at line start or after whitespace,
        // track whether the previous non-output char was a line break.
        bool at_line_start = true; // true at very beginning

        for (std::size_t i = 0; i < n; ++i) {
            const char c = json_string[i];
            const char next = (i + 1 < n) ? json_string[i + 1] : '\0';

            // Handle string boundaries only outside comments
            if (comment == Comment::None && c == '"') {
                if (!detail::is_escaped_quote(json_string, i)) {
                    in_string = !in_string;
                }
                // Strings affect at_line_start only if they include line breaks
                if (c == '\n' || c == '\r') at_line_start = true;
                else if (c != ' ' && c != '\t' && c != '\f' && c != '\v') at_line_start = false;
                continue;
            }

            if (in_string) {
                // Inside string, just continue scanning; line starts update:
                if (c == '\n' || c == '\r') at_line_start = true;
                else if (c != ' ' && c != '\t' && c != '\f' && c != '\v') at_line_start = false;
                continue;
            }

            // --- Not in string: detect comment starts/ends ---
            if (comment == Comment::None) {
                // Start of '//' line comment
                if (c == '/' && next == '/') {
                    // Flush the chunk before comment
                    out.append(json_string, chunk_begin, i - chunk_begin);
                    // Enter line comment
                    comment = Comment::Line;
                    // Comment starts at i; skip the second '/'
                    ++i;
                    // Chunk restarts after the comment end
                    continue;
                }
                // Start of '/* ... */' block comment
                if (c == '/' && next == '*') {
                    out.append(json_string, chunk_begin, i - chunk_begin);
                    comment = Comment::Block;
                    ++i; // skip '*'
                    continue;
                }
                // '#' single-line comment only if at line start or preceded by whitespace
                if (c == '#') {
                    // If previous emitted char was line start, or previous char is space-like:
                    // treat as comment if at_line_start == true
                    // OR the previous char exists and is whitespace.
                    bool prev_whitespace = (i == 0) ? true : !!std::isspace(static_cast<unsigned char>(json_string[i - 1]));
                    if (at_line_start || prev_whitespace) {
                        out.append(json_string, chunk_begin, i - chunk_begin);
                        comment = Comment::Line;
                        continue;
                    }
                }

                // Update line start tracking when not in comment
                if (c == '\n' || c == '\r') at_line_start = true;
                else if (c != ' ' && c != '\t' && c != '\f' && c != '\v') at_line_start = false;

                continue;
            }

            if (comment == Comment::Line) {
                // End line comment at '\n' or CRLF boundaries
                if (c == '\n') {
                    // Include comment in [chunk_begin, i) — replaced with spaces if needed
                    if (with_whitespace) {
                        detail::append_spaces_preserve_newlines(out, json_string, chunk_begin, i);
                        out.push_back('\n'); // keep newline itself
                    } else {
                        // Drop the comment segment; keep newline
                        out.push_back('\n');
                    }
                    chunk_begin = i + 1;
                    comment = Comment::None;
                    at_line_start = true;
                } else if (c == '\r' && next == '\n') {
                    if (with_whitespace) {
                        detail::append_spaces_preserve_newlines(out, json_string, chunk_begin, i);
                        out.push_back('\r');
                        out.push_back('\n');
                    } else {
                        out.push_back('\r');
                        out.push_back('\n');
                    }
                    chunk_begin = i + 2;
                    ++i; // consumed '\n'
                    comment = Comment::None;
                    at_line_start = true;
                }
                continue;
            }

            if (comment == Comment::Block) {
                // End of block comment "*/"
                if (c == '*' && next == '/') {
                    const std::size_t comment_end_inclusive = i + 1; // include '/'
                    if (with_whitespace) {
                        detail::append_spaces_preserve_newlines(out, json_string, chunk_begin, comment_end_inclusive + 1);
                    }
                    chunk_begin = comment_end_inclusive + 1;
                    ++i; // skip '/'
                    comment = Comment::None;
                    // Line start status after block depends on next char; keep conservative:
                    // if next char is newline, at_line_start will be updated in the main loop;
                    continue;
                }
                // While inside block comment — do nothing
                continue;
            }
        }

        // Flush tail
        if (comment == Comment::None) {
            out.append(json_string, chunk_begin, n - chunk_begin);
        } else {
            // Ended inside a comment: drop it or replace with spaces
            if (with_whitespace) {
                detail::append_spaces_preserve_newlines(out, json_string, chunk_begin, n);
            }
            // else: drop
        }

        return out;
    }

} // namespace ImGuiX::Utils

