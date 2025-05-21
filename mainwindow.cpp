#include "mainwindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Create the game view and scene
    view = new QGraphicsView(this);
    scene = new GameScene(this);
    view->setScene(scene);
    
    // Set up the view
    view->setRenderHint(QPainter::Antialiasing);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFixedSize(800, 600);
    
    // Set the view as the central widget
    setCentralWidget(view);
    
    // Set window properties
    setWindowTitle("Simple Square Game");
    setFixedSize(800, 600);
}

MainWindow::~MainWindow()
{
    delete scene;
    delete view;
}
