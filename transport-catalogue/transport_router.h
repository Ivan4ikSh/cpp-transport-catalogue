#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

namespace transport {
	const double dist_c = 100.0 / 6.0;

	struct BusSettings {
		int wait_time = 0;
		double velocity = 0.0;
	};

	class Router {
	public:
		Router() = default;
		Router(const int bus_wait_time, const double bus_velocity) : bus_options_({ bus_wait_time ,bus_velocity }) {}
		Router(const BusSettings& settings, const Catalogue& catalogue) {
			bus_options_ = settings;
			BuildGraph(catalogue);
		}

		void BuildGraph(const Catalogue& catalogue);
		const std::optional<graph::Router<double>::RouteInfo> FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
		const graph::Edge<double>& GetGraphEdge(graph::EdgeId edge_id) const;
	private:
		BusSettings bus_options_;

		graph::DirectedWeightedGraph<double> graph_;
		std::map<std::string, graph::VertexId> stop_ids_;
		std::unique_ptr<graph::Router<double>> router_;

		void InitStopsGraph(graph::DirectedWeightedGraph<double>& stop_graph, const std::map<std::string_view, const transport::type::Stop*> stop_list);
		void InitBusesGraph(graph::DirectedWeightedGraph<double>& stop_graph, const std::map<std::string_view, const transport::type::Bus*> bus_list, const Catalogue& catalogue);
	};

}