#include "mock_data.h"

TopologyData buildMockTopology() {
    TopologyData data;

    data.stations = {
        {"S1", "Fuko Substation A"},
        {"S2", "Fuko Substation B"}
    };

    data.style.levelColors.insert("220kV", QColor(220, 30, 30));
    data.style.levelColors.insert("110kV", QColor(255, 140, 0));
    data.style.levelColors.insert("35kV", QColor(80, 160, 255));

    data.nodes = {
        {"N_A_BUS_1"}, {"N_A_BUS_2"}, {"N_A_LINE_OUT"}, {"N_A_T"},
        {"N_B_BUS_1"}, {"N_B_LINE_IN"}, {"N_B_LOAD"}, {"N_B_CAP"}, {"N_B_REACT"},
        {"N_EXT_1"}
    };

    data.devices = {
        {"BUS_A", "S1", "220kV", DeviceType::Busbar, {"N_A_BUS_1", "N_A_BUS_2"}, {true, "A站母线"}},
        {"BRK_A1", "S1", "220kV", DeviceType::Breaker, {"N_A_BUS_2", "N_A_LINE_OUT"}, {true, "A站断路器"}},
        {"DS_A1", "S1", "220kV", DeviceType::Disconnector, {"N_A_LINE_OUT", "N_A_T"}, {true, "A站刀闸"}},
        {"LINE_AB", "", "220kV", DeviceType::Line, {"N_A_T", "N_B_LINE_IN"}, {true, "A-B联络线"}},
        {"TAP_AB", "", "220kV", DeviceType::Junction, {"N_A_T"}, {false, ""}},
        {"LOAD_TAP", "", "110kV", DeviceType::Line, {"N_A_T", "N_B_LOAD"}, {true, "T接支路"}},

        {"BUS_B", "S2", "110kV", DeviceType::Busbar, {"N_B_BUS_1"}, {true, "B站母线"}},
        {"TR_B1", "S2", "110kV", DeviceType::Transformer2W, {"N_B_LINE_IN", "N_B_BUS_1"}, {true, "B站#1主变"}},
        {"TR_B2", "S2", "35kV", DeviceType::Transformer3W, {"N_B_BUS_1", "N_B_CAP", "N_B_REACT"}, {true, "B站#2三绕组"}},
        {"CAP_B", "S2", "35kV", DeviceType::Capacitor, {"N_B_CAP"}, {true, "电容"}},
        {"RCT_B", "S2", "35kV", DeviceType::Reactor, {"N_B_REACT"}, {true, "电抗"}},
        {"LOAD_B", "S2", "35kV", DeviceType::Load, {"N_B_LOAD"}, {true, "B站负荷"}},
        {"EXT_1", "", "220kV", DeviceType::ExtGrid, {"N_EXT_1", "N_A_BUS_1"}, {true, "外部电网"}}
    };

    return data;
}
