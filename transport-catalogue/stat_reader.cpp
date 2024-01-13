#include "stat_reader.h"
#include <iostream>
#include <unordered_set>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace transport;

void ParseAndPrintStat(const Catalogue& transport_catalogue, std::string_view request, std::ostream& output) {
    auto pos = request.find(' ');

    std::string command = move(std::string(request.substr(0, pos)));
    std::string value = move(std::string(request.substr(pos + 1)));

    if (command == "Bus") {
        if (transport_catalogue.FindBus(value) == nullptr) {
            output << "Bus " << value << ": not found\n";
            return;
        }
        data::Bus bus_data = transport_catalogue.GetBusData(value).value();
        output << "Bus " << bus_data.name << ": "
            << bus_data.stop_count << " stops on route, "
            << bus_data.unique_stop_count << " unique stops, "
            << bus_data.route_length << " route length, "
            << bus_data.curvature << " curvature\n";
    }
    else if (command == "Stop") {
        if (transport_catalogue.FindStop(value) == nullptr) {
            output << "Stop " << value << ": not found\n";
            return;
        }

        data::Stop stop_data = transport_catalogue.GetStopData(value).value();
        if (stop_data.buses_by_stop.size() == 0) {
            output << "Stop " << value << ": no buses\n";
            return;
        }
        output << "Stop " << stop_data.name << ": buses ";
        for (const auto& bus : stop_data.buses_by_stop) {
            output << bus << " ";
        }
        output << "\n";            
    }
}
