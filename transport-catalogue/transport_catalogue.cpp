#include "transport_catalogue.h"
#include <algorithm>

namespace transport {
    void Catalogue::AddStop(const std::string& name, const geo::Coordinates& coords) {
        all_stops_.push_back({ name, coords });
        stopname_to_stop_[all_stops_.back().name] = &all_stops_.back();

        SyncBuses(name, coords);
    }

    void Catalogue::AddBus(const std::string_view& bus_number, const std::vector<std::string_view>& stops_sv, const bool is_circle) {
        std::vector<const Stop*> stops = SyncStops(bus_number, stops_sv);

        all_buses_.push_back({ std::string(bus_number), stops, is_circle });
        busname_to_bus_[all_buses_.back().number] = &all_buses_.back();

        for (const auto& stop : busname_to_bus_.at(bus_number)->stops) {
            buses_by_stop_[stop->name].insert(busname_to_bus_.at(bus_number)->number);
        }
    }

    const Bus* Catalogue::FindBus(std::string_view bus_number) const {
        return busname_to_bus_.count(bus_number) ? busname_to_bus_.at(bus_number) : nullptr;
    }

    const Stop* Catalogue::FindStop(std::string_view stop_name) const {
        return stopname_to_stop_.count(stop_name) ? stopname_to_stop_.at(stop_name) : nullptr;
    }
    size_t Catalogue::UiniqueStopsCount(std::string_view bus_number) const {
        std::unordered_set<std::string_view> unique_words;
        for (const auto& stop : busname_to_bus_.at(bus_number)->stops) {
            unique_words.insert(stop->name);
        }

        return unique_words.size();
    }
    std::set<std::string> Catalogue::FindStopsForBus(std::string& bus_number) const {
        return buses_by_stop_.count(bus_number) ? buses_by_stop_.at(bus_number) : std::set<std::string>{};
    }
    void Catalogue::SyncBuses(const std::string name, geo::Coordinates coords) {
        for (auto& bus_info : all_buses_) {
            Stop* new_stop = new Stop;
            for (auto& stop : bus_info.stops) {
                if (stop->name == name) {
                    new_stop->name = std::move(name);
                    new_stop->coordinates = coords;
                    stop = new_stop;
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
        return stops;
    }
}