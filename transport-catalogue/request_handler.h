#pragma once

#include "json.h"
#include "json_reader.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

#include <sstream>
class RequestHandler {
public:
    explicit RequestHandler(const transport::Catalogue& catalogue, const renderer::MapRenderer& renderer) 
        : catalogue_(catalogue), renderer_(renderer) {}
    void ProcessRequests(const json::Node& stat_requests) const;

    svg::Document RenderMap() const;
private:
    const transport::Catalogue& catalogue_;
    const renderer::MapRenderer& renderer_;

    const json::Node PrintBus(const json::Dict& request_map) const;
    const json::Node PrintStop(const json::Dict& request_map) const;
    const json::Node PrintMap(const json::Dict& request_map) const;

    std::optional<transport::data::Bus> GetBusData(const std::string bus_number) const;
    const std::set<std::string> GetBusesByStop(std::string_view stop_name) const;
};