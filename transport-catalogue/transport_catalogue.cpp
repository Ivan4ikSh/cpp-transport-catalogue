#include "transport_catalogue.h"
#include <algorithm>

namespace transport {
    void Catalogue::AddStop(const std::string name, geo::Coordinates coords) {
        std::set<std::string> buses_by_stop;
        all_stops_.push_back({ name, coords, buses_by_stop });
        stopname_to_stop_[all_stops_.back().name] = &all_stops_.back();

        SyncBuses(name, coords);
    }

    void Catalogue::AddBus(const std::string_view bus_number, const std::vector<std::string_view> stops_sv, const bool is_circle) {
        std::vector<const Stop*> stops = SyncStops(bus_number, stops_sv);

        all_buses_.push_back({ std::string(bus_number), stops, is_circle });
        busname_to_bus_[all_buses_.back().number] = &all_buses_.back();
    }

    const Bus* Catalogue::FindBus(std::string_view bus_number) const {
        return busname_to_bus_.count(bus_number) ? busname_to_bus_.at(bus_number) : nullptr;
    }

    const Stop* Catalogue::FindStop(std::string_view stop_name) const {
        return stopname_to_stop_.count(stop_name) ? stopname_to_stop_.at(stop_name) : nullptr;
    }
    void Catalogue::SyncBuses(const std::string name, geo::Coordinates coords) {
        for (auto& bus_info : all_buses_) {
            Stop* new_stop = new Stop;
            for (auto& stop : bus_info.stops) {
                if (stop->name == name) {
                    new_stop->name = std::move(name);
                    new_stop->coordinates = coords;
                    stop = new_stop;

                    for (auto& stop_info : all_stops_) {
                        if (stop_info.name == stop->name) stop_info.buses_by_stop.insert(bus_info.number);
                    }
                }
            }
        }
    }
    std::vector<const Stop*> Catalogue::SyncStops(const std::string_view bus_number, const std::vector<std::string_view> stops_sv) {
        std::vector<const Stop*> stops;
        for (const std::string_view& stop : stops_sv) {
            const Stop* found = FindStop(stop);
            if (found != nullptr) {
                stops.push_back(found);
            }
            else {
                Stop* s = new Stop;
                s->name = stop;
                s->coordinates = { 0,0 };

                stops.push_back(s);
            }
        }

        for (const auto& route_stop : stops) {
            for (auto& stop : all_stops_) {
                if (stop.name == route_stop->name) stop.buses_by_stop.insert(std::string(bus_number));
            }
        }

        return stops;
    }
}