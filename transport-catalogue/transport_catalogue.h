#pragma once
#include <deque>
#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include "geo.h"

namespace transport {
	namespace type {
		struct Stop {
			std::string name;
			geo::Coordinates coordinates;
		};

		struct Bus {
			std::string number;
			std::vector<Stop*> stops;
			bool circle;
		};
	}
	class Catalogue {
	public:
		void AddStop(const std::string& name, const geo::Coordinates& coords);
		void AddBus(const std::string_view& bus_number, const std::vector<std::string_view>& stops_sv, const bool is_circle);

		void OutInfo(std::string command, std::string value, std::ostream& output) const ;
	private:
		std::deque<type::Bus> all_buses_;
		std::deque<type::Stop> all_stops_;
		std::unordered_map<std::string_view, type::Bus*> busname_to_bus_;
		std::unordered_map<std::string_view, type::Stop*> stopname_to_stop_;

		std::unordered_map<std::string, std::set<std::string>> buses_by_stop_;

		void SyncBuses(const std::string name, geo::Coordinates coords);
		std::vector<type::Stop*> SyncStops(const std::string_view bus_number, const std::vector<std::string_view> stops_sv);
		type::Bus* FindBus(std::string_view bus_number) const;
		type::Stop* FindStop(std::string_view stop_name) const;
		std::set<std::string> FindStopsForBus(std::string& bus_number) const;
		size_t UiniqueStopsCount(std::string_view bus_number) const;
		// Реализуйте класс самостоятельно
	};
}