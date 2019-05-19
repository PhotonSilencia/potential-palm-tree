#include "Sphere.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <QMessageBox>

Sphere::Sphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r) {

    radius = r;
    center = std::make_shared<QVector3D>(x,y,z);

    // the position buffer
    positionBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    positionBuffer.create();
    positionBuffer.bind();
    positionBuffer.allocate(&positions[0],
                            positions.size() * sizeof(QVector3D));
    positionBuffer.release();

}


void Sphere::render(std::shared_ptr<QOpenGLShaderProgram> program) {

    int positionAttributeID;
    positionAttributeID = program->attributeLocation("position");

    // enable the attributes
    program->enableAttributeArray(positionAttributeID);

    // set the geometry using the generated VBOs
    positionBuffer.bind();
    program->setAttributeBuffer(positionAttributeID, GL_FLOAT, 0, 3);
    positionBuffer.release();

    program->setUniformValue("radius", radius);
    program->setUniformValue("center", *center);

    glDrawArrays(GL_TRIANGLES, 0, 36);

}
