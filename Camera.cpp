#include "Camera.h"
#include <QDebug>

Camera::Camera(QVector3D position, QVector3D up, float yaw, float pitch) :
    position(position),
    worldUp(up),
    front(-position),
    picth(pitch),
    yaw(yaw),
    
    movementSpeed(SPEED),
    mouseSensitivity(SENSITIVITY),
    zoom(ZOOM) {
    this->updateCameraVectors();

    for (uint i = 0; i != 1024; ++i)
        keys[i] = false;
}

Camera::~Camera()
{

}


// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
QMatrix4x4 Camera::getViewMatrix()
{
    QMatrix4x4 view;
    view.setToIdentity();
    view.lookAt(position, position + front, worldUp);
    return view;
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::processKeyboard(Camera_Movement direction, float deltaTime,const QVector3D &center)
{
    float velocity = this->movementSpeed * deltaTime;
    if (direction == FORWARD)
        this->position += this->front * velocity;
    if (direction == BACKWARD)
        this->position -= this->front * velocity;
    if (direction == LEFT)
        this->position -= this->right * velocity;
    if (direction == RIGHT)
        this->position += this->right * velocity;
    if (direction == UP)
        this->position += this->worldUp * velocity/2;
    if (direction == DOWN)
        this->position -= this->worldUp * velocity/2;
    if (direction == Find_Center)
        this->position = center;
        return;
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::processMouseMovement(float xoffset, float yoffset, bool constraintPitch)
{
    xoffset *= this->mouseSensitivity;
    yoffset *= this->mouseSensitivity;

    //鼠标左右移动速度  可以做除法
    this->yaw += xoffset;
    this->picth += yoffset;

    if (constraintPitch) {
        if (this->picth > 89.0f)
            this->picth = 89.0f;
        if (this->picth < -89.0f)
            this->picth = -89.0f;
    }

    this->updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::processMouseScroll(float yoffset)
{
    if (this->zoom >= 1.0f && this->zoom <= 45.0f)
        this->zoom -= yoffset;
    if (this->zoom > 45.0f)
        this->zoom = 45.0f;
    if (this->zoom < 1.0f)
        this->zoom = 1.0f;
}

void Camera::processInput(float dt,QVector3D cent)
{
    if (keys[Qt::Key_W])
        processKeyboard(FORWARD, dt);
    if (keys[Qt::Key_S])
        processKeyboard(BACKWARD, dt);
    if (keys[Qt::Key_A])
        processKeyboard(LEFT, dt);
    if (keys[Qt::Key_D])
        processKeyboard(RIGHT, dt);
    if (keys[Qt::Key_E])
        processKeyboard(UP, dt);
    if (keys[Qt::Key_Q])
        processKeyboard(DOWN, dt);
    if (keys[Qt::Key_F])
        if (cent == QVector3D(0.0f, 0.0f, 0.0f)) 
        {
            return;
        }
        else {
            processKeyboard(Find_Center, dt, cent);
        }
        
}

void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    QVector3D front;
    front.setX(cos(this->yaw) * cos(this->picth));
    front.setY(sin(this->picth));
    front.setZ(sin(this->yaw) * cos(this->picth));
    this->front = front.normalized();
    this->right = QVector3D::crossProduct(this->front, this->worldUp).normalized();
    this->up = QVector3D::crossProduct(this->right, this->front).normalized();
}
