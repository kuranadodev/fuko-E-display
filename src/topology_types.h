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
