#include "json_reader.h"

const json::Node& JsonReader::GetStatRequests() const {
    if (!input_.GetRoot().AsMap().count("stat_requests")) return dummy_;
    return input_.GetRoot().AsMap().at("stat_requests");
}

const json::Node& JsonReader::GetRenderSettings() const {
    if (!input_.GetRoot().AsMap().count("render_settings")) return dummy_;
    return input_.GetRoot().AsMap().at("render_settings");
}

void JsonReader::FillCatalogue(transport::Catalogue& catalogue) {
    const json::Array& arr = GetBaseRequests().AsArray();
    std::vector<json_reader::Stop> query_stop;
    std::vector<json_reader::Bus> query_bus;

    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsMap();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            query_stop.push_back(FillStop(request_stops_map));
        }
        if (type == "Bus") {
            query_bus.push_back(FillBus(request_stops_map));
        }
    }

    for (auto& stop : query_stop) {
        catalogue.AddStop(stop.stop_name, stop.coordinates);
    }

    for (auto& stop : query_stop) {
        for (const auto& distance : stop.stop_distances) {
            catalogue.SetDistance(catalogue.FindStop(stop.stop_name), catalogue.FindStop(distance.first), distance.second);
        }
    }

    for (auto& bus : query_bus) {
        catalogue.AddBus(bus.bus_number, bus.stops, bus.is_circle);
    }
}

renderer::MapRenderer JsonReader::FillRenderSettings(const json::Dict& request_map) const {
    renderer::RenderSettings render_settings;
    render_settings.width = request_map.at("width").AsDouble();
    render_settings.height = request_map.at("height").AsDouble();
    render_settings.padding = request_map.at("padding").AsDouble();
    render_settings.stop_radius = request_map.at("stop_radius").AsDouble();
    render_settings.line_width = request_map.at("line_width").AsDouble();
    render_settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();

    const json::Array& bus_label_offset = request_map.at("bus_label_offset").AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();

    const json::Array& stop_label_offset = request_map.at("stop_label_offset").AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };

    if (request_map.at("underlayer_color").IsString()) render_settings.underlayer_color = request_map.at("underlayer_color").AsString();
    else if (request_map.at("underlayer_color").IsArray()) {
        const json::Array& underlayer_color = request_map.at("underlayer_color").AsArray();
        if (underlayer_color.size() == 3) {
            render_settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
        }
        else if (underlayer_color.size() == 4) {
            render_settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
        }
        else throw std::logic_error("wrong underlayer colortype");
    }
    else throw std::logic_error("wrong underlayer color");

    render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();

    const json::Array& color_palette = request_map.at("color_palette").AsArray();
    for (const auto& color_element : color_palette) {
        if (color_element.IsString()) render_settings.color_palette.push_back(color_element.AsString());
        else if (color_element.IsArray()) {
            const json::Array& color_type = color_element.AsArray();
            uint8_t a = static_cast<uint8_t>(color_type[0].AsInt());
            uint8_t b = static_cast<uint8_t>(color_type[1].AsInt());
            uint8_t c = static_cast<uint8_t>(color_type[2].AsInt());

            if (color_type.size() == 3) {
                render_settings.color_palette.emplace_back(svg::Rgb(a,b,c));
            }
            else if (color_type.size() == 4) {
                render_settings.color_palette.emplace_back(svg::Rgba(a, b, c, color_type[3].AsDouble()));
            }
            else throw std::logic_error("wrong color_palette type");
        }
        else throw std::logic_error("wrong color_palette");
    }

    return render_settings;
}

const json::Node& JsonReader::GetBaseRequests() const {
    if (!input_.GetRoot().AsMap().count("base_requests")) {
        return dummy_;
    }
    return input_.GetRoot().AsMap().at("base_requests");
}

json_reader::Stop JsonReader::FillStop(const json::Dict& request_map) const {
    std::string stop_name = request_map.at("name").AsString();
    geo::Coordinates coordinates = { request_map.at("latitude").AsDouble(), request_map.at("longitude").AsDouble() };
    std::map<std::string, int> stop_distances;

    auto& distances = request_map.at("road_distances").AsMap();
    for (auto& [stop_name, dist] : distances) {
        stop_distances.emplace(stop_name, dist.AsInt());
    }

    return { stop_name, coordinates, stop_distances };
}

json_reader::Bus JsonReader::FillBus(const json::Dict& request_map) const {
    std::string bus_number = request_map.at("name").AsString();
    std::vector<std::string> stops;
    for (auto& stop : request_map.at("stops").AsArray()) {
        stops.push_back(stop.AsString());
    }
    bool circular_route = request_map.at("is_roundtrip").AsBool();
    return { bus_number, stops, circular_route };
}