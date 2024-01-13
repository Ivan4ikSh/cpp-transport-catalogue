#include <iostream>
#include <chrono>
#include <fstream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;
using namespace transport;
using namespace input;

int main() {
    Catalogue catalogue;
    fstream file("test.txt");
    if (!file) cout << "error" << endl;

    int base_request_count;
    file >> base_request_count >> ws;
    {
        Reader reader;
        for (int i = 0; i < base_request_count; ++i) {
            string line;
            getline(file, line);
            reader.ParseLine(line);
        }
        reader.ApplyCommands(catalogue);
    }

    int stat_request_count;
    file >> stat_request_count >> ws;
    for (int i = 0; i < stat_request_count; ++i) {
        string line;
        getline(file, line);
        ParseAndPrintStat(catalogue, line, cout);
    }
}