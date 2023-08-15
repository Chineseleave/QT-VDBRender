#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "vdb_module.h"
#include <QMainWindow>
#include <QOpenGLWidget>
#include <qopenglfunctions_4_5_core.h>
#include <QtOpenGL/QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QEvent>
#include <QKeyEvent>
#include <Camera.h>
#include <QOpenGLVersionFunctionsFactory>
#include <QOpenGLFunctions>
#include <QPainter>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }

QT_END_NAMESPACE
struct GeometryData
{
    std::vector<GLfloat> points,normals;
    std::vector<GLuint> indices;
};

Q_DECLARE_METATYPE(GeometryData)
class VDB_Module;
class MainWindow : public QOpenGLWidget ,protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    MainWindow(QOpenGLWidget *parent = nullptr);
    ~MainWindow();
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
    void test();
    void addVDBBox();
    void createShader(QOpenGLShaderProgram &use_Shader,const QString &vertStr,const QString &fragStr);
protected:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
    //void convertToTexture(FloatGrid::Ptr Grid);
    //void convertToTextureMultThread();

    //void vdbConvertPolygon();
    void testPoly();
    void LoadVDBSequence(int startFrame, int endFrame);
private:
    //Ui::MainWindow *ui;
    QOpenGLFunctions_4_5_Core *f = nullptr;
    QOpenGLVertexArrayObject Vao;
    std::vector<float> vdbData;
    QOpenGLBuffer vbo,ebo;
    QOpenGLBuffer vdbPointsPos_Vbo,vdbPointsNormal_Vbo;


    QOpenGLBuffer vdbBoxVbo,vdbBoxEbo;
    QOpenGLVertexArrayObject vdbBoxVao;
    QOpenGLShaderProgram vdbBoxShader;


    QOpenGLShaderProgram shader;
    QOpenGLTexture *m_texture=nullptr;
    std::vector<QOpenGLTexture*> m_Vdbsequence;
    GLuint m_textureID;
    int m_width = 64, m_height = 64, m_depth = 64;

    QVector3D vdbSize;

private:
    std::unique_ptr<Camera> camera;
    QVector3D F_Track;
    QMatrix4x4 projection,viewMat,model;
    bool m_bLeftPressed{false};
    QPoint m_lastPos;
    void initCamera();
    float slice;
    GeometryData m_GeometryData;
    int timeStep = 1;
private:
    VDB_Module *m_VDB_Module = nullptr;
    QPainter qp;
    QPainter CurrentFrame;
    QString str_Fps;
    QVector<QPen> pens;
    QVector<QVector3D> pointsPos;
};
#endif // MAINWINDOW_H
