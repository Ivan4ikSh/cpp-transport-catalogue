#include "request_handler.h"

void RequestHandler::ProcessRequests(const json::Node& stat_requests) const {
    json::Array result;
    for (auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") {
            result.push_back(PrintStop(request_map).AsMap());
        }
        if (type == "Bus") {
            result.push_back(PrintBus(request_map).AsMap());
        }
        if (type == "Map") {
            result.push_back(PrintMap(request_map).AsMap());
        }
    }

    json::Print(json::Document{ result }, std::cout);
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSVG(catalogue_.GetSortedAllBuses());
}

const json::Node RequestHandler::PrintBus(const json::Dict& request_map) const {
    json::Dict result;
    const std::string& bus_number = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();

    if (!catalogue_.FindBus(bus_number)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        transport::data::Bus bus_data = GetBusData(bus_number).value();
        result["curvature"] = bus_data.curvature;
        result["route_length"] = bus_data.route_length;
        result["stop_count"] = static_cast<int>(bus_data.stop_count);
        result["unique_stop_count"] = static_cast<int>(bus_data.unique_stop_count);
    }

    return json::Node{ result };
}

const json::Node RequestHandler::PrintStop(const json::Dict& request_map) const {
    json::Dict result;
    const std::string& stop_name = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();

    if (!catalogue_.FindStop(stop_name)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        json::Array buses;
        for (auto& bus : GetBusesByStop(stop_name)) {
            buses.push_back(bus);
        }
        result["buses"] = buses;
    }

    return json::Node{ result };
}

const json::Node RequestHandler::PrintMap(const json::Dict& request_map) const {
    json::Dict result;
    result["request_id"] = request_map.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map = RenderMap();
    map.Render(strm);
    result["map"] = strm.str();

    return json::Node{ result };
}

std::optional<transport::data::Bus> RequestHandler::GetBusData(const std::string bus_number) const {
    return catalogue_.GetBusData(bus_number);
}

const std::set<std::string> RequestHandler::GetBusesByStop(std::string_view stop_name) const {
    return catalogue_.FindStopsForBus(stop_name);
}
