#ifndef LIGHT_H
#define LIGHT_H

#include "Model.h"

const QVector3D cube_data[] = {
    QVector3D(-1.0f, -1.0f, -1.0f),  // triangle 1 : begin
    QVector3D(-1.0f, -1.0f, 1.0f),
    QVector3D(-1.0f, 1.0f, 1.0f),  // triangle 1 : end
    QVector3D(1.0f, 1.0f, -1.0f),  // triangle 2 : begin
    QVector3D(-1.0f, -1.0f, -1.0f),
    QVector3D(-1.0f, 1.0f, -1.0f),  // triangle 2 : end
    QVector3D(1.0f, -1.0f, 1.0f),
    QVector3D(-1.0f, -1.0f, -1.0f),
    QVector3D(1.0f, -1.0f, -1.0f),
    QVector3D(1.0f, 1.0f, -1.0f),
    QVector3D(1.0f, -1.0f, -1.0f),
    QVector3D(-1.0f, -1.0f, -1.0f),
    QVector3D(-1.0f, -1.0f, -1.0f),
    QVector3D(-1.0f, 1.0f, 1.0f),
    QVector3D(-1.0f, 1.0f, -1.0f),
    QVector3D(1.0f, -1.0f, 1.0f),
    QVector3D(-1.0f, -1.0f, 1.0f),
    QVector3D(-1.0f, -1.0f, -1.0f),
    QVector3D(-1.0f, 1.0f, 1.0f),
    QVector3D(-1.0f, -1.0f, 1.0f),
    QVector3D(1.0f, -1.0f, 1.0f),
    QVector3D(1.0f, 1.0f, 1.0f),
    QVector3D(1.0f, -1.0f, -1.0f),
    QVector3D(1.0f, 1.0f, -1.0f),
    QVector3D(1.0f, -1.0f, -1.0f),
    QVector3D(1.0f, 1.0f, 1.0f),
    QVector3D(1.0f, -1.0f, 1.0f),
    QVector3D(1.0f, 1.0f, 1.0f),
    QVector3D(1.0f, 1.0f, -1.0f),
    QVector3D(-1.0f, 1.0f, -1.0f),
    QVector3D(1.0f, 1.0f, 1.0f),
    QVector3D(-1.0f, 1.0f, -1.0f),
    QVector3D(-1.0f, 1.0f, 1.0f),
    QVector3D(1.0f, 1.0f, 1.0f),
    QVector3D(-1.0f, 1.0f, 1.0f),
    QVector3D(1.0f, -1.0f, 1.0f)};

class Light : public Model {
   public:
    Light(int name);
    virtual ~Light();

    void setAmbient(float r, float g, float b);
    void setDiffuse(float r, float g, float b);
    void setSpecular(float r, float g, float b);
    void setAttenuation(float constAtt, float linAtt, float quadAtt);

    QVector3D getAmbient();
    QVector3D getDiffuse();
    QVector3D getSpecular();

    virtual void render(std::shared_ptr<QOpenGLShaderProgram> program);

   private:
    void triangulate();

    QVector3D ambient;
    QVector3D diffuse;
    QVector3D specular;
    float constAtt;
    float linAtt;
    float quadAtt;
};

#endif  // LIGHT_H
