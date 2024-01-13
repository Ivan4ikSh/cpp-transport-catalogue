#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

#include "geo.h"
#include "transport_catalogue.h"

using namespace transport;

namespace input {
    struct Description {
        explicit operator bool() const {
            return !command.empty();
        }

        bool operator!() const {
            return !operator bool();
        }

        std::string command;
        std::string id;
        std::string description;
    };

    class Reader {
    public:
        void ParseLine(std::string_view line);
        void ApplyCommands(Catalogue& catalogue) const;
    private:
        std::vector<Description> commands_;
    };
}