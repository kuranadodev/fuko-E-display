#pragma once

#include "topology_scene.h"

#include <QMainWindow>

class QGraphicsView;

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    TopologyScene *m_scene{nullptr};
    QGraphicsView *m_view{nullptr};
};
