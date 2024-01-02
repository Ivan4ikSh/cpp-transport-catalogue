#include "start_reader.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>
using namespace transport;

void ParseAndPrintStat(const Catalogue& transport_catalogue, std::string_view request, std::ostream& output) {
    auto pos = request.find(' ');
    
    std::string command = move(std::string(request.substr(0, pos)));
    std::string value = move(std::string(request.substr(pos + 1)));

    transport_catalogue.OutInfo(command, value, output);
}