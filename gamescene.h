#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QKeyEvent>
#include <QGraphicsPolygonItem>

class GameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GameScene(QObject *parent = nullptr);
    ~GameScene();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void update();  // Game loop function
    void movePlatforms();

private:
    // Player properties
    QGraphicsRectItem* player{nullptr};
    qreal playerSpeed{5.0};
    qreal jumpForce{15.0};
    qreal gravity{0.8};
    qreal verticalVelocity{0.0};
    bool isJumping{false};
    bool moveLeft{false};
    bool moveRight{false};
    bool jump{false};

    // Platform properties
    QTimer* platformTimer{nullptr};
    QTimer* gameTimer{nullptr};
    QGraphicsRectItem* movingPlatform{nullptr};
    QGraphicsRectItem* movingPlatform2{nullptr};
    int movingPlatformDirection{1};
    int movingPlatform2Direction{1};
    int currentScene{1};

    // Scene creation functions
    void createScene(int sceneNumber);
    void createPlatforms(int sceneNumber);
    void createSpikes(int sceneNumber);
};

#endif // GAMESCENE_H
