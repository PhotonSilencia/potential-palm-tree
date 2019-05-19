#include "Light.h"
#include <vector>

using namespace std;

Light::Light(int n) {
    name = n;
    // create the trianglution of the sphere representation of the light source
    triangulate();
    initBufferObjects();
    calcBB();

}

Light::~Light() {}

void Light::triangulate() {
    // this is a subdivision algorithm that needs a cube as initial input
    vector<QVector3D> triangles(
        cube_data, cube_data + sizeof(cube_data) / sizeof(cube_data[0]));
    size_t steps = 6;
    for (size_t i = 0; i < steps; ++i) {
        vector<QVector3D> pos_new;
        pos_new.reserve(triangles.size());
        for (size_t j = 0; j < triangles.size(); j += 3) {
            QVector3D midab = (triangles[j] + triangles[j + 1]) / 2.0;
            midab = midab * triangles[j].length() / midab.length();
            // add the 2 new triangles (6 vertices) to the mesh
            pos_new.push_back(triangles[j]);
            pos_new.push_back(triangles[j + 2]);
            pos_new.push_back(midab);
            pos_new.push_back(triangles[j + 1]);
            pos_new.push_back(triangles[j + 2]);
            pos_new.push_back(midab);
        }
        triangles = pos_new;
    }

    positions = triangles;
    normals = triangles;
    tex_coords = vector<QVector2D>(triangles.size(), QVector2D(-1, -1));
    indices.clear();  // we don't need index buffers
}

void Light::setAmbient(float r, float g, float b) {
    ambient = QVector3D(r, g, b);
}

void Light::setDiffuse(float r, float g, float b) {
    diffuse = QVector3D(r, g, b);
}

void Light::setSpecular(float r, float g, float b) {
    specular = QVector3D(r, g, b);
}

void Light::setAttenuation(float c, float l, float q) {
    constAtt = c;
    linAtt = l;
    quadAtt = q;
}

QVector3D Light::getAmbient() { return ambient; }

QVector3D Light::getDiffuse() { return diffuse; }

QVector3D Light::getSpecular() { return specular; }

void Light::render(std::shared_ptr<QOpenGLShaderProgram> program) {

    int positionAttributeID, normalAttributeID, texCoordAttributeID;
    positionAttributeID = program->attributeLocation("position");
    normalAttributeID = program->attributeLocation("normal");
    texCoordAttributeID = program->attributeLocation("texCoords");

    // enable the attributes
    program->enableAttributeArray(positionAttributeID);
    program->enableAttributeArray(normalAttributeID);
    program->enableAttributeArray(texCoordAttributeID);

    positionBuffer.bind();
    program->setAttributeBuffer(positionAttributeID, GL_FLOAT, 0, 3);
    positionBuffer.release();
    normalBuffer.bind();
    program->setAttributeBuffer(normalAttributeID, GL_FLOAT, 0, 3);
    normalBuffer.release();
    texCoordBuffer.bind();
    program->setAttributeBuffer(texCoordAttributeID, GL_FLOAT, 0, 2);
    texCoordBuffer.release();

    program->setUniformValue(program->uniformLocation("ambient"), ambient);
    program->setUniformValue(program->uniformLocation("diffuse"), diffuse);
    program->setUniformValue(program->uniformLocation("specular"), specular);
    program->setUniformValue(program->uniformLocation("shininess"), 96.0f);
    program->setUniformValue(program->uniformLocation("alpha"), 1.0f);

    glDrawArrays( GL_TRIANGLES, 0, positions.size());
}
