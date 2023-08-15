#include "vdb_module.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QFuture>
#include <QtConcurrent>
VDB_Module::VDB_Module()
{
    openvdb::initialize();

    machineCount = std::thread::hardware_concurrency();

    // 线程数
    threadCount = machineCount/3;

    // 每个线程处理的体素数量

}

void processVoxel(FloatGrid::Ptr Grid,const openvdb::CoordBBox &bbox ,const openvdb::Vec3i &size,const Coord& coord, float* data)
{
    FloatGrid::ConstAccessor accessor = Grid->getConstAccessor();
    float value = accessor.getValue(coord);

    // 存储结果
    int index = (coord.z() - bbox.min().z()) * size.y() * size.x()
                + (coord.y() - bbox.min().y()) * size.x()
                + (coord.x() - bbox.min().x());
    data[index] = value;
}
QOpenGLTexture* VDB_Module::convertTo3DTex(FloatGrid::Ptr Grid)
{
    /*
    CoordBBox bbox = Grid->evalActiveVoxelBoundingBox();
    Vec3i size = bbox.max() - bbox.min() + Vec3i(1);

    // 将网格数据转换为纹理数据
    float* data = new float[size.x() * size.y() * size.z()];

    /// @读取原始值速度更快
    FloatGrid::ConstAccessor accessor = Grid->getConstAccessor();
    int index = 0;

    for (int z = bbox.min().z(); z <= bbox.max().z(); ++z)
    {
        for (int y = bbox.min().y(); y <= bbox.max().y(); ++y)
        {
            for (int x = bbox.min().x(); x <= bbox.max().x(); ++x)
            {
                Coord coord(x, y, z);

                data[index++] = accessor.getValue(coord);
            }
        }
    }
    */


    QElapsedTimer timer;
    timer.start();


    openvdb::CoordBBox bbox = Grid->evalActiveVoxelBoundingBox();
    openvdb::Vec3d voxelDimSize = Grid->evalActiveVoxelDim().asVec3d();//每个轴有多少个voxel
    openvdb::Vec3d volSize = Grid->voxelSize();//一个voxel的大小是多少理解为精度

    BoxSize = QVector3D((voxelDimSize*volSize).x(),(voxelDimSize*volSize).y(),(voxelDimSize*volSize).z());
    openvdb::math::Transform::Ptr transform = Grid->transformPtr();

    float* data = new float[voxelDimSize.x() * voxelDimSize.y()* voxelDimSize.z()];
    //生成一张3Dtexutre
    QOpenGLTexture *m_texture = new QOpenGLTexture(QOpenGLTexture::Target3D);
    m_texture->setSize(voxelDimSize.x(), voxelDimSize.y(), voxelDimSize.z());
    m_texture->setFormat(QOpenGLTexture::R16F);
    m_texture->allocateStorage(QOpenGLTexture::Red,QOpenGLTexture::Float16);


    const int chunkSize = voxelDimSize.z() / threadCount;


    // 开启多个线程处理数据
    std::vector<std::thread> threads;
    for (int i = 0; i < threadCount; ++i)
    {
        int startZ = bbox.min().z() + i * chunkSize;
        int endZ = (i == threadCount - 1) ? bbox.max().z() : startZ + chunkSize - 1;
        threads.emplace_back([&, startZ, endZ]() {
            FloatGrid::ConstAccessor accessor = Grid->getConstAccessor();
            int index = (startZ - bbox.min().z()) * voxelDimSize.x() * voxelDimSize.y();

            for (int z = startZ; z <= endZ; ++z)
            {
                for (int y = bbox.min().y(); y <= bbox.max().y(); ++y)
                {
                    for (int x = bbox.min().x(); x <= bbox.max().x(); ++x)
                    {
                        Coord coord(x, y, z);

                        data[index++] = accessor.getValue(coord);
                    }
                }
            }
        });
    }
    // 等待所有线程执行完毕
    for (auto& thread : threads)
    {
        thread.join();
    }
    qDebug() << "多线程处理时间" << timer.elapsed() << "milliseconds";
    timer.start();
    m_texture->setData(QOpenGLTexture::Red,QOpenGLTexture::Float32,data);
    m_texture->generateMipMaps();
    m_texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_texture->setMagnificationFilter(QOpenGLTexture::Nearest);
    delete[] data;

    qDebug() << "生成3Dtexure的并且清理指针的时间" << timer.elapsed() << "milliseconds";

    return m_texture;


}
QOpenGLTexture* VDB_Module::readFloatGrid(const std::string& filename, const std::string& gridName)
{

    // Create a VDB file object.
    io::File file(filename);
    FloatGrid::Ptr baseGrid;
    // Open the file.  This reads the file header, but not any grids.
    file.open();
    baseGrid = gridPtrCast<FloatGrid>(file.readGrid(gridName));
    /*
    for (io::File::NameIterator nameIter = file.beginName();
         nameIter != file.endName(); ++nameIter)
    {
        // Read in only the grid we are interested in.
        if (nameIter.gridName() != gridName)
        {
            continue;
        } else {
            baseGrid = gridPtrCast<FloatGrid>(file.readGrid(nameIter.gridName()));
        }
    }*/
    file.close();

    return convertTo3DTex(baseGrid);
}


