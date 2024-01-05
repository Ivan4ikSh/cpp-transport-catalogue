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

size_t transport::Catalogue::GetStopCount(std::string_view bus_number) const {
    return busname_to_bus_.count(bus_number) ? FindBus(bus_number)->stops.size() : 0;
}

size_t transport::Catalogue::GetUiniqueStopsCount(std::string_view bus_number) const {
    std::unordered_set<std::string_view> unique_words;
    
    if (busname_to_bus_.count(bus_number)) {
        for (const auto& stop : busname_to_bus_.at(bus_number)->stops) {
            unique_words.insert(stop->name);
        }
    }
    else {
        return 0;
    }
    return unique_words.size();
}
double transport::Catalogue::GetBusRouteDistance(std::string_view bus_number) const {
    transport::type::Bus* bus = FindBus(bus_number);
    if (bus==nullptr) {
        return 0.0;
    }

    double route = 0;
    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        route += geo::ComputeDistance(bus->stops[i]->coordinates, bus->stops[i + 1]->coordinates);
    }
    return route;
}
std::set<std::string> transport::Catalogue::FindStopsForBus(std::string& bus_number) const {
    return buses_by_stop_.count(bus_number) ? buses_by_stop_.at(bus_number) : std::set<std::string>{};
}

std::optional<transport::data::Bus>transport::Catalogue::GetBusData(std::string value) const {
    transport::data::Bus bus_data;
    bus_data.name = value;
    bus_data.stop_count = GetStopCount(value);
    bus_data.unique_stop_count = GetUiniqueStopsCount(value);
    bus_data.route_length = GetBusRouteDistance(value);

    return bus_data;
}

std::optional<transport::data::Stop>transport::Catalogue::GetStopData(std::string value) const {
    transport::data::Stop stop_data;
    stop_data.name = value;
    stop_data.buses_by_stop = FindStopsForBus(value);

    return stop_data;
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

