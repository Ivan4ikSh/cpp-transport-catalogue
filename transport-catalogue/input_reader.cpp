#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>

geo::Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) return { nan, nan };

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return { lat, lng };
}

std::unordered_map<std::string, int> ParseDistances(std::string str) {
    if (str.find(",", str.find(",") + 1) > str.size()) return{};

    str = str.substr(str.find(",", str.find(",") + 1) + 2);

    std::unordered_map<std::string, int> distances;
    while (!str.empty()) {
        std::string stop_to_name;
        int distance = std::stoi(str.substr(0, str.find_first_of("m to ")));
        str.erase(0, str.find_first_of("m to ") + 5);

        if (str.find("m to ") == str.npos) {
            stop_to_name = str.substr(0, str.npos - 1);
            str.clear();
        }
        else {
            stop_to_name = str.substr(0, str.find_first_of(','));
            str.erase(0, str.find_first_of(',') + 2);
        }
        distances[stop_to_name] = distance;
    }
    return distances;
}

std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) return {};
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

std::vector<std::string> Split(std::string_view string, char delim) {
    std::vector<std::string> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(std::string(substr));
        }
        pos = delim_pos + 1;
    }

    return result;
}

std::vector<std::string> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) return Split(route, '>');

    auto stops = Split(route, '-');
    std::vector<std::string> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

input::Description ParseDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) return {};

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) return {};

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) return {};

    return { std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1)) };
}

void input::Reader::ParseLine(std::string_view line) {
    auto command_description = ParseDescription(line);
    if (command_description) commands_.push_back(std::move(command_description));
}


void input::Reader::ApplyCommands([[maybe_unused]] Catalogue& catalogue) const {
    std::vector<std::pair<std::string, std::string>> query_bus;
    std::vector<std::pair<std::string, std::string>> query_stop;

    for (const auto& elem : commands_) {
        if (elem.command == "Stop") {
            query_stop.push_back({ elem.id, elem.description });
        }
        if (elem.command == "Bus") {
            query_bus.push_back({ elem.id, elem.description });
        }
    }

    for (auto& stop : query_stop) {
        catalogue.AddStop(stop.first, ParseCoordinates(stop.second));
    }

    for (auto& stop : query_stop) {
        for (const auto& distance : ParseDistances(stop.second)) {
            catalogue.SetDistance(catalogue.FindStop(stop.first), catalogue.FindStop(distance.first), distance.second);
        }
    }

    for (auto& bus : query_bus) {
        catalogue.AddBus(bus.first, ParseRoute(bus.second), (bus.second.find('>') != std::string_view::npos) ? true : false);
        bus = {};
    }
}