#include "stat_reader.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>

void ParseAndPrintStat(const Catalogue& tansport_catalogue, std::string_view request, std::ostream& output) {
    auto pos = request.find(' ');
    
    std::string command = move(std::string(request.substr(0, pos)));
    std::string value = move(std::string(request.substr(pos + 1)));

    if (command == "Bus") {
        const Bus* bus = tansport_catalogue.FindBus(value);
        output << "Bus " << value << ": ";

        if (bus == nullptr) {
            output << "not found\n";
            return;
        }

        output << bus->stops.size() << " stops on route, ";
        std::unordered_set<std::string_view> unique_words;
        for (const auto& stop : bus->stops) {
            unique_words.insert(stop->name);
        }
        output << unique_words.size() << " unique stops, ";

        double route = 0;
        for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
            route += geo::ComputeDistance(bus->stops[i]->coordinates, bus->stops[i + 1]->coordinates);
        }
        output << route << " route length\n";
    }
    else if (command == "Stop") {
        const Stop* stop = tansport_catalogue.FindStop(value);
        output << "Stop " << value << ": ";

        if (stop == nullptr) {
            output << "not found\n";
            return;
        }
        if (tansport_catalogue.FindStop(value)->buses_by_stop.size() == 0) {
            output << "no buses\n";
            return;
        }

        output << "buses ";
        for (const auto& bus : stop->buses_by_stop) {
            output << bus << " ";
        }
        output << "\n";
    }
}