#include "Sphere.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <QMessageBox>

Sphere::Sphere(QVector3D sp_center, float sp_radius) {

    center = sp_center;
    radius = sp_radius;

    QVector3D p1 = QVector3D{2 * radius, 2 * radius, 0};
    QVector3D p2 = QVector3D{2 * -radius, 2 * radius, 0};
    QVector3D p3 = QVector3D{2 * -radius, 2 * -radius, 0};
    QVector3D p4 = QVector3D{2 * radius, 2 * -radius, 0};

    sp_positions.resize(6);

    sp_positions[0] = p1;
    sp_positions[1] = p2;
    sp_positions[2] = p3;
    sp_positions[3] = p3;
    sp_positions[4] = p4;
    sp_positions[5] = p1;

    // the position buffer
    positionBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    positionBuffer.create();
    positionBuffer.bind();
    positionBuffer.allocate(&sp_positions[0],
                            sp_positions.size() * sizeof(QVector3D));
    positionBuffer.release();

}


void Sphere::render(std::shared_ptr<QOpenGLShaderProgram> program) {

    program->enableAttributeArray("position");
    positionBuffer.bind();
    program->setAttributeBuffer("position", GL_FLOAT, 0, 3);

    program->setUniformValue("radius", radius);
    program->setUniformValue("center", center);

    glDrawArrays(GL_TRIANGLES, 0, 2 * 3);

}

QVector3D Sphere::getCenter(){
    return center;
}
