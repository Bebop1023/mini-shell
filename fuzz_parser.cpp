#include "parser.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>



// Fuzzer entry point for fuzz testing the parsePipeline function
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    std::string line(reinterpret_cast<const char*>(data), size);
    Pipeline p = parsePipeline(line);

    if (!p.error.empty()) {
        return 0;
    }

    for (const std::vector<std::string>& cmd : p.commands) {
        if (cmd.empty()) {
            abort();
        }
        for (const std::string& w : cmd) {
            if (w == "|" || w == ">") {
                abort();
            }
        }
    }

    if (!p.outfile.empty() && p.commands.empty()) {
        abort();
    }
    if (p.outfile == "|" || p.outfile == ">") {
        abort();
    }

    return 0;
}
