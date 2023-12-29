#include "stat_reader.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>
using namespace transport;

void ParseAndPrintStat(const Catalogue& transport_catalogue, std::string_view request, std::ostream& output) {
    auto pos = request.find(' ');
    
    std::string command = move(std::string(request.substr(0, pos)));
    std::string value = move(std::string(request.substr(pos + 1)));

    if (command == "Bus") {
        info::Bus(transport_catalogue, transport_catalogue.FindBus(value), value, output);
    }
    else if (command == "Stop") {
        info::Stop(transport_catalogue, transport_catalogue.FindStop(value), value, output);
    }
}

void info::Bus(const Catalogue& transport_catalogue, const transport::Bus* bus, std::string value, std::ostream& output) {
    output << "Bus " << value << ": ";

    if (bus == nullptr) {
        output << "not found\n";
        return;
    }

    output << bus->stops.size() << " stops on route, ";
    output << transport_catalogue.UiniqueStopsCount(value) << " unique stops, ";

    double route = 0;
    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        route += geo::ComputeDistance(bus->stops[i]->coordinates, bus->stops[i + 1]->coordinates);
    }
    output << route << " route length\n";
}

void info::Stop(const Catalogue& transport_catalogue, const transport::Stop* stop, std::string value, std::ostream& output){
    std::set<std::string> stop_list = transport_catalogue.FindStopsForBus(value);
    output << "Stop " << value << ": ";

    if (stop == nullptr) {
        output << "not found\n";
        return;
    }
    if (stop_list.size() == 0) {
        output << "no buses\n";
        return;
    }
    output << "buses ";
    for (const auto& bus : stop_list) {
        output << bus << " ";
    }
    output << "\n";
}
