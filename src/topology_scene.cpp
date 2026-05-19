#include "topology_scene.h"

#include <QFont>
#include <QHash>
#include <QPainter>
#include <QSvgGenerator>

TopologyScene::TopologyScene(QObject *parent)
    : QGraphicsScene(parent) {}

void TopologyScene::loadTopology(const TopologyData &data) {
    clear();
    m_data = data;

    QHash<QString, QPointF> devicePositions;
    QHash<QString, QRectF> stationBounds;
    QHash<QString, int> bayOrderByKey;
    QHash<QString, int> stationNextBayOrder;
    int externalIndex = 0;

    for (const auto &device : m_data.devices) {
        if (device.stationId.isEmpty()) {
            const QPointF pos = positionForDevice(device, 0, externalIndex++);
            devicePositions.insert(device.id, pos);
            continue;
        }

        QString bayKey;
        if (!device.bayId.isEmpty()) {
            bayKey = device.stationId + "|EXPLICIT|" + device.bayId;
        } else {
            QString primaryNode = device.terminalNodeIds.isEmpty() ? "NO_NODE" : device.terminalNodeIds.first();
            bayKey = device.stationId + "|AUTO|" + device.voltageLevel + "|" + QString::number(device.voltageBandOrder)
                + "|" + QString::number(static_cast<int>(device.role)) + "|" + primaryNode;
        }

        int bayOrder = device.bayOrder;
        if (bayOrder < 0) {
            if (!bayOrderByKey.contains(bayKey)) {
                bayOrderByKey.insert(bayKey, stationNextBayOrder.value(device.stationId, 0));
                stationNextBayOrder[device.stationId] = stationNextBayOrder.value(device.stationId, 0) + 1;
            }
            bayOrder = bayOrderByKey.value(bayKey);
        }

        const QPointF pos = positionForDevice(device, bayOrder, externalIndex);
        devicePositions.insert(device.id, pos);

        QRectF &b = stationBounds[device.stationId];
        QRectF current(pos.x() - 50, pos.y() - 26, 100, 52);
        b = b.isNull() ? current : b.united(current);

    }

    drawConnectivityLayer(devicePositions);

    for (const auto &device : m_data.devices) {
        const QPointF pos = devicePositions.value(device.id);
        const QColor color = m_data.style.colorFor(device.voltageLevel);
        drawDevice(device, pos, color);
    }

    for (const auto &station : m_data.stations) {
        QRectF bbox = stationBounds.value(station.id);
        if (bbox.isNull()) {
            continue;
        }
        bbox = bbox.adjusted(-60, -70, 60, 50);
        addRect(bbox, QPen(QColor(100, 100, 100), 1.5, Qt::DashLine));
        auto *label = addText(station.name, QFont("Sans", 11, QFont::Bold));
        label->setDefaultTextColor(QColor(45, 45, 45));
        label->setPos(bbox.left() + 8, bbox.top() - 30);
    }

    drawLegend();
    setSceneRect(itemsBoundingRect().adjusted(-24, -24, 24, 24));
}

bool TopologyScene::exportToSvg(const QString &filePath) {
    if (filePath.isEmpty()) return false;
    QSvgGenerator generator;
    generator.setFileName(filePath);
    generator.setSize(sceneRect().size().toSize());
    generator.setViewBox(sceneRect());
    generator.setTitle("Fuko E-Display Topology Export");
    QPainter painter;
    if (!painter.begin(&generator)) return false;
    render(&painter);
    painter.end();
    return true;
}

QPointF TopologyScene::stationBase(const QString &stationId) const {
    if (stationId == "S1") return QPointF(120, 110);
    if (stationId == "S2") return QPointF(780, 110);
    return QPointF(500, 520);
}

