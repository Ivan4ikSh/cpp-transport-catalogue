#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"
using namespace transport;
void ParseAndPrintStat(const Catalogue& transport_catalogue, std::string_view request, std::ostream& output);

namespace info {
	void Bus(const Catalogue& transport_catalogue, const transport::Bus* bus, std::string value, std::ostream& output);
	void Stop(const Catalogue& transport_catalogue, const transport::Stop* stop, std::string value, std::ostream& output);
}