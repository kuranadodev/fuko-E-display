#include "mainwindow.h"
#include "mock_data.h"

#include <QAction>
#include <QFileDialog>
#include <QGraphicsView>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_scene(new TopologyScene(this)), m_view(new QGraphicsView(m_scene, this)) {
    setWindowTitle("Fuko E-Display (Qt Widgets + SVG)");
    resize(1200, 800);

    m_view->setRenderHint(QPainter::Antialiasing, true);
    m_view->setDragMode(QGraphicsView::ScrollHandDrag);
    setCentralWidget(m_view);

    m_scene->loadTopology(buildMockTopology());

    auto *toolbar = addToolBar("Actions");
    QAction *exportAction = toolbar->addAction("Export SVG");

    connect(exportAction, &QAction::triggered, this, [this]() {
        const QString path = QFileDialog::getSaveFileName(this, "Export SVG", "topology.svg", "SVG Files (*.svg)");
        if (path.isEmpty()) {
            return;
        }

        if (!m_scene->exportToSvg(path)) {
            QMessageBox::warning(this, "Export Failed", "Failed to export SVG.");
            return;
        }
        QMessageBox::information(this, "Export Done", QString("SVG exported to: %1").arg(path));
    });
}
