#include "topology_scene.h"

#include <QFont>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QHash>
#include <QPainter>
#include <QSet>
#include <QSvgGenerator>

TopologyScene::TopologyScene(QObject *parent)
    : QGraphicsScene(parent) {}

void TopologyScene::loadTopology(const TopologyData &data) {
    clear();
    m_data = data;

    QHash<QString, QPointF> devicePositions;
    QHash<QString, QRectF> stationBounds;
    QHash<QString, int> stationCounters;
    int externalIndex = 0;

    for (const auto &device : m_data.devices) {
        const int stationIndex = stationCounters.value(device.stationId, 0);
        QPointF pos = positionForDevice(device, stationIndex, externalIndex);
        if (device.stationId.isEmpty()) {
            ++externalIndex;
        } else {
            stationCounters[device.stationId] = stationIndex + 1;
        }

        devicePositions.insert(device.id, pos);
        const QColor color = m_data.style.colorFor(device.voltageLevel);
        drawDevice(device, pos, color);

        if (!device.stationId.isEmpty()) {
            QRectF &b = stationBounds[device.stationId];
            QRectF current(pos.x() - 40, pos.y() - 20, 80, 40);
            b = b.isNull() ? current : b.united(current);
        }
    }

    drawConnectivityLayer(devicePositions);

    for (const auto &station : m_data.stations) {
        const QRectF bbox = stationBounds.value(station.id).adjusted(-40, -40, 40, 40);
        addRect(bbox, QPen(QColor(100, 100, 100), 1.5, Qt::DashLine));
        auto *label = addText(station.name);
        label->setDefaultTextColor(QColor(60, 60, 60));
        label->setPos(bbox.left() + 4, bbox.top() - 28);
    }

    setSceneRect(itemsBoundingRect().adjusted(-20, -20, 20, 20));
}

bool TopologyScene::exportToSvg(const QString &filePath) {
    if (filePath.isEmpty()) {
        return false;
    }

    QSvgGenerator generator;
    generator.setFileName(filePath);
    generator.setSize(sceneRect().size().toSize());
    generator.setViewBox(sceneRect());
    generator.setTitle("Fuko E-Display Topology Export");

    QPainter painter;
    if (!painter.begin(&generator)) {
        return false;
    }
    render(&painter);
    painter.end();
    return true;
}

QPointF TopologyScene::stationBase(const QString &stationId) const {
    if (stationId == "S1") return QPointF(100, 120);
    if (stationId == "S2") return QPointF(760, 280);
    return QPointF(560, 560);
}

QPointF TopologyScene::positionForDevice(const Device &device, int stationIndex, int externalIndex) const {
    const QPointF base = stationBase(device.stationId);

    if (device.stationId.isEmpty()) {
        return QPointF(base.x() + externalIndex * 140.0, base.y() + ((externalIndex % 2) * 90.0));
    }

    switch (device.type) {
    case DeviceType::Busbar:
        return QPointF(base.x() + 120.0, base.y());
    case DeviceType::Breaker:
    case DeviceType::Disconnector:
    case DeviceType::GroundSwitch:
        return QPointF(base.x() + 260.0 + stationIndex * 80.0, base.y());
    case DeviceType::Transformer2W:
    case DeviceType::Transformer3W:
        return QPointF(base.x() + 260.0 + stationIndex * 90.0, base.y() + 120.0);
    case DeviceType::Capacitor:
    case DeviceType::Reactor:
    case DeviceType::Load:
        return QPointF(base.x() + 180.0 + stationIndex * 80.0, base.y() + 220.0);
    default:
        return QPointF(base.x() + 180.0 + (stationIndex % 4) * 100.0, base.y() + 80.0 + (stationIndex / 4) * 90.0);
    }
}

QPointF TopologyScene::computeNodeAnchor(const QString &nodeId, const QHash<QString, QPointF> &devicePositions) const {
    QPointF sum(0, 0);
    int count = 0;
    for (const auto &device : m_data.devices) {
        if (!device.terminalNodeIds.contains(nodeId)) {
            continue;
        }
        if (const auto it = devicePositions.find(device.id); it != devicePositions.end()) {
            sum += it.value();
            ++count;
        }
    }

    if (count == 0) {
        return QPointF();
    }
    return QPointF(sum.x() / count, sum.y() / count);
}

