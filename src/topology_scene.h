#pragma once

#include "topology_types.h"

#include <QGraphicsScene>

class TopologyScene : public QGraphicsScene {
public:
    explicit TopologyScene(QObject *parent = nullptr);

    void loadTopology(const TopologyData &data);
    bool exportToSvg(const QString &filePath);

private:
    void drawDevice(const Device &device, const QPointF &pos, const QColor &color);
    void drawConnectivityLayer(const QHash<QString, QPointF> &devicePositions);
    QPointF computeNodeAnchor(const QString &nodeId, const QHash<QString, QPointF> &devicePositions) const;
    QPointF stationBase(const QString &stationId) const;
    QPointF positionForDevice(const Device &device, int bayOrder, int externalIndex) const;
    QPointF terminalAnchor(const Device &device, const QPointF &center, int terminalIndex) const;
    void drawLegend();

    TopologyData m_data;
};
