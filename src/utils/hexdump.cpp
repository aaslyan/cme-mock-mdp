#include "utils/hexdump.h"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace cme_mock {

void hexdump(const uint8_t* data, size_t length, const std::string& label)
{
    if (!label.empty()) {
        std::cout << label << " (" << length << " bytes):" << std::endl;
    }

    for (size_t i = 0; i < length; ++i) {
        // Print offset at beginning of each line
        if (i % 16 == 0) {
            std::cout << std::hex << std::setw(8) << std::setfill('0') << i << ": ";
        }

        // Print hex byte
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]) << " ";

        // Add extra space after 8 bytes for readability
        if ((i + 1) % 8 == 0) {
            std::cout << " ";
        }

        // Print ASCII representation at end of line
        if ((i + 1) % 16 == 0 || i == length - 1) {
            // Pad with spaces if last line is incomplete
            size_t remaining = 16 - ((i + 1) % 16);
            if (remaining != 16) {
                for (size_t j = 0; j < remaining; ++j) {
                    std::cout << "   ";
                }
                if (remaining > 8) {
                    std::cout << " ";
                }
            }

            std::cout << " |";
            // Print ASCII characters for this line
            size_t line_start = i - ((i + 1) % 16 == 0 ? 15 : (i % 16));
            for (size_t j = line_start; j <= i; ++j) {
                char c = static_cast<char>(data[j]);
                std::cout << (isprint(c) ? c : '.');
            }
            std::cout << "|" << std::endl;
        }
    }
    std::cout << std::dec << std::endl;
}

std::string hexdump_string(const uint8_t* data, size_t length)
{
    std::ostringstream oss;

    for (size_t i = 0; i < length; ++i) {
        // Print offset at beginning of each line
        if (i % 16 == 0) {
            oss << std::hex << std::setw(8) << std::setfill('0') << i << ": ";
        }

        // Print hex byte
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(data[i]) << " ";

        // Add extra space after 8 bytes for readability
        if ((i + 1) % 8 == 0) {
            oss << " ";
        }

        // Print ASCII representation at end of line
        if ((i + 1) % 16 == 0 || i == length - 1) {
            // Pad with spaces if last line is incomplete
            size_t remaining = 16 - ((i + 1) % 16);
            if (remaining != 16) {
                for (size_t j = 0; j < remaining; ++j) {
                    oss << "   ";
                }
                if (remaining > 8) {
                    oss << " ";
                }
            }

            oss << " |";
            // Print ASCII characters for this line
            size_t line_start = i - ((i + 1) % 16 == 0 ? 15 : (i % 16));
            for (size_t j = line_start; j <= i; ++j) {
                char c = static_cast<char>(data[j]);
                oss << (isprint(c) ? c : '.');
            }
            oss << "|" << std::endl;
        }
    }
    oss << std::dec;
    return oss.str();
}

void hexdump_compact(const uint8_t* data, size_t length, const std::string& label)
{
    if (!label.empty()) {
        std::cout << label << " (" << length << " bytes): ";
    }

    for (size_t i = 0; i < length; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]);
        if (i + 1 < length) {
            std::cout << " ";
        }
    }
    std::cout << std::dec << std::endl;
}

} // namespace cme_mock