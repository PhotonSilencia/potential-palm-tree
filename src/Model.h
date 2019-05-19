#ifndef MODEL_H
#define MODEL_H

#include <QGLFunctions>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QVector2D>
#include <QVector3D>
#include <memory>
#include <vector>

#include "CGTypes.h"

struct BoundingBox {
  QVector3D center;
  QVector3D bbmin;
  QVector3D bbmax;
};

class Model {
public:
  Model(QString path, int name, std::vector<Mesh> meshes,
        std::vector<Face> faces, std::vector<Material> materials,
        std::vector<QVector3D> positions, std::vector<QVector3D> normals,
        std::vector<QVector2D> tex_coords);
  // an empty model
  Model();
  virtual ~Model();

  // render the model
  virtual void render(std::shared_ptr<QOpenGLShaderProgram> program);

  // static functions to read a model from an obj file
  static std::shared_ptr<Model> importOBJFile(QString fileName);
  static void readMTLFile(QString fileName, std::vector<Material> &materials);
  // static variable for the name of all objects
  static int nameCount;

  // get the model matrix
  QMatrix4x4 getTransformations();
  // set the model matrix (used, when a scene is loaded)
  void setTransformations(QMatrix4x4 t);

  // read all the properties (and their sizes), so the scene can draw the
  // model
  int getName();
  Mesh *getMeshes();
  Face *getFaces();
  Material *getMaterials();
  QVector3D *getNormals();
  QVector3D *getPositions();
  QVector2D *getTexCoords();
  size_t getNmeshes();
  size_t getNfaces();
  size_t getNmaterials();
  size_t getNnormals();
  size_t getNpositions();
  size_t getNtexCoords();
  QString getPath();

  // those functions are called by the gui
  void rotate(QVector3D axis, float angle);
  void translate(double dx, double dy, double dz);
  void scale(double sx, double sy, double sz);
  BoundingBox getBoundingBox();

protected:
  // the path, where the model is (only needed for saving the scene)
  QString path;
  // the name (index) of the model
  int name;

  // the data as it comes from the obj file
  std::vector<Mesh> meshes;
  std::vector<Face> faces;
  std::vector<Material> materials;
  std::vector<QVector3D> positions;
  std::vector<QVector3D> normals;
  std::vector<QVector2D> tex_coords;

  // the opengl buffer objects
  QOpenGLBuffer positionBuffer;
  QOpenGLBuffer normalBuffer;
  QOpenGLBuffer texCoordBuffer;
  std::vector<QOpenGLBuffer> indexBuffers;
  // the opengl index array
  std::vector<std::vector<GLuint>> indices;
  // calculate the bounding box
  void calcBB();
  // initialize the opengl buffers
  void initBufferObjects();

private:
  // workaround for missing OpenGL support under Windows
  QGLFunctions gl;

  // informations for the object's bounding box
  QVector3D center;
  QVector3D minPos;
  QVector3D maxPos;

  // the tranformation matrix
  QMatrix4x4 trafoMatrix;

  // reorder the elements, so that everything complies with the OpenGL format
  void reorder();
};

#endif // MODEL_H
