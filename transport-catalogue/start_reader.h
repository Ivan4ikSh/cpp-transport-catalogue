#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"
using namespace transport;

void ParseAndPrintStat(const Catalogue& transport_catalogue, std::string_view request, std::ostream& output);