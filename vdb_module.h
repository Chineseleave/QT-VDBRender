#ifndef VDB_MODULE_H
#define VDB_MODULE_H

#include <openvdb/openvdb.h>
#include <openvdb/io/Stream.h>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix3x3>
#include <QTransform>
#include <QVector3D>
using namespace openvdb;

class VDB_Module
{
public:
    VDB_Module();
    QOpenGLTexture* readFloatGrid(const std::string& filename, const std::string& gridName = "density");
    QVector3D BoxSize;
    QVector3D getBoxSize(){
        return BoxSize;
    }
private:
    QOpenGLTexture *convertTo3DTex(FloatGrid::Ptr Grid);
    int machineCount=0;
    int threadCount=0;

};

#endif // VDB_MODULE_H
