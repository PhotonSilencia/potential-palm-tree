#ifndef SPHERE_H
#define SPHERE_H

#include <QGLFunctions>
#include <vector>
#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>

#include <iostream>
#include <memory>
#include <array>

#include "CGTypes.h"

class Sphere
{
public:
    Sphere(QVector3D sp_center, float sp_radius);
    virtual void render(std::shared_ptr<QOpenGLShaderProgram> program);

private:

    QGLFunctions gl;
    QOpenGLBuffer positionBuffer;

    QVector3D center;
    QVector3D normal;
    float radius;

    std::vector<QVector3D> sp_positions;
    /*= {

        QVector3D(-1.0f, 1.0f, 0.0f), // triangle 1 : begin
        QVector3D(-1.0f, -1.0f, 0.0f),
        QVector3D(1.0f, -1.0f, 0.0f), // triangle 1 : end

        QVector3D(1.0f, 1.0f, 0.0f), // triangle 2 : begin
        QVector3D(-1.0f, 1.0f, 0.0f),
        QVector3D(1.0f, -1.0f, 0.0f), // triangle 2 : end

    };*/
};

#endif // SPHERE_H
