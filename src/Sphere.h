#ifndef SPHERE_H
#define SPHERE_H

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

class Sphere
{
public:
    Sphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r);

    virtual void render(std::shared_ptr<QOpenGLShaderProgram> program);

    std::shared_ptr<QOpenGLTexture> color;

private:

    std::shared_ptr<QVector3D> center;
    GLfloat radius;

      QOpenGLBuffer positionBuffer;

      const std::vector<QVector3D> positions = {
          QVector3D(0.0f, 1.0f, 0.0f), // triangle 1 : begin
          QVector3D(-2.0f, 1.0f, 0.0f),
          QVector3D(-2.0f, -1.0f, 0.0f), // triangle 1 : end

          QVector3D(-2.0f, -1.0f, 0.0f),
          QVector3D(0.0f, -1.0f, 0.0f),
          QVector3D(0.0f, 1.0f, 0.0f),
      };

};

#endif // SPHERE_H
