#include "Skybox.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <QMessageBox>
#include "Model.h"

using namespace std;

static const std::vector<QOpenGLTexture::CubeMapFace> faces =
{
    QOpenGLTexture::CubeMapPositiveX,
    QOpenGLTexture::CubeMapPositiveZ,
    QOpenGLTexture::CubeMapNegativeY,
    QOpenGLTexture::CubeMapNegativeZ,
    QOpenGLTexture::CubeMapNegativeX,
    QOpenGLTexture::CubeMapPositiveY
};

Skybox::Skybox() {

    // the position buffer
    positionBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    positionBuffer.create();
    positionBuffer.bind();
    positionBuffer.allocate(&positions[0],
                            positions.size() * sizeof(QVector3D));
    positionBuffer.release();

  // An dieser Stelle wird wieder die nullte Texturebene genutzt.
  glActiveTexture(GL_TEXTURE0);

  skyImages.resize(6);

  skyImages[0] = std::unique_ptr<QImage>(new QImage(
      QString("skybox/right.png")));
  skyImages[1] =
      std::unique_ptr<QImage>(new QImage(QString("skybox/back.png")));
  skyImages[2] = std::unique_ptr<QImage>(
      new QImage(QString("skybox/down.png")));
  skyImages[3] = std::unique_ptr<QImage>(new QImage(
      QString("skybox/front.png")));
  skyImages[4] = std::unique_ptr<QImage>(
      new QImage(QString("skybox/left.png")));
  skyImages[5] = std::unique_ptr<QImage>(new QImage(QString("skybox/up.png")));


  // Prepare texture
  texture = std::make_shared<QOpenGLTexture>(QOpenGLTexture::TargetCubeMap);

  texture->create();
  texture->setSize(skyImages[0]->width(), skyImages[0]->height(), skyImages[0]->depth());
  texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
  texture->allocateStorage();

  // Transfer all pictures
  for (size_t i = 0; i < faces.size(); i++) {

      texture->setData(0, 0, faces[i], QOpenGLTexture::BGRA, QOpenGLTexture::UInt8, (const void*)skyImages[i]->constBits(), 0);

  }

  // Generate Mipmaps
  texture->setWrapMode(QOpenGLTexture::ClampToEdge);
  texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture->generateMipMaps();
}


void Skybox::render(std::shared_ptr<QOpenGLShaderProgram> program) {

    int positionAttributeID;
    positionAttributeID = program->attributeLocation("position");

    // enable the attributes
    program->enableAttributeArray(positionAttributeID);

    // set the geometry using the generated VBOs
    positionBuffer.bind();
    program->setAttributeBuffer(positionAttributeID, GL_FLOAT, 0, 3);
    positionBuffer.release();

    glDisable(GL_DEPTH_TEST);

    texture->bind(0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->textureId());
    program->setUniformValue("skybox", 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glEnable(GL_DEPTH_TEST);

}
