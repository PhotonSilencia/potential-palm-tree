#ifndef CGTYPES_H
#define CGTYPES_H

#include <vector>

#include <QImage>
#include <QVector2D>
#include <QVector3D>

// since this programm works only with triangles every vector in the Face-struct
// has dimension 3
struct Face {
    std::vector<unsigned int> positions;
    std::vector<unsigned int> normals;
    std::vector<unsigned int> tex_coords;
};

struct Mesh {
    std::vector<unsigned int> faces;
    unsigned int material;
};

struct Material {
    std::string name;
    QVector3D ambient;
    QVector3D diffuse;
    QVector3D specular;
    float shininess;
    float alpha;      // how light is going through an object
    float sharpness;  // how is does the material reflect (1.0 -> perfect
                      // mirror, 0.0 -> no reflections)
    float density;    // index of refraction i.e. how does the light bend when
                      // going through an object (1.0 means no bending)
    // diffuse (ambient) map
    bool isTexture;
    unsigned int tex_id;
    std::string texName;
    QImage texture;
    // normal map
    bool hasNormalMap;
    unsigned int normalMap_id;
    std::string normalMapName;
    QImage normalMap;
};

#endif  // CGTYPES_H
