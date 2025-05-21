#include "player.h"
#include <QDebug>
#include "gamescene.h"


using namespace std;

Player::Player(GameScene* scene)
    : gameScene(scene),
    moveLeft(false),
    moveRight(false),
    jump(false),
    onPlatform(false),
    playerSpeed(5.0),
    jumpForce(15.0),
    gravity(0.8),
    verticalVelocity(0),
    isJumping(false),
    hasSword(false),
    hasKnife(false),
    score(0),
    coins(0)
{
    // Create sprite
    mSprite = new playersprite(
        ":/anim/anim/kid.png",
        ":/anim/anim/kid (1) (1).json",
        ":/anim/anim/kid.json"
    );
    addToGroup(mSprite);
    if (mSprite->boundingRect().isValid()) {
        qDebug() << "Sprite initialized with size:" << mSprite->boundingRect();
    }

    // Initial animation
    if (mSprite) {
        mSprite->playAnimation("stand");
    }

    // Set up timer for updates
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Player::update);
    timer->start(16); // ~60fps

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
    setZValue(100);
}

// Handle keyboard input for moving the player and detecting collisions.
void Player::keyPressEvent(QKeyEvent* event)
{
    if (!mSprite) return;

    qDebug() << "Player::keyPressEvent - Key pressed:" << event->key(); // ADDED THIS LINE

    switch (event->key()) {
    case Qt::Key_Left:
        moveLeft = true;
        mSprite->playAnimation("run");
        break;
    case Qt::Key_Right:
        moveRight = true;
        mSprite->playAnimation("run");
        break;
    case Qt::Key_Space:
        if (!isJumping) {
            jump = true;
            isJumping = true;
            verticalVelocity = -jumpForce;
            mSprite->playAnimation("jump");
        }
        break;
    }

    mSprite->keyPressEvent(event); //pass the key event to the sprite to trigger the correct animation
}

void Player::keyReleaseEvent(QKeyEvent *event)
{
    qDebug() << "Player::keyReleaseEvent - Key released:" << event->key();  // ADDED THIS LINE
    switch (event->key()) {
    case Qt::Key_Left:
        moveLeft = false;
        if (!moveRight) mSprite->playAnimation("stand");
        break;
    case Qt::Key_Right:
        moveRight = false;
        if (!moveLeft) mSprite->playAnimation("stand");
        break;
    case Qt::Key_Space:
        jump = false;
        break;
    }
    mSprite->keyReleaseEvent(event);
}

void Player::update() {
    QPointF pos = this->pos();

    // Horizontal movement
    if (moveLeft) pos.setX(pos.x() - playerSpeed);
    if (moveRight) pos.setX(pos.x() + playerSpeed);

    // Vertical movement
    verticalVelocity += gravity;
    pos.setY(pos.y() + verticalVelocity);

    // Check platform collisions
    onPlatform = false;
    for (QGraphicsItem* item : collidingItems()) {
        if (Platform* platform = dynamic_cast<Platform*>(item)) {
            if (verticalVelocity > 0 && pos.y() + playerBoundingRect().height() <= platform->pos().y()) {
                pos.setY(platform->pos().y() - playerBoundingRect().height());
                verticalVelocity = 0;
                isJumping = false;
                onPlatform = true;
            }
        }
    }

    setPos(pos);
}

QRectF Player::playerBoundingRect() const {
    if (mSprite) {
        return mSprite->boundingRect();
    }
    return QRectF(0, 0, 40, 50); // Fallback dimensions
}

void Player::addScore(int pts) {
    score += pts;
}

void Player::spendCoins(int amount) {
    if (coins >= amount) {
        coins -= amount;
    }
}

void Player::gainCoins(int amount) {
    coins += amount;
}

void Player::buySword() {
    if (coins >= 10 && !hasSword) {
        spendCoins(10);
        hasSword = true;
        emit swordPickedUp();
    }
}

void Player::buyKnife() {
    if (coins >= 5 && !hasKnife) {
        spendCoins(5);
        hasKnife = true;
    }
}
