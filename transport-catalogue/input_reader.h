#pragma once
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

using namespace transport;

namespace input {
    struct Description {
        // ����������, ������ �� ������� (���� command ��������)
        explicit operator bool() const {
            return !command.empty();
        }

        bool operator!() const {
            return !operator bool();
        }

        std::string command;      // �������� �������
        std::string id;           // id �������� ��� ���������
        std::string description;  // ��������� �������
    };

    class Reader {
    public:
        /**
         * ������ ������ � ��������� Description � ��������� ��������� � commands_
         */
        void ParseLine(std::string_view line);

        /**
         * ��������� ������� ������������ ����������, ��������� ������� �� commands_
         */
        void ApplyCommands(Catalogue& catalogue) const;

    private:
        std::vector<Description> commands_;
    };
}