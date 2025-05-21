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
        QGraphicsRectItem* platform4 = new QGraphicsRectItem(500, 200, 225, 65);
        QLinearGradient platform4Gradient(600, 350, 600, 370);
        platform4Gradient.setColorAt(0, QColor(120, 80, 50));
        platform4Gradient.setColorAt(1, QColor(90, 60, 40));
        platform4->setBrush(platform4Gradient);
        platform4->setPen(QPen(QColor(70, 50, 30), 1));
        addItem(platform4);

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
            spike->setPos(125 + i * 20, 230);
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
            spike->setPos(675 + i * 20, 180);
            spike->setBrush(QColor(200, 0, 0));
            spike->setPen(Qt::NoPen);
            addItem(spike);
        }

        for (int i = 0; i < 3; i++) {
            QGraphicsPolygonItem* spike = new QGraphicsPolygonItem();
            QPolygonF triangle;
            triangle << QPointF(0, 20) << QPointF(10, 0) << QPointF(20, 20);
            spike->setPolygon(triangle);
            spike->setPos(500 + i * 20, 180);
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
            spike->setPos(0 + i * 20, 380);
            spike->setBrush(QColor(200, 0, 0));
            spike->setPen(Qt::NoPen);
            addItem(spike);
        }

        for (int i = 0; i < 3; i++) {
            QGraphicsPolygonItem* spike = new QGraphicsPolygonItem();
            QPolygonF triangle;
            triangle << QPointF(0, 20) << QPointF(10, 0) << QPointF(20, 20);
            spike->setPolygon(triangle);
            spike->setPos(350 + i * 20, 380);
            spike->setBrush(QColor(200, 0, 0));
            spike->setPen(Qt::NoPen);
            addItem(spike);
        }

        for (int i = 0; i < 3; i++) {
            QGraphicsPolygonItem* spike = new QGraphicsPolygonItem();
            QPolygonF triangle;
            triangle << QPointF(0, 20) << QPointF(10, 0) << QPointF(20, 20);
            spike->setPolygon(triangle);
            spike->setPos(750 + i * 20, 380);
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
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_Left:
        moveLeft = true;
        break;
    case Qt::Key_Right:
        moveRight = true;
        break;
    case Qt::Key_Space:
        if (!isJumping && player->y() >= 0) {
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

void GameScene::resetPlayer() {
    // Reset player to starting position
    player->setPos(0, 0);

    // Reset movement variables
    verticalVelocity = 0;
    isJumping = false;
    moveLeft = false;
    moveRight = false;

    qDebug() << "Player died and respawned!";
}

void GameScene::update() {
    // Horizontal movement with boundary checks
    if (moveLeft) {
        player->setX(qMax(0.0, player->x() - playerSpeed));
    }
    if (moveRight) {
        player->setX(qMin(sceneRect().width() - player->rect().width(), player->x() + playerSpeed));
    }

    // Apply gravity
    verticalVelocity += gravity;

    // Proposed new Y position
    qreal newY = player->y() + verticalVelocity;

    // Prevent player from going above the top of the screen
    if (newY < 0) {
        newY = 0;
        verticalVelocity = 0;
    }

    QRectF playerRect = player->mapRectToScene(player->boundingRect());


    qreal finalY = newY;

    // Check collisions only if falling or moving vertically
    if (verticalVelocity >= 0) {
        for (QGraphicsItem* item : items()) {
            if (item == player || !item->isVisible()) continue;

            // Check for platform collisions
            QGraphicsRectItem* platform = dynamic_cast<QGraphicsRectItem*>(item);
            if (platform) {
                QRectF platformRect = platform->mapRectToScene(platform->boundingRect());

                bool horizontalOverlap = (playerRect.right() > platformRect.left()) &&
                                         (playerRect.left() < platformRect.right());

                if (horizontalOverlap) {
                    qreal playerBottom = playerRect.bottom();
                    qreal playerTop = playerRect.top();
                    qreal platformTop = platformRect.top();
                    qreal platformBottom = platformRect.bottom();

                    if (playerBottom <= platformTop && (playerBottom + verticalVelocity) >= platformTop) {

                        // landed = true;
                        finalY = platformTop - player->boundingRect().height();
                        verticalVelocity = 0;
                        isJumping = false;

                        // platform->setBrush(QBrush(Qt::red));

                        break;
                    }

                    // if (playerTop <= platformBottom && playerTop + verticalVelocity >= platformBottom) {
                    //     if (verticalVelocity > 0) { // Le joueur monte (heurt depuis le bas)
                    //         qDebug() << "Collision avec le bas de la plateforme !";
                    //         qDebug() << "playerTop: " << playerTop << "player bottom : " << playerBottom;
                    //         qDebug() << "platformBottom: " << platformBottom;

                    //         // Arrêter le mouvement vers le haut
                    //         verticalVelocity = 0;

                    //         // Repositionner le joueur juste sous la plateforme
                    //         finalY = platformBottom;

                    //         // Optionnel : changer la couleur de la plateforme
                    //         platform->setBrush(Qt::blue); // Différente couleur pour heurt par le bas

                    //         // Vous pourriez aussi ajouter un effet ou un son ici
                    //     }
                    // }
                }
            }

            // Check for spike collisions
            if (item->type() == QGraphicsPolygonItem::Type) {
                QRectF spikeRect = item->mapRectToScene(item->boundingRect());
                if (playerRect.intersects(spikeRect)) {
                    resetPlayer();
                    return;
                }
            }
        }
    }

    // Check if player fell off the bottom of the screen
    if (newY > sceneRect().height()) {
        resetPlayer();
        return;
    }

    // Update player vertical position
    player->setY(finalY);
}


// void GameScene::update() {
//     // Mouvement horizontal
//     if (moveLeft) {
//         player->setX(qMax(0.0, player->x() - playerSpeed));
//     }
//     if (moveRight) {
//         player->setX(qMin(sceneRect().width() - player->rect().width(), player->x() + playerSpeed));
//     }

//     // Application de la gravité
//     verticalVelocity = qMin(verticalVelocity + gravity, 20.0);
//     qreal newY = player->y() + verticalVelocity;
//     QRectF playerRect = player->mapRectToScene(player->boundingRect());
//     qreal finalY = newY;

//     // Constantes
//     const qreal LANDING_TOLERANCE = 1.0;  // Tolérance pour l'atterrissage
//     const qreal SIDE_TOLERANCE = 0.5;     // Tolérance pour les côtés
//     const qreal RESTITUTION = 0.4;

//     bool onGround = false;

//     // Détection des collisions
//     for (QGraphicsItem* item : items()) {
//         if (item == player || !item->isVisible()) continue;

//         // Collision avec plateforme
//         if (QGraphicsRectItem* platform = dynamic_cast<QGraphicsRectItem*>(item)) {
//             QRectF platformRect = platform->mapRectToScene(platform->boundingRect());

//             // Vérification du chevauchement
//             if (playerRect.intersects(platformRect)) {
//                 qreal playerBottom = playerRect.bottom();
//                 qreal platformTop = platformRect.top();

//                 // Collision par le haut (atterrissage)
//                 if (verticalVelocity >= 0 &&
//                     playerBottom <= platformTop + LANDING_TOLERANCE) {

//                     finalY = platformTop - player->boundingRect().height();
//                     verticalVelocity = 0;
//                     isJumping = false;
//                     onGround = true;
//                     platform->setBrush(Qt::red);
//                     break;
//                 }

//                 // Collision par le bas
//                 if (verticalVelocity < 0 &&
//                     player->y() <= platformRect.bottom()) {
//                     verticalVelocity = -verticalVelocity * RESTITUTION;
//                     platform->setBrush(Qt::blue);
//                 }

//                 // Collisions latérales
//                 if (!onGround) {
//                     // Gauche
//                     if (playerRect.right() > platformRect.left() &&
//                         playerRect.left() < platformRect.left()) {
//                         player->setX(platformRect.left() - player->boundingRect().width());
//                     }
//                     // Droite
//                     else if (playerRect.left() < platformRect.right() &&
//                              playerRect.right() > platformRect.right()) {
//                         player->setX(platformRect.right());
//                     }
//                 }
//             }
//         }

//         // Collision avec pics
//         if (item->type() == QGraphicsPolygonItem::Type) {
//             QRectF spikeRect = item->mapRectToScene(item->boundingRect());
//             if (playerRect.intersects(spikeRect)) {
//                 resetPlayer();
//                 return;
//             }
//         }
//     }

//     // Limites de l'écran
//     if (newY < 0) {
//         finalY = 0;
//         verticalVelocity = 0;
//     }

//     if (newY > sceneRect().height()) {
//         resetPlayer();
//         return;
//     }

//     // Mise à jour position
//     player->setY(finalY);
// }
