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
    QPointF stationBase(const QString &stationId) const;
    QPointF positionForDevice(const Device &device, int index) const;

    TopologyData m_data;
};