QPointF TopologyScene::positionForDevice(const Device &device, int bayOrder, int externalIndex) const {
    if (device.stationId.isEmpty()) {
        return QPointF(430 + externalIndex * 180.0, 470 + ((externalIndex % 2) * 100.0));
    }

    const QPointF base = stationBase(device.stationId);
    const qreal bayX = base.x() + 100.0 + bayOrder * 130.0;
    const qreal bandY = base.y() + device.voltageBandOrder * 175.0;

    if (device.role == DeviceRole::Bus || device.type == DeviceType::Busbar) return QPointF(base.x() + 260.0, bandY + 40.0);
    if (device.role == DeviceRole::BusDisconnector) return QPointF(bayX, bandY + 85.0);
    if (device.role == DeviceRole::Breaker) return QPointF(bayX, bandY + 130.0);
    if (device.role == DeviceRole::LineDisconnector) return QPointF(bayX, bandY + 175.0);

    switch (device.type) {
    case DeviceType::Transformer2W:
    case DeviceType::Transformer3W:
        return QPointF(bayX, bandY + 220.0);
    case DeviceType::Capacitor:
    case DeviceType::Reactor:
    case DeviceType::Load:
        return QPointF(bayX, bandY + 260.0);
    default:
        return QPointF(bayX, bandY + 215.0);
    }
}

QPointF TopologyScene::terminalAnchor(const Device &device, const QPointF &center, int terminalIndex) const {
    const bool leftSide = (terminalIndex % 2 == 0);
    switch (device.type) {
    case DeviceType::Busbar: return QPointF(center.x() + (leftSide ? -35.0 : 35.0), center.y());
    case DeviceType::Breaker:
    case DeviceType::Disconnector:
    case DeviceType::Line: return QPointF(center.x(), center.y() + (leftSide ? -12.0 : 12.0));
    case DeviceType::Transformer2W:
    case DeviceType::Transformer3W: return QPointF(center.x() + (leftSide ? -18.0 : 18.0), center.y() + (terminalIndex > 1 ? 10.0 : -10.0));
    default: return QPointF(center.x() + (leftSide ? -14.0 : 14.0), center.y());
    }
}

QPointF TopologyScene::computeNodeAnchor(const QString &nodeId, const QHash<QString, QPointF> &devicePositions) const {
    QPointF sum(0, 0);
    int count = 0;
    for (const auto &device : m_data.devices) {
        const int terminalIdx = device.terminalNodeIds.indexOf(nodeId);
        if (terminalIdx < 0) continue;
        if (const auto it = devicePositions.find(device.id); it != devicePositions.end()) {
            sum += terminalAnchor(device, it.value(), terminalIdx);
            ++count;
        }
    }
    if (count == 0) return QPointF();
    return QPointF(sum.x() / count, sum.y() / count);
}

void TopologyScene::drawConnectivityLayer(const QHash<QString, QPointF> &devicePositions) {
    QHash<QString, QPointF> nodeAnchors;
    for (const auto &node : m_data.nodes) nodeAnchors.insert(node.id, computeNodeAnchor(node.id, devicePositions));

    const QPen linkPen(QColor(95, 95, 95), 1.4);
    for (const auto &device : m_data.devices) {
        const QPointF devicePos = devicePositions.value(device.id);
        for (int i = 0; i < device.terminalNodeIds.size(); ++i) {
            const QString nodeId = device.terminalNodeIds.at(i);
            if (!nodeAnchors.contains(nodeId)) continue;
            const QPointF nodePos = nodeAnchors.value(nodeId);
            if (nodePos.isNull()) continue;
            const QPointF start = terminalAnchor(device, devicePos, i);
            const qreal midX = (start.x() + nodePos.x()) / 2.0;
            addLine(QLineF(start, QPointF(midX, start.y())), linkPen)->setZValue(-2);
            addLine(QLineF(QPointF(midX, start.y()), QPointF(midX, nodePos.y())), linkPen)->setZValue(-2);
            addLine(QLineF(QPointF(midX, nodePos.y()), nodePos), linkPen)->setZValue(-2);
        }
    }

    for (auto it = nodeAnchors.constBegin(); it != nodeAnchors.constEnd(); ++it) {
        const QPointF nodePos = it.value();
        if (nodePos.isNull()) continue;
        addEllipse(nodePos.x() - 2.6, nodePos.y() - 2.6, 5.2, 5.2, QPen(Qt::NoPen), QBrush(QColor(120, 120, 120)))->setZValue(-1);
    }
}

