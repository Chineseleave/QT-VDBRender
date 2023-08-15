#include "mainwindow.h"
//#include "./ui_mainwindow.h"
#include <QElapsedTimer>
#include <queue>
#include <valarray>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QDebug>
#include "geometrysample.h"

MainWindow::MainWindow(QOpenGLWidget *parent)
    : QOpenGLWidget(parent),
      vbo(QOpenGLBuffer::VertexBuffer),
      ebo(QOpenGLBuffer::IndexBuffer),
      vdbBoxVbo(QOpenGLBuffer::VertexBuffer),
      vdbBoxEbo(QOpenGLBuffer::IndexBuffer)
{
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setSamples(4);//多重采样
    this->setFormat(surfaceFormat); //setFormat是QOpenGLWidget的函数
    m_VDB_Module = new VDB_Module;

    QPen nor_Pen;
    QPen low_Pen;
    QPen lowset_Pen;

    nor_Pen.setColor(QColor(0, 255, 0));
    low_Pen.setColor(QColor(0, 255, 255));
    lowset_Pen.setColor(QColor(234, 250, 230));

    pens << nor_Pen << low_Pen << lowset_Pen;
}
void MainWindow::LoadVDBSequence(int startFrame, int endFrame)
{
    m_Vdbsequence.resize(endFrame - startFrame + 1);
    for (int i = startFrame; i != endFrame; ++i) {
        const QString& sequenceStr = QString("P:/TaoTieAssets/Vdb/Cloud/Single/cloud_v058_0.02.vdb").arg(i);
    }
}


/*
void MainWindow::vdbConvertPolygon()
{
    tools::VolumeToMesh mesher(m_grid->getGridClass() == openvdb::GRID_LEVEL_SET ? 0.0 : 0.01);
    mesher(*m_grid);

    // Copy points and generate point normals.
    m_GeometryData.points.resize(mesher.pointListSize() * 3);
    m_GeometryData.normals.resize(mesher.pointListSize() * 3);

    for (Index64 n = 0, i = 0,  N = mesher.pointListSize(); n < N; ++n) {
        const openvdb::Vec3s& p = mesher.pointList()[n];
        m_GeometryData.points[i++] = p[0];
        m_GeometryData.points[i++] = p[1];
        m_GeometryData.points[i++] = p[2];
    }

    // Copy primitives
    tools::PolygonPoolList& polygonPoolList = mesher.polygonPoolList();
    Index64 numQuads = 0;
    for (Index64 n = 0, N = mesher.polygonPoolListSize(); n < N; ++n) {
        numQuads += polygonPoolList[n].numQuads();
    }

    m_GeometryData.indices.reserve(numQuads * 4);
    Vec3d normal, e1, e2;

    for (Index64 n = 0, N = mesher.polygonPoolListSize(); n < N; ++n) {
        const tools::PolygonPool& polygons = polygonPoolList[n];
        for (Index64 i = 0, I = polygons.numQuads(); i < I; ++i) {
            const Vec4I& quad = polygons.quad(i);
            m_GeometryData.indices.emplace_back(quad[0]);
            m_GeometryData.indices.emplace_back(quad[1]);
            m_GeometryData.indices.emplace_back(quad[2]);
            m_GeometryData.indices.emplace_back(quad[3]);

            e1 = mesher.pointList()[quad[1]];
            e1 -= mesher.pointList()[quad[0]];
            e2 = mesher.pointList()[quad[2]];
            e2 -= mesher.pointList()[quad[1]];
            normal = e1.cross(e2);

            const double length = normal.length();
            if (length > 1.0e-7) normal *= (1.0 / length);
            for (int v = 0; v < 4; ++v)
            {
                m_GeometryData.normals[quad[v]*3]    = static_cast<GLfloat>(-normal[0]);
                m_GeometryData.normals[quad[v]*3+1]  = static_cast<GLfloat>(-normal[1]);
                m_GeometryData.normals[quad[v]*3+2]  = static_cast<GLfloat>(-normal[2]);
            }
        }
    }
}*/
//计算帧速率
void CalculateFrameRate(QString& str_Fps)
{
    // This will store our fps
    //计算FPS
    static float framesPerSecond = 0.0f;
    static float lastTime = 0.0f;       // This will hold the time from the last frame
    float currentTime = float(GetTickCount64()) * 0.001f;
    ++framesPerSecond;
    if (currentTime - lastTime > 1.0f)
    {

        lastTime = currentTime;
        str_Fps = QString::number(int(framesPerSecond));
        framesPerSecond = 0;
    }
}
MainWindow::~MainWindow()
{
    //delete ui;
    Vao.destroy();
}

