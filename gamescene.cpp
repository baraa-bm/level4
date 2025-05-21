#include "gamescene.h"
#include <QGraphicsView>
#include <QTimer>
#include <QPainter>
#include <QLinearGradient>
#include <QKeyEvent>
#include <QDebug>
#include <QPolygonF>

GameScene::GameScene(QObject *parent) : QGraphicsScene(parent),
    playerSpeed(5.0),
    jumpForce(15.0),
    gravity(0.8),
    verticalVelocity(0),
    isJumping(false),
    moveLeft(false),
    moveRight(false),
    jump(false),
    currentScene(1)
{
    // Create player rectangle with gradient brush
    player = new QGraphicsRectItem(0, 0, 30, 30);
    QLinearGradient playerGradient(0, 0, 30, 30);
    playerGradient.setColorAt(0, QColor(0, 150, 255));
    playerGradient.setColorAt(1, QColor(0, 100, 200));
    player->setBrush(playerGradient);
    player->setPen(Qt::NoPen);
    addItem(player);
    player->setPos(0, 0);  // Start somewhere visible on the scene

    // Initialize platform timer for moving platforms
    platformTimer = new QTimer(this);
    connect(platformTimer, &QTimer::timeout, this, &GameScene::movePlatforms);
    platformTimer->start(16); // ~60 FPS

    // Initialize game update timer
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameScene::update);
    gameTimer->start(16); // ~60 FPS

    // Set scene size
    setSceneRect(0, 0, 800, 600);

    // Create initial scene elements
    createScene(currentScene);
}

void GameScene::createScene(int sceneNumber)
{
    // Clear everything except player
    for (QGraphicsItem* item : items()) {
        if (item != player)
            removeItem(item);
    }

    // Background with gradient
    QGraphicsRectItem* background = new QGraphicsRectItem(0, 0, 800, 600);
    QLinearGradient bgGradient(0, 0, 0, 600);
    bgGradient.setColorAt(0, QColor(30, 30, 60));
    bgGradient.setColorAt(1, QColor(10, 10, 30));
    background->setBrush(bgGradient);
    background->setPen(Qt::NoPen);
    addItem(background);
    background->setZValue(-1);

    // Add stars
    for (int i = 0; i < 100; ++i) {
        int size = 1 + rand() % 3;
        QGraphicsEllipseItem* star = new QGraphicsEllipseItem(0, 0, size, size);
        star->setPos(rand() % 800, rand() % 300);
        star->setBrush(QColor(255, 255, 255, 150 + rand() % 105));
        star->setPen(Qt::NoPen);
        addItem(star);
        star->setZValue(-0.5);
    }

    createPlatforms(sceneNumber);
    createSpikes(sceneNumber);
}

