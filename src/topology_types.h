#pragma once

#include <QColor>
#include <QHash>
#include <QPointF>
#include <QString>
#include <QStringList>
#include <QVector>

enum class DeviceType {
    Busbar,
    Breaker,
    Disconnector,
    GroundSwitch,
    Transformer2W,
    Transformer3W,
    Line,
    Capacitor,
    Reactor,
    Load,
    ExtGrid,
    Junction,
    Terminal
};

enum class DeviceRole {
    Generic,
    Bus,
    BusDisconnector,
    Breaker,
    LineDisconnector,
    Feeder,
    Branch,
    Auxiliary
};

struct Annotation {
    bool enabled{true};
    QString text;
    QString position{"right"};
    QPointF offset{8.0, -8.0};
};

struct Device {
    QString id;
    QString stationId;
    QString voltageLevel;
    DeviceType type;
    QStringList terminalNodeIds;
    Annotation annotation;
    QString bayId;
    DeviceRole role{DeviceRole::Generic};
    int voltageBandOrder{0};
    int bayOrder{-1};
};

struct Node {
    QString id;
};

struct Station {
    QString id;
    QString name;
};

struct VoltageStyleConfig {
    QHash<QString, QColor> levelColors;
    QColor defaultColor{128, 128, 128};

    QColor colorFor(const QString &level) const {
        return levelColors.value(level, defaultColor);
    }
};

struct TopologyData {
    QVector<Station> stations;
    QVector<Node> nodes;
    QVector<Device> devices;
    VoltageStyleConfig style;
};
