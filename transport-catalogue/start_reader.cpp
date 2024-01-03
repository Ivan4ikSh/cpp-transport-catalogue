#include "start_reader.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>
using namespace transport;

void BusInfo(const Catalogue& transport_catalogue, std::string value, std::ostream& output) {
    if (transport_catalogue.FindBus(value) == nullptr) {
        output << "Bus " << value << ": not found\n";
        return;
    }

    output << "Bus " << value << ": ";
    output << transport_catalogue.GetStopsCount(value) << " stops on route, ";
    output << transport_catalogue.GetUiniqueStopsCount(value) << " unique stops, ";
    output << transport_catalogue.GetBusRouteDistance(value) << " route length\n";
}

void StopInfo(const Catalogue& transport_catalogue, std::string value, std::ostream& output) {
    if (transport_catalogue.FindStop(value) == nullptr) {
        output << "Stop " << value << ": not found\n";
        return;
    }

    std::set<std::string> stop_list = transport_catalogue.FindStopsForBus(value);
    if (stop_list.size() == 0) {
        output << "Stop " << value << ": no buses\n";
        return;
    }
    
    output << "Stop " << value << ": buses ";
    for (const auto& bus : stop_list) {
        output << bus << " ";
    }
    output << "\n";
}

void ParseAndPrintStat(const Catalogue& transport_catalogue, std::string_view request, std::ostream& output) {
    auto pos = request.find(' ');

    std::string command = move(std::string(request.substr(0, pos)));
    std::string value = move(std::string(request.substr(pos + 1)));

    if (command == "Bus") {
        BusInfo(transport_catalogue, value, output);
    }
    else if (command == "Stop") {
        StopInfo(transport_catalogue, value, output);
    }
}
