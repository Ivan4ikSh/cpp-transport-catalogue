#pragma once
#include "json.h"
#include "svg.h"
#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"

class JsonReader {
public:
    JsonReader(std::istream& input) : input_(json::Load(input)) {}

    const json::Node& GetStatRequests() const;
    const json::Node& GetRenderSettings() const;

    void FillCatalogue(transport::Catalogue& catalogue);
    renderer::MapRenderer FillRenderSettings(const json::Dict& request_map) const;
private:
    json::Document input_;
    json::Node dummy_ = nullptr;

    const json::Node& GetBaseRequests() const;

    json_reader::Stop FillStop(const json::Dict& request_map) const;
    json_reader::Bus FillBus(const json::Dict& request_map) const;
};