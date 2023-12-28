#pragma once
#include <deque>
#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include "geo.h"

namespace transport {
	struct Stop {
		std::string name;
		geo::Coordinates coordinates;
		std::set<std::string> buses_by_stop;
	};

	struct Bus {
		std::string number;
		std::vector<const Stop*> stops;
		bool circle;
	};

	class Catalogue {
	public:
		void AddStop(const std::string name, geo::Coordinates coords);
		void AddBus(std::string_view bus_number, const std::vector<const Stop*> stops, bool is_circle);

		const Bus* FindBus(std::string_view bus_number) const;
		const Stop* FindStop(std::string_view stop_name) const;

		std::deque<Bus> GetAllBuses() const { return all_buses_; };
	private:
		std::deque<Bus> all_buses_;
		std::deque<Stop> all_stops_;
		std::unordered_map<std::string_view, const Bus*> busname_to_bus_;
		std::unordered_map<std::string_view, const Stop*> stopname_to_stop_;
		// Реализуйте класс самостоятельно
	};
}