void TopologyScene::drawDevice(const Device &device, const QPointF &pos, const QColor &color) { const QPen pen(color, 2);
    switch (device.type) {
    case DeviceType::Busbar: addLine(pos.x() - 60, pos.y(), pos.x() + 60, pos.y(), QPen(color, 6)); break;
    case DeviceType::Breaker: addRect(pos.x() - 14, pos.y() - 12, 28, 24, pen); break;
    case DeviceType::Disconnector: addLine(pos.x() - 16, pos.y(), pos.x() + 16, pos.y(), pen); addLine(pos.x(), pos.y(), pos.x() + 12, pos.y() - 12, pen); break;
    case DeviceType::GroundSwitch: addLine(pos.x(), pos.y() - 15, pos.x(), pos.y() + 5, pen); addLine(pos.x() - 10, pos.y() + 5, pos.x() + 10, pos.y() + 5, pen); break;
    case DeviceType::Transformer2W: addEllipse(pos.x() - 24, pos.y() - 12, 24, 24, pen); addEllipse(pos.x(), pos.y() - 12, 24, 24, pen); break;
    case DeviceType::Transformer3W: addEllipse(pos.x() - 14, pos.y() - 18, 24, 24, pen); addEllipse(pos.x() + 10, pos.y() - 18, 24, 24, pen); addEllipse(pos.x() - 2, pos.y() + 2, 24, 24, pen); break;
    case DeviceType::Line: addLine(pos.x() - 38, pos.y(), pos.x() + 38, pos.y(), pen); break;
    case DeviceType::Capacitor: addLine(pos.x() - 10, pos.y() - 14, pos.x() - 10, pos.y() + 14, pen); addLine(pos.x() + 10, pos.y() - 14, pos.x() + 10, pos.y() + 14, pen); break;
    case DeviceType::Reactor: addEllipse(pos.x() - 22, pos.y() - 8, 14, 14, pen); addEllipse(pos.x() - 8, pos.y() - 8, 14, 14, pen); addEllipse(pos.x() + 6, pos.y() - 8, 14, 14, pen); break;
    case DeviceType::Load: addRect(pos.x() - 16, pos.y() - 16, 32, 32, pen); addLine(pos.x() - 16, pos.y() - 16, pos.x() + 16, pos.y() + 16, pen); break;
    case DeviceType::ExtGrid: addPolygon(QPolygonF{QPointF(pos.x(), pos.y() - 18), QPointF(pos.x() - 16, pos.y() + 14), QPointF(pos.x() + 16, pos.y() + 14)}, pen); break;
    case DeviceType::Junction:
    case DeviceType::Terminal: addEllipse(pos.x() - 4, pos.y() - 4, 8, 8, QPen(color), QBrush(color)); break;
    }
    if (device.annotation.enabled && !device.annotation.text.isEmpty()) {
        auto *text = addText(device.annotation.text, QFont("Sans", device.type == DeviceType::Busbar ? 10 : 9, device.type == DeviceType::Busbar ? QFont::Bold : QFont::Normal));
        text->setDefaultTextColor(QColor(28, 28, 28));
        text->setPos(pos + device.annotation.offset);
    }
}

void TopologyScene::drawLegend() {
    const QRectF legendRect(20, 20, 165, 95);
    addRect(legendRect, QPen(QColor(130, 130, 130)), QBrush(QColor(255, 255, 255, 230)));
    auto *title = addText("电压等级", QFont("Sans", 9, QFont::Bold));
    title->setPos(legendRect.left() + 8, legendRect.top() + 4);
    QStringList levels{"220kV", "110kV", "35kV"};
    for (int i = 0; i < levels.size(); ++i) {
        const qreal y = legendRect.top() + 28 + i * 20;
        const QColor color = m_data.style.colorFor(levels.at(i));
        addLine(legendRect.left() + 10, y, legendRect.left() + 50, y, QPen(color, 4));
        auto *label = addText(levels.at(i), QFont("Sans", 8));
        label->setPos(legendRect.left() + 58, y - 10);
    }
}
