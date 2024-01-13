#include "transport_catalogue.h"
#include <algorithm>
#include <iosfwd>
#include <string_view>
#include <iostream>
#include <iostream>
#include <memory>
#include <map>

using namespace std::literals;

void transport::Catalogue::AddStop(const std::string& name, const geo::Coordinates& coords) {
    all_stops_.push_back({ name, coords,{} });
    stopname_to_stop_[all_stops_.back().name] = &all_stops_.back();
}

void transport::Catalogue::AddBus(const std::string_view& bus_number, const std::vector<std::string_view>& stops, const bool is_circle) {
    std::vector<type::Stop*> stop_list;
    for (const auto& route_stop : stops) {
        stop_list.push_back(FindStop(route_stop));
    }

    all_buses_.push_back({ std::string(bus_number), stop_list, is_circle });
    busname_to_bus_[all_buses_.back().number] = &all_buses_.back();

    for (const auto& route_stop : stops) {
        type::Stop* stop = FindStop(route_stop);
        if (stop) {
            stop->buses_by_stop.insert(busname_to_bus_.at(bus_number)->number);
        }
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
    if (bus == nullptr) {
        return 0;
    }

    double route = 0;
    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        auto from = bus->stops[i];
        auto to = bus->stops[i + 1];
        route += GetDistance(from, to);
    }
    return route;

}

double transport::Catalogue::GetBusRouteCurvature(std::string_view bus_number) const {
    transport::type::Bus* bus = FindBus(bus_number);
    if (bus == nullptr) {
        return 0;
    }
    double route_length = GetBusRouteDistance(bus_number);

    double geographic_length = 0.0;
    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        auto from = bus->stops[i];
        auto to = bus->stops[i + 1];
        geographic_length += geo::ComputeDistance(from->coordinates, to->coordinates);
    }
    return route_length / geographic_length; 
}

void transport::Catalogue::SetDistance(type::Stop* from, type::Stop* to, const int distance) {
    stop_distances_[{from, to}] = distance;
}

int transport::Catalogue::GetDistance(type::Stop* from, type::Stop* to) const {
    return stop_distances_.count({ from, to }) ? stop_distances_.at({ from, to }) : stop_distances_.at({ to, from });
}

std::set<std::string> transport::Catalogue::FindStopsForBus(std::string& stop_name) const {
    type::Stop* stop = FindStop(stop_name);
    return stop ? stop->buses_by_stop : std::set<std::string>{};
}

std::optional<transport::data::Bus>transport::Catalogue::GetBusData(std::string value) const {
    transport::data::Bus bus_data;
    bus_data.name = value;
    bus_data.stop_count = GetStopCount(value);
    bus_data.unique_stop_count = GetUiniqueStopsCount(value);
    bus_data.route_length = GetBusRouteDistance(value);
    bus_data.curvature = GetBusRouteCurvature(value);

    return bus_data;
}

std::optional<transport::data::Stop>transport::Catalogue::GetStopData(std::string value) const {
    transport::data::Stop stop_data;
    stop_data.name = value;
    stop_data.buses_by_stop = FindStopsForBus(value);

    return stop_data;
}

const std::unordered_map<std::pair<transport::type::Stop*, transport::type::Stop*>, int, transport::Catalogue::StopDistancesHasher> transport::Catalogue::GetStopDistances() const {
    return stop_distances_;
}