void GameScene::createPlatforms(int sceneNumber)
{
    // Remove existing moving platforms if any
    if (movingPlatform) {
        removeItem(movingPlatform);
        delete movingPlatform;
        movingPlatform = nullptr;
    }
    if (movingPlatform2) {
        removeItem(movingPlatform2);
        delete movingPlatform2;
        movingPlatform2 = nullptr;
    }

    // Ground platform
    QGraphicsRectItem* ground = new QGraphicsRectItem(350, 500, 800, 50);
    QLinearGradient groundGrad(0, 550, 0, 600);
    groundGrad.setColorAt(0, QColor(80, 50, 30));
    groundGrad.setColorAt(1, QColor(50, 30, 20));
    ground->setBrush(groundGrad);
    ground->setPen(Qt::NoPen);
    addItem(ground);

    if (sceneNumber == 1) {
        // Static platforms
        // platform with either coin or enemy
        QGraphicsRectItem* platform4 = new QGraphicsRectItem(500, 200, 225, 65);
        QLinearGradient platform4Gradient(600, 350, 600, 370);
        platform4Gradient.setColorAt(0, QColor(120, 80, 50));
        platform4Gradient.setColorAt(1, QColor(90, 60, 40));
        platform4->setBrush(platform4Gradient);
        platform4->setPen(QPen(QColor(70, 50, 30), 1));
        addItem(platform4);

        // platform with spikes
        QGraphicsRectItem* platform5 = new QGraphicsRectItem(125, 250, 246, 40);
        QLinearGradient platform5Gradient(200, 250, 200, 270);
        platform5Gradient.setColorAt(0, QColor(120, 80, 50));
        platform5Gradient.setColorAt(1, QColor(90, 60, 40));
        platform5->setBrush(platform5Gradient);
        platform5->setPen(QPen(QColor(70, 50, 30), 1));
        addItem(platform5);

        QGraphicsRectItem* platform1 = new QGraphicsRectItem(0, 100, 350, 20);
        QLinearGradient platform1Gradient(100, 400, 100, 420);
        platform1Gradient.setColorAt(0, QColor(120, 80, 50));
        platform1Gradient.setColorAt(1, QColor(90, 60, 40));
        platform1->setBrush(platform1Gradient);
        platform1->setPen(QPen(QColor(70, 50, 30), 1));
        addItem(platform1);

        // bottom platforms with a gap in the middle
        QGraphicsRectItem* platform10 = new QGraphicsRectItem(0, 400, 200, 20);
        QLinearGradient platform10Gradient(100, 400, 100, 420);
        platform10Gradient.setColorAt(0, QColor(120, 80, 50));
        platform10Gradient.setColorAt(1, QColor(90, 60, 40));
        platform10->setBrush(platform10Gradient);
        platform10->setPen(QPen(QColor(70, 50, 30), 1));
        addItem(platform10);

        QGraphicsRectItem* platform2 = new QGraphicsRectItem(300, 400, 560, 20);
        QLinearGradient platform2Gradient(300, 350, 300, 370);
        platform2Gradient.setColorAt(0, QColor(120, 80, 50));
        platform2Gradient.setColorAt(1, QColor(90, 60, 40));
        platform2->setBrush(platform2Gradient);
        platform2->setPen(QPen(QColor(70, 50, 30), 1));
        addItem(platform2);

        // Moving platform 1 (horizontal sliding)
        movingPlatform = new QGraphicsRectItem(350, 100, 80, 20);
        movingPlatform->setBrush(QColor(150, 100, 60));
        movingPlatform->setPen(QPen(QColor(70, 50, 30), 1));
        addItem(movingPlatform);
        movingPlatformDirection = 1;

        // Moving platform 2
        movingPlatform2 = new QGraphicsRectItem(0, 500, 80, 20);
        movingPlatform2->setBrush(QColor(150, 100, 60));
        movingPlatform2->setPen(QPen(QColor(70, 50, 30), 1));
        addItem(movingPlatform2);
        movingPlatform2Direction = 1;
    }
}

void GameScene::createSpikes(int sceneNumber)
{
    switch (sceneNumber) {
    case 1: {
        // the platform full of spikes
        for (int i = 0; i < 12; i++) {
            QGraphicsPolygonItem* spike = new QGraphicsPolygonItem();
            QPolygonF triangle;
            triangle << QPointF(0, 20) << QPointF(10, 0) << QPointF(20, 20);
            spike->setPolygon(triangle);
            spike->setPos(125 + i * 20, 230); // On platform 2
            spike->setBrush(QColor(200, 0, 0));
            spike->setPen(Qt::NoPen);
            addItem(spike);
        }

        // spikes on the edges of the second platform
        for (int i = 0; i < 3; i++) {
            QGraphicsPolygonItem* spike = new QGraphicsPolygonItem();
            QPolygonF triangle;
            triangle << QPointF(0, 20) << QPointF(10, 0) << QPointF(20, 20);
            spike->setPolygon(triangle);
            spike->setPos(675 + i * 20, 180); // On platform 3
            spike->setBrush(QColor(200, 0, 0));
            spike->setPen(Qt::NoPen);
            addItem(spike);
        }

        for (int i = 0; i < 3; i++) {
            QGraphicsPolygonItem* spike = new QGraphicsPolygonItem();
            QPolygonF triangle;
            triangle << QPointF(0, 20) << QPointF(10, 0) << QPointF(20, 20);
            spike->setPolygon(triangle);
            spike->setPos(500 + i * 20, 180); // On platform 3
            spike->setBrush(QColor(200, 0, 0));
            spike->setPen(Qt::NoPen);
            addItem(spike);
        }

        // Ground spikes
        for (int i = 0; i < 3; i++) {
            QGraphicsPolygonItem* spike = new QGraphicsPolygonItem();
            QPolygonF triangle;
            triangle << QPointF(0, 20) << QPointF(10, 0) << QPointF(20, 20);
            spike->setPolygon(triangle);
            spike->setPos(0 + i * 20, 380); // On platform 3
            spike->setBrush(QColor(200, 0, 0));
            spike->setPen(Qt::NoPen);
            addItem(spike);
        }

        for (int i = 0; i < 3; i++) {
            QGraphicsPolygonItem* spike = new QGraphicsPolygonItem();
            QPolygonF triangle;
            triangle << QPointF(0, 20) << QPointF(10, 0) << QPointF(20, 20);
            spike->setPolygon(triangle);
            spike->setPos(350 + i * 20, 380); // On platform 3
            spike->setBrush(QColor(200, 0, 0));
            spike->setPen(Qt::NoPen);
            addItem(spike);
        }

        for (int i = 0; i < 3; i++) {
            QGraphicsPolygonItem* spike = new QGraphicsPolygonItem();
            QPolygonF triangle;
            triangle << QPointF(0, 20) << QPointF(10, 0) << QPointF(20, 20);
            spike->setPolygon(triangle);
            spike->setPos(750 + i * 20, 380); // On platform 3
            spike->setBrush(QColor(200, 0, 0));
            spike->setPen(Qt::NoPen);
            addItem(spike);
        }
        break;
    }
    }
}

