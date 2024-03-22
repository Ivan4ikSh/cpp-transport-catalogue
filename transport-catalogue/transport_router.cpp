#include "transport_router.h"

namespace transport {
    void Router::BuildGraph(const Catalogue& catalogue) {
        const auto& all_stops = catalogue.GetSortedAllStops();
        graph::DirectedWeightedGraph<double> stops_graph(all_stops.size() * 2);
        InitStopsGraph(stops_graph, all_stops);

        const auto& all_buses = catalogue.GetSortedAllBuses();
        InitBusesGraph(stops_graph, all_buses, catalogue);

        graph_ = std::move(stops_graph);
        router_ = std::make_unique<graph::Router<double>>(graph_);
    }

    const std::optional<graph::Router<double>::RouteInfo> Router::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
        return router_->BuildRoute(stop_ids_.at(std::string(stop_from)), stop_ids_.at(std::string(stop_to)));
    }

    const graph::Edge<double>& Router::GetGraphEdge(graph::EdgeId edge_id) const {
        return graph_.GetEdge(edge_id);
    }

    void Router::InitStopsGraph(graph::DirectedWeightedGraph<double>& stop_graph, const std::map<std::string_view, const transport::type::Stop*> stop_list) {
        std::map<std::string, graph::VertexId> stop_ids;
        graph::VertexId vertex_id = 0;

        for (const auto& [stop_name, stop_info] : stop_list) {
            stop_ids[stop_info->name] = vertex_id;
            stop_graph.AddEdge({
                    stop_info->name,
                    0,
                    vertex_id,
                    ++vertex_id,
                    static_cast<double>(bus_options_.wait_time)
                });
            ++vertex_id;
        }
        stop_ids_ = std::move(stop_ids);
    }
    void Router::InitBusesGraph(graph::DirectedWeightedGraph<double>& stop_graph, const std::map<std::string_view, const transport::type::Bus*> bus_list, const Catalogue& catalogue) {
        for_each(bus_list.begin(), bus_list.end(), [&stop_graph, this, &catalogue](const auto& item) {
            const auto& bus_info = item.second;
            const auto& stops = bus_info->stops;
            size_t stops_count = stops.size();

            for (size_t i = 0; i < stops_count; ++i) {
                for (size_t j = i + 1; j < stops_count; ++j) {

                    const type::Stop* stop_from = stops[i];
                    const type::Stop* stop_to = stops[j];
                    int dist_sum = 0;
                    int dist_sum_inverse = 0;

                    for (size_t k = i + 1; k <= j; ++k) {
                        dist_sum += catalogue.GetDistance(stops[k - 1], stops[k]);
                        dist_sum_inverse += catalogue.GetDistance(stops[k], stops[k - 1]);
                    }

                    stop_graph.AddEdge({ bus_info->number,
                                          j - i,
                                          stop_ids_.at(stop_from->name) + 1,
                                          stop_ids_.at(stop_to->name),
                                          static_cast<double>(dist_sum) / (bus_options_.velocity * dist_c) });

                    if (!bus_info->circle) {
                        stop_graph.AddEdge({ bus_info->number,
                                              j - i,
                                              stop_ids_.at(stop_to->name) + 1,
                                              stop_ids_.at(stop_from->name),
                                              static_cast<double>(dist_sum_inverse) / (bus_options_.velocity * dist_c) });
                    }
                }
            }
            });
    }
}