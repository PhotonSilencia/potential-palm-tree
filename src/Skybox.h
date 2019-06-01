#ifndef SKYBOX_H
#define SKYBOX_H

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

class Skybox
{
public:
    Skybox();

    virtual void render(std::shared_ptr<QOpenGLShaderProgram> program);
    void bindTexture();

    std::shared_ptr<QOpenGLTexture> texture;

private:

      QOpenGLBuffer positionBuffer;
      std::vector<std::unique_ptr<QImage>> skyImages;

      const std::vector<QVector3D> positions = {
          QVector3D(-50.0f,50.0f,-50.0f), // triangle 1 : begin
          QVector3D(-50.0f,-50.0f, -50.0f),
          QVector3D(-50.0f, -50.0f, 50.0f), // triangle 1 : end

          QVector3D(-50.0f,50.0f,-50.0f),// 5
          QVector3D(-50.0f, -50.0f, 50.0f),
          QVector3D(-50.0f, 50.0f,50.0f),

          QVector3D(-50.0f,-50.0f,-50.0f),
          QVector3D(-50.0f, 50.0f,-50.0f), // triangle 2 : end
          QVector3D(50.0f, 50.0f,-50.0f), // triangle 2 : begin

          QVector3D(-50.0f,-50.0f,-50.0f),
          QVector3D(50.0f, 50.0f,-50.0f),// 4
          QVector3D(50.0f,-50.0f,-50.0f),

          QVector3D(-50.0f,-50.0f,-50.0f),
          QVector3D(-50.0f,-50.0f,50.0f),
          QVector3D(50.0f,-50.0f, 50.0f), // 3

          QVector3D(-50.0f,-50.0f,-50.0f),
          QVector3D(50.0f,-50.0f, 50.0f),// 6
          QVector3D(50.0f,-50.0f, -50.0f),

          QVector3D(-50.0f,-50.0f, 50.0f),
          QVector3D(-50.0f,50.0f, 50.0f),
          QVector3D( 50.0f, 50.0f, 50.0f),// 7

          QVector3D(-50.0f, -50.0f, 50.0f),// 12
          QVector3D(50.0f, 50.0f, 50.0f),
          QVector3D(50.0f,-50.0f, 50.0f),

          QVector3D(50.0f,50.0f,-50.0f), // triangle 1 : begin
          QVector3D(50.0f,-50.0f, -50.0f),
          QVector3D(50.0f, -50.0f, 50.0f), // triangle 1 : end

          QVector3D(50.0f,50.0f,-50.0f),// 5
          QVector3D(50.0f, -50.0f, 50.0f),
          QVector3D(50.0f, 50.0f,50.0f),

          QVector3D(-50.0f, 50.0f,-50.0f),
          QVector3D(-50.0f, 50.0f, 50.0f),// 10
          QVector3D(50.0f, 50.0f,50.0f),

          QVector3D(-50.0f, 50.0f,-50.0f),
          QVector3D(50.0f, 50.0f, 50.0f),
          QVector3D(50.0f, 50.0f, -50.0f)// 11
      };

};

#endif // SKYBOX_H