void MainWindow::initializeGL(){
    initializeOpenGLFunctions();
    initCamera();
    glClearColor(0, 0, 0, 1);
    //m_grid = readVDB("P:/Houdini/BuildTest/testsmoke.vdb");
    f = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_5_Core>();
    QElapsedTimer timer;
    timer.start();

    //LoadVDBSequence(1,127);



    //qDebug()<<vdbSize;
    //qDebug() << "The slow operation took" << timer.elapsed() << "milliseconds";
    /*generalSampleGeometryPoints(5000);*/

    QString path = QString("P:/TaoTieAssets/Vdb/Zeno/smoke1/smoke_%1.vdb").arg(timeStep);

    m_texture = m_VDB_Module->readFloatGrid("P:/TaoTieAssets/Vdb/Cloud/Single/cloud_v058_0.02.vdb","density");
    vdbSize = m_VDB_Module->getBoxSize();

    glEnable(GL_TEXTURE_3D);
    glEnable(GL_DEPTH_TEST);
    //testPoly();使用polygon渲染

    test();

    addVDBBox();
    // 启用深度测试

}
void MainWindow::resizeGL(int w, int h){
    glViewport(0, 0, w, h);
    projection.setToIdentity();
    projection.perspective(45.0f, (float)w / h, 0.001f, 1000.0f);
}
void MainWindow::paintGL(){
    //glEnable(GL_DEPTH_TEST);
    CalculateFrameRate(str_Fps);
    glClearColor(0.0,0.0,0.0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //获取mvp矩阵
    //GL_CHECK_ERRORS
    camera->processInput(0.05f, F_Track);//speed
    projection.setToIdentity();
    projection.perspective(camera->zoom, 1.0f * width() / height(), 0.001f, 10000.0f);
    viewMat.setToIdentity();
    viewMat = camera->getViewMatrix();
    /*
    QString path = QString("P:/TaoTieAssets/Vdb/Zeno/smoke1/smoke_%1.vdb").arg(timeStep);

    m_texture = m_VDB_Module->readFloatGrid(path.toStdString(),"density");

    test();*/

    glEnable(GL_BLEND);

    {
        QOpenGLVertexArrayObject::Binder vaoBind(&Vao);
        // 绑定着色器程序和纹理
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//需要渲染体积的时候开启
        shader.bind();
        shader.setUniformValue("view",viewMat);
        shader.setUniformValue("projection",projection);
        shader.setUniformValue("model",QMatrix4x4(1,0,0,0,
                                                  0,1,0,0,
                                                  0,0,1,0,
                                                  0,0,0,1));
        shader.setUniformValue("VdbBoxSize",QMatrix4x4(vdbSize.x(),0,0,0,
                                                        0,vdbSize.y(),0,0,
                                                        0,0,vdbSize.z(),0,
                                                        0,0,0,1));
        shader.setUniformValue("camPos",camera->position);
        m_texture->bind();

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    glDisable(GL_BLEND);
/*
    glEnable(GL_DEPTH_TEST);
    {
        QOpenGLVertexArrayObject::Binder vaoBind(&vdbBoxVao);
        // 绑定着色器程序和纹理

        vdbBoxShader.bind();
        vdbBoxShader.setUniformValue("view",viewMat);
        vdbBoxShader.setUniformValue("projection",projection);
        m_texture->bind();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }*/

    /*
    {
        QOpenGLVertexArrayObject::Binder vaoBind(&Vao);
        // 绑定着色器程序和纹理
        shader.bind();
        shader.setUniformValue("view",viewMat);
        shader.setUniformValue("projection",projection);
        glDrawElements(GL_QUADS, m_GeometryData.indices.size(), GL_UNSIGNED_INT, m_GeometryData.indices.data());
        //glDrawArrays(GL_TRIANGLES,0,m_GeometryData.points.size());
    }

    m_texture->release();
    shader.release();
    m_texture->destroy();
    glDisable(GL_SRC_ALPHA);
    glDisable(GL_ONE_MINUS_SRC_ALPHA);
    {
        QOpenGLVertexArrayObject::Binder vaoBind(&Vao);
        shader.bind();
        shader.setUniformValue("view",viewMat);
        shader.setUniformValue("projection",projection);
        glDrawArrays(GL_POINTS,0,pointsPos.size());
    }
    shader.release();*/


    int i_strFps = str_Fps.toInt();               //字符串转int

    qp.begin(this);

    if (i_strFps > 60)
    {
        qp.setPen(pens[0]);
    }
    else if (i_strFps < 59 && i_strFps>40)
    {
        qp.setPen(pens[1]);
    }
    else if (i_strFps < 39 && i_strFps>20)
    {
        qp.setPen(QColor(0, 0, 255));
    }
    else if (i_strFps < 19)
    {
        qp.setPen(pens[2]);
    }

    qp.drawText(width() - 40, height() - 10, "FPS:" + str_Fps);
    qp.end();


    QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);

    timeStep++;
    if(timeStep>=30){
        timeStep=1;
    }
}
void MainWindow::createShader(QOpenGLShaderProgram &use_Shader, const QString &vertStr, const QString &fragStr)
{
    if (!use_Shader.isLinked())
    {
        bool success = use_Shader.addShaderFromSourceFile(QOpenGLShader::Vertex, vertStr);
        if (!success) {
            qDebug() << "shaderProgram addShaderFromSourceFile failed!" << use_Shader.log();
            return;
        }
        success = use_Shader.addShaderFromSourceFile(QOpenGLShader::Fragment, fragStr);
        if (!success) {
            qDebug() << "shaderProgram addShaderFromSourceFile failed!" << use_Shader.log();
            return;
        }
        success = use_Shader.link();
        if(!success) {
            qDebug() << "shaderProgram link failed!" << use_Shader.log();
        }
    }

}
void MainWindow::test()
{
    createShader(shader,":/test_volumevert1.vert",":/test_volumefrag1.frag");

    // 定义顶点坐标和索引数组
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.5f,     // V0
        0.5f, -0.5f, 0.5f,      // V1
        -0.5f, 0.5f, 0.5f,      // V2
        0.5f, 0.5f, 0.5f,       // V3
        -0.5f, -0.5f, -0.5f,    // V4
        0.5f, -0.5f, -0.5f,     // V5
        -0.5f, 0.5f, -0.5f,     // V6
        0.5f, 0.5f, -0.5f       // V7
    };

    GLuint indices[] = {
        // 前面
        0, 1, 2,
        1, 3, 2,
        // 后面
        5, 4, 7,
        4, 6, 7,
        // 左边
        4, 0, 6,
        0, 2, 6,
        // 右边
        1, 5, 3,
        5, 7, 3,
        // 上面
        2, 3, 6,
        3, 7, 6,
        // 底部
        4, 5, 0,
        5, 1, 0
    };

    vbo.create();
    vbo.bind();
    vbo.allocate(vertices, sizeof(vertices));

    {
        QOpenGLVertexArrayObject::Binder vaoBind(&Vao);
        ebo.create();
        ebo.bind();
        ebo.allocate(indices,sizeof(indices));
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        shader.setUniformValue("volume", 0);

        //shader.setUniformValue("step_size", QVector3D(1.0/m_texture->width(), 1.0/m_texture->height(), 1.0/m_texture->depth()));
    }
    shader.release();
    vbo.release();

}
void MainWindow::keyPressEvent(QKeyEvent* event)
{
    int key = event->key();

    if (key >= 0 && key < 1024)
        camera->keys[key] = true;

}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{

    int key = event->key();
    if (key >= 0 && key < 1024)
        camera->keys[key] = false;

}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftPressed = true;
        m_lastPos = event->pos();

    }

}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event)

    m_bLeftPressed = false;

}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_bLeftPressed) {
        int xpos = event->pos().x();
        int ypos = event->pos().y();

        int xoffset = xpos - m_lastPos.x();
        int yoffset = m_lastPos.y() - ypos;
        m_lastPos = event->pos();
        camera->processMouseMovement(xoffset, yoffset);
    }

}

