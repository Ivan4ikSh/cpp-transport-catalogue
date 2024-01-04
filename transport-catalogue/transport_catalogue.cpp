#include "transport_catalogue.h"
#include <algorithm>
#include <iosfwd>
#include <string_view>
#include <iostream>

using namespace std::literals;

void transport::Catalogue::AddStop(const std::string& name, const geo::Coordinates& coords) {
    all_stops_.push_back({ name, coords });
    stopname_to_stop_[all_stops_.back().name] = &all_stops_.back();

    SyncBuses(name, coords);
}

void transport::Catalogue::AddBus(const std::string_view& bus_number, const std::vector<std::string_view>& stops_sv, const bool is_circle) {
    std::vector<type::Stop*> stops = SyncStops(bus_number, stops_sv);

    all_buses_.push_back({ std::string(bus_number), stops, is_circle });
    busname_to_bus_[all_buses_.back().number] = &all_buses_.back();

    for (const auto& stop : busname_to_bus_.at(bus_number)->stops) {
        buses_by_stop_[stop->name].insert(busname_to_bus_.at(bus_number)->number);
    }
}

transport::type::Bus* transport::Catalogue::FindBus(std::string_view bus_number) const {
    return busname_to_bus_.count(bus_number) ? busname_to_bus_.at(bus_number) : nullptr;
}

transport::type::Stop* transport::Catalogue::FindStop(std::string_view stop_name) const {
    return stopname_to_stop_.count(stop_name) ? stopname_to_stop_.at(stop_name) : nullptr;
}

size_t transport::Catalogue::GetStopsCount(std::string_view bus_number) const {
    return FindBus(bus_number)->stops.size();
}

size_t transport::Catalogue::GetUiniqueStopsCount(std::string_view bus_number) const {
    std::unordered_set<std::string_view> unique_words;
    for (const auto& stop : busname_to_bus_.at(bus_number)->stops) {
        unique_words.insert(stop->name);
    }

    return unique_words.size();
}
double transport::Catalogue::GetBusRouteDistance(std::string_view bus_number) const {
    transport::type::Bus* bus = FindBus(bus_number);

    double route = 0;
    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        route += geo::ComputeDistance(bus->stops[i]->coordinates, bus->stops[i + 1]->coordinates);
    }
    return route;
}
std::set<std::string> transport::Catalogue::FindStopsForBus(std::string& bus_number) const {
    return buses_by_stop_.count(bus_number) ? buses_by_stop_.at(bus_number) : std::set<std::string>{};
}

std::string transport::Catalogue::BusInfo(std::string value) const {
    if (FindBus(value) == nullptr) {
        return "Bus "s + value + ": not found\n"s;
    }

    return "Bus "s + value + ": "s +
        std::to_string(GetStopsCount(value)) + " stops on route, "s +
        std::to_string(GetUiniqueStopsCount(value)) + " unique stops, "s +
        std::to_string(GetBusRouteDistance(value)) + " route length\n"s;
}

std::string transport::Catalogue::StopInfo(std::string value) const {
    if (FindStop(value) == nullptr) {
        return "Stop "s + value + ": not found\n"s;
    }

    std::set<std::string> stop_list = FindStopsForBus(value);
    if (stop_list.size() == 0) {
        return "Stop "s + value + ": no buses\n"s;
    }

    std::string output = "Stop "s + value + ": buses "s;
    for (const auto& bus : stop_list) {
        output += bus + " "s;
    }
    output += "\n"s;

    return output;
}

void transport::Catalogue::SyncBuses(const std::string name, geo::Coordinates coords) {
    for (auto& bus_info : all_buses_) {
        for (auto& stop : bus_info.stops) {
            if (stop->name == name) {
                stop->name = std::move(name);
                stop->coordinates = coords;
            }
        }
    }
}

std::vector<transport::type::Stop*> transport::Catalogue::SyncStops(const std::string_view bus_number, const std::vector<std::string_view> stops_sv) {
    std::vector<type::Stop*> stops;
    for (const std::string_view& stop : stops_sv) {
        type::Stop* found = FindStop(stop);
        if (found != nullptr) {
            stops.push_back(found);
        }
        else {
            type::Stop* s = new type::Stop;
            s->name = stop;
            s->coordinates = { 0,0 };
            stops.push_back(s);
        }
    }
    return stops;
}