void TopologyScene::drawConnectivityLayer(const QHash<QString, QPointF> &devicePositions) {
    QHash<QString, QPointF> nodeAnchors;
    for (const auto &node : m_data.nodes) {
        nodeAnchors.insert(node.id, computeNodeAnchor(node.id, devicePositions));
    }

    const QPen linkPen(QColor(90, 90, 90), 1.5);
    for (const auto &device : m_data.devices) {
        const QPointF devicePos = devicePositions.value(device.id);
        for (const auto &nodeId : device.terminalNodeIds) {
            if (!nodeAnchors.contains(nodeId)) {
                continue;
            }
            const QPointF nodePos = nodeAnchors.value(nodeId);
            if (nodePos.isNull()) {
                continue;
            }
            addLine(QLineF(devicePos, nodePos), linkPen)->setZValue(-2);
        }
    }

    for (auto it = nodeAnchors.constBegin(); it != nodeAnchors.constEnd(); ++it) {
        const QPointF nodePos = it.value();
        if (nodePos.isNull()) {
            continue;
        }
        addEllipse(nodePos.x() - 3, nodePos.y() - 3, 6, 6, QPen(Qt::NoPen), QBrush(QColor(120, 120, 120)))->setZValue(-1);
    }
}

void TopologyScene::drawDevice(const Device &device, const QPointF &pos, const QColor &color) {
    const QPen pen(color, 2);

    switch (device.type) {
    case DeviceType::Busbar:
        addLine(pos.x() - 45, pos.y(), pos.x() + 45, pos.y(), QPen(color, 6));
        break;
    case DeviceType::Breaker:
        addRect(pos.x() - 18, pos.y() - 12, 36, 24, pen);
        break;
    case DeviceType::Disconnector:
        addLine(pos.x() - 20, pos.y(), pos.x() + 20, pos.y(), pen);
        addLine(pos.x(), pos.y(), pos.x() + 15, pos.y() - 15, pen);
        break;
    case DeviceType::GroundSwitch:
        addLine(pos.x(), pos.y() - 15, pos.x(), pos.y() + 5, pen);
        addLine(pos.x() - 10, pos.y() + 5, pos.x() + 10, pos.y() + 5, pen);
        break;
    case DeviceType::Transformer2W:
        addEllipse(pos.x() - 24, pos.y() - 12, 24, 24, pen);
        addEllipse(pos.x(), pos.y() - 12, 24, 24, pen);
        break;
    case DeviceType::Transformer3W:
        addEllipse(pos.x() - 14, pos.y() - 18, 24, 24, pen);
        addEllipse(pos.x() + 10, pos.y() - 18, 24, 24, pen);
        addEllipse(pos.x() - 2, pos.y() + 2, 24, 24, pen);
        break;
    case DeviceType::Line:
        addLine(pos.x() - 40, pos.y(), pos.x() + 40, pos.y(), pen);
        break;
    case DeviceType::Capacitor:
        addLine(pos.x() - 10, pos.y() - 14, pos.x() - 10, pos.y() + 14, pen);
        addLine(pos.x() + 10, pos.y() - 14, pos.x() + 10, pos.y() + 14, pen);
        break;
    case DeviceType::Reactor:
        addEllipse(pos.x() - 22, pos.y() - 8, 14, 14, pen);
        addEllipse(pos.x() - 8, pos.y() - 8, 14, 14, pen);
        addEllipse(pos.x() + 6, pos.y() - 8, 14, 14, pen);
        break;
    case DeviceType::Load:
        addRect(pos.x() - 16, pos.y() - 16, 32, 32, pen);
        addLine(pos.x() - 16, pos.y() - 16, pos.x() + 16, pos.y() + 16, pen);
        break;
    case DeviceType::ExtGrid:
        addPolygon(QPolygonF{QPointF(pos.x(), pos.y() - 18), QPointF(pos.x() - 16, pos.y() + 14), QPointF(pos.x() + 16, pos.y() + 14)}, pen);
        break;
    case DeviceType::Junction:
    case DeviceType::Terminal:
        addEllipse(pos.x() - 4, pos.y() - 4, 8, 8, QPen(color), QBrush(color));
        break;
    }

    if (device.annotation.enabled && !device.annotation.text.isEmpty()) {
        auto *text = addText(device.annotation.text, QFont("Sans", 9));
        text->setDefaultTextColor(QColor(30, 30, 30));
        text->setPos(pos + device.annotation.offset);
    }
}
