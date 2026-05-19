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
        {"BUS_A", "S1", "220kV", DeviceType::Busbar, {"N_A_BUS_1", "N_A_BUS_2"}, {true, "A站 220kV I母"}, "A-BUS", DeviceRole::Bus, 0},
        {"DS_A_BUS", "S1", "220kV", DeviceType::Disconnector, {"N_A_BUS_2", "N_A_LINE_OUT"}, {true, "QS-201"}, "A-LINE", DeviceRole::BusDisconnector, 0},
        {"BRK_A1", "S1", "220kV", DeviceType::Breaker, {"N_A_LINE_OUT", "N_A_T"}, {true, "QF-201"}, "A-LINE", DeviceRole::Breaker, 0},
        {"DS_A_LINE", "S1", "220kV", DeviceType::Disconnector, {"N_A_T", "N_A_LINE_OUT"}, {true, "QS-202"}, "A-LINE", DeviceRole::LineDisconnector, 0},

        {"BUS_B", "S2", "110kV", DeviceType::Busbar, {"N_B_BUS_1"}, {true, "B站 110kV I母"}, "B-BUS", DeviceRole::Bus, 0},
        {"TR_B1", "S2", "110kV", DeviceType::Transformer2W, {"N_B_LINE_IN", "N_B_BUS_1"}, {true, "1#主变"}, "B-MAINTR", DeviceRole::Feeder, 0},
        {"TR_B2", "S2", "35kV", DeviceType::Transformer3W, {"N_B_BUS_1", "N_B_CAP", "N_B_REACT"}, {true, "2#三绕组"}, "B-TR3", DeviceRole::Feeder, 1},
        {"CAP_B", "S2", "35kV", DeviceType::Capacitor, {"N_B_CAP"}, {true, "C-301 电容"}, "B-CAP", DeviceRole::Auxiliary, 1},
        {"RCT_B", "S2", "35kV", DeviceType::Reactor, {"N_B_REACT"}, {true, "L-301 电抗"}, "B-REACT", DeviceRole::Auxiliary, 1},
        {"LOAD_B", "S2", "35kV", DeviceType::Load, {"N_B_LOAD"}, {true, "F-301 负荷"}, "B-LOAD", DeviceRole::Branch, 1},

        {"LINE_AB", "", "220kV", DeviceType::Line, {"N_A_T", "N_B_LINE_IN"}, {true, "220kV A-B联络线"}, "X-LINE", DeviceRole::Feeder, 0},
        {"LOAD_TAP", "", "110kV", DeviceType::Line, {"N_A_T", "N_B_LOAD"}, {true, "T接支路"}, "X-TAP", DeviceRole::Branch, 1},
        {"EXT_1", "", "220kV", DeviceType::ExtGrid, {"N_EXT_1", "N_A_BUS_1"}, {true, "外部电网"}, "X-GRID", DeviceRole::Feeder, 0}
    };

    return data;
}