void MainWindow::wheelEvent(QWheelEvent* event)
{
    QPoint offset = event->angleDelta();
    camera->processMouseScroll(offset.y() / 5.0f);

}

void MainWindow::initCamera(){
    F_Track = QVector3D(0, 0, 0);//初始化追踪模型位置
    m_bLeftPressed = false;
    camera = std::make_unique<Camera>(QVector3D(0.0f, 0.0f, 0.0f));
}

void MainWindow::testPoly(){

    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/polygon_Vert.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/polygon_frag.frag");
    shader.link();

    vdbPointsPos_Vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vdbPointsPos_Vbo.create();
    vdbPointsPos_Vbo.bind();
    vdbPointsPos_Vbo.allocate(m_GeometryData.points.data(), sizeof(GLfloat)*m_GeometryData.points.size());

    vdbPointsNormal_Vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vdbPointsNormal_Vbo.create();
    vdbPointsNormal_Vbo.bind();
    vdbPointsNormal_Vbo.allocate(m_GeometryData.normals.data(),sizeof(GLfloat)*m_GeometryData.normals.size());

    ebo.create();
    ebo.bind();
    ebo.allocate(m_GeometryData.indices.data(),sizeof(GLuint)*m_GeometryData.indices.size());

    {
        QOpenGLVertexArrayObject::Binder vaoBind(&Vao);
        vdbPointsPos_Vbo.bind();
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        vdbPointsNormal_Vbo.bind();
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
    }
    vdbPointsPos_Vbo.release();
    vdbPointsNormal_Vbo.release();
}

