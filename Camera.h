#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QMatrix4x4>
#include <QKeyEvent>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    Find_Center
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 1.0f;
const float SENSITIVITY = 0.01f;
const float ZOOM = 45.0f;

class Camera {
public:
    Camera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH);
    ~Camera();

    QMatrix4x4 getViewMatrix();
    void processMouseMovement(float xoffset, float yoffset, bool constraintPitch = true);
    void processMouseScroll(float yoffset);
    void processInput(float dt, QVector3D cent);

    QVector3D position;
    QVector3D worldUp;
    QVector3D front;

    QVector3D up;
    QVector3D right;

    //Eular Angles
    float picth;
    float yaw;

    //Camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    //Keyboard multi-touch
    bool keys[1024];
private:
    void updateCameraVectors();
    void processKeyboard(Camera_Movement direction, float deltaTime,const QVector3D &center = QVector3D(0.0f, 0.0f, 0.0f));
};

#endif // CAMERA_H
