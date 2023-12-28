#include "transport_catalogue.h"
#include <algorithm>

namespace transport {
    void Catalogue::AddStop(const std::string name, geo::Coordinates coords) {
        std::set<std::string> buses_by_stop;
        all_stops_.push_back({ name, coords, buses_by_stop });
        stopname_to_stop_[all_stops_.back().name] = &all_stops_.back();

        for (auto& bus_info : all_buses_) {
            Stop* new_stop = new Stop;
            for (auto& stop : bus_info.stops) {
                if (stop->name == name) {
                    new_stop->name = name;
                    new_stop->coordinates = coords;
                    stop = new_stop;
                    for (auto& stop_info : all_stops_) {
                        if (stop_info.name == stop->name) stop_info.buses_by_stop.insert(bus_info.number);
                    }
                }
            }
        }
    }

    void Catalogue::AddBus(std::string_view bus_number, const std::vector<const Stop*> stops, bool is_circle) {
        all_buses_.push_back({ std::string(bus_number), stops, is_circle });
        busname_to_bus_[all_buses_.back().number] = &all_buses_.back();

        for (const auto& route_stop : stops) {
            for (auto& stop_ : all_stops_)
                if (stop_.name == route_stop->name) stop_.buses_by_stop.insert(std::string(bus_number));
        }
    }

    const Bus* Catalogue::FindBus(std::string_view bus_number) const {
        return busname_to_bus_.count(bus_number) ? busname_to_bus_.at(bus_number) : nullptr;
    }

    const Stop* Catalogue::FindStop(std::string_view stop_name) const {
        return stopname_to_stop_.count(stop_name) ? stopname_to_stop_.at(stop_name) : nullptr;
    }
}