void MainWindow::addVDBBox()
{
    createShader(vdbBoxShader,":/test_vert.vert",":/test_frag.frag");

    // 定义顶点坐标和索引数组
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.5f,     // V0
        0.5f, -0.5f, 0.5f,      // V1
        -0.5f, 0.5f, 0.5f,      // V2
        0.5f, 0.5f, 0.5f,       // V3
        -0.5f, -0.5f, -0.5f,    // V4
        0.5f, -0.5f, -0.5f,     // V5
        -0.5f, 0.5f, -0.5f,     // V6
        0.5f, 0.5f, -0.5f       // V7
    };

    GLuint indices[] = {
        // 前面
        0, 1, 2,
        1, 3, 2,
        // 后面
        5, 4, 7,
        4, 6, 7,
        // 左边
        4, 0, 6,
        0, 2, 6,
        // 右边
        1, 5, 3,
        5, 7, 3,
        // 上面
        2, 3, 6,
        3, 7, 6,
        // 底部
        4, 5, 0,
        5, 1, 0
    };
    vdbBoxVbo.create();
    vdbBoxVbo.bind();
    vdbBoxVbo.allocate(vertices, sizeof(vertices));

    {
        QOpenGLVertexArrayObject::Binder vaoBind(&vdbBoxVao);
        vdbBoxEbo.create();
        vdbBoxEbo.bind();
        vdbBoxEbo.allocate(indices,sizeof(indices));
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);



    }
    vdbBoxShader.release();
    vdbBoxVbo.release();

}