GameScene::~GameScene()
{
    // Stop timers
    if (platformTimer) {
        platformTimer->stop();
        delete platformTimer;
    }
    if (gameTimer) {
        gameTimer->stop();
        delete gameTimer;
    }

    // Clear all items
    clear();
}

void GameScene::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return; // avoid multiple repeats

    switch (event->key()) {
    case Qt::Key_Left:
        moveLeft = true;
        break;
    case Qt::Key_Right:
        moveRight = true;
        break;
    case Qt::Key_Space:
        if (!isJumping) {
            verticalVelocity = -jumpForce;
            isJumping = true;
        }
        break;
    }
}

void GameScene::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_Left:
        moveLeft = false;
        break;
    case Qt::Key_Right:
        moveRight = false;
        break;
    }
}

void GameScene::movePlatforms()
{
    // Move movingPlatform left and right between bounds
    if (movingPlatform) {
        qreal x = movingPlatform->x();
        if (x > 350) movingPlatformDirection = -1;
        else if (x < 0) movingPlatformDirection = 1;
        movingPlatform->setX(x + movingPlatformDirection * 2);
    }

    // Move movingPlatform2 up and down between bounds
    if (movingPlatform2) {
        qreal x = movingPlatform2->x();
        if (x > 250) movingPlatform2Direction = -1;
        else if (x < 0) movingPlatform2Direction = 1;
        movingPlatform2->setX(x + movingPlatform2Direction * 1.5);
    }
}

void GameScene::update() {
    // Movement
    if (moveLeft)
        player->moveBy(-playerSpeed, 0);
    if (moveRight)
        player->moveBy(playerSpeed, 0);

    // Check collision with platforms and spikes first
    bool onPlatform = false;
    QRectF playerRect = player->mapRectToScene(player->boundingRect());
    playerRect.setHeight(playerRect.height() + 1); // Add 1 pixel to check below player

    for (QGraphicsItem* item : items()) {
        if (item == player || !item->isVisible()) continue;

        QRectF itemRect = item->mapRectToScene(item->boundingRect());
        
        // Check for spike collision
        if (item->type() == QGraphicsPolygonItem::Type) {
            if (playerRect.intersects(itemRect)) {
                // Reset player position and velocity
                player->setPos(0, 0);
                verticalVelocity = 0;
                isJumping = false;
                return;
            }
        }
        
        // Check if player is above a platform and falling
        if (verticalVelocity >= 0 && // Only check when falling
            playerRect.bottom() >= itemRect.top() &&
            playerRect.bottom() <= itemRect.top() + 6 && // Small threshold for landing
            playerRect.right() > itemRect.left() &&
            playerRect.left() < itemRect.right()) {
            
            player->setY(itemRect.top() - player->rect().height());
            verticalVelocity = 0;
            isJumping = false;
            onPlatform = true;
            break;
        }
    }

    // Apply gravity and vertical movement if not on platform
    if (!onPlatform) {
        verticalVelocity += gravity;
        player->moveBy(0, verticalVelocity);
    }

    // Keep player within scene bounds
    if (player->x() < 0)
        player->setX(0);
    if (player->x() > sceneRect().width() - player->rect().width())
        player->setX(sceneRect().width() - player->rect().width());
    if (player->y() < 0) {
        player->setY(0);
        verticalVelocity = 0;
    }
    if (player->y() > sceneRect().height()) {
        // Reset player when falling below the scene
        player->setPos(0, 0);
        verticalVelocity = 0;
        isJumping = false;
    }
}
