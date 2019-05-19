#ifndef SCENE_H
#define SCENE_H

#include <memory>

#include <QMouseEvent>
#include <QOpenGLBuffer>
#include <QOpenGLFramebufferObject>
#include <QOpenGLVertexArrayObject>
#include <QTime>
#include <QTimer>
#include <QVector4D>
#include <QWheelEvent>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/qmatrix4x4.h>
#include <QtOpenGL/QGLWidget>
#include "Skybox.h"
#include "Sphere.h"

class Model;
struct Material;
class Light;

class Scene : public QGLWidget {
  Q_OBJECT
public:
  Scene(QWidget *parent = 0);
  ~Scene();

  // these functions are called by the GUI, when the user selects the
  // corresponding menu entry
  void scaleModel(double sx, double sy, double sz);
  void translateModel(double dx, double dy, double dz);
  void rotateModel(double ax, double ay, double az, double angle);
  void deleteModel();
  bool loadModelFromOBJFile(QString path);
  void addLight();

  void saveScene(QString filepath);
  void loadScene(QString filepath);

protected:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();

public slots:
  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);
  void reloadShader();
  void resetScene();
  void setFloor();

  void triangleInit();

protected slots:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

private:
  std::shared_ptr<QOpenGLShaderProgram>
  loadShaders(QString vertexShaderSource, QString fragmentShaderSource);

  // for camera movement
  int xRot;
  int yRot;
  int zRot;
  float zoom;
  float transx;
  float transy;
  QPoint lastPos;
  QVector4D lastDraggingPos;
  float draggingDepth;
  bool isDragging;
  bool mousepressed;
  void normalizeAngle(int *angle);

  QOpenGLVertexArrayObject vao;
  // show the floor
  bool showFloor;

  // the "main" Shader program
  std::shared_ptr<QOpenGLShaderProgram> m_program;
  // the transformation matrices
  QMatrix4x4 m_view, m_projection;
  // function to set the matrices according to camera movement
  void setTransformations();

  // the models in the scene
  std::vector<std::shared_ptr<Model>> m_models;

  // the lights in the scene
  std::vector<std::shared_ptr<Light>> m_lights;

  // the selected Model
  int m_selectedModel;

  // the timer to repaint the scene
  QTimer updateTimer;

  // the "selected model" Shader program
  std::shared_ptr<QOpenGLShaderProgram> s_program;

  // the skybox
  std::shared_ptr<Skybox> m_skybox;

  // the skybox shader program
  std::shared_ptr<QOpenGLShaderProgram> skybox_program;

  //the sphere
  std::shared_ptr<Sphere> m_sphere;

  //the sphere shader program
  std::shared_ptr<QOpenGLShaderProgram> sphere_program;

  // AUFGABE 1: hier sollten Sie die Attribute setzen und die Buffer deklarieren

  // position and color attributes
  const std::vector<QVector3D> positions = {
      QVector3D(-1.0f, -1.0f, 0.0f),
      QVector3D(1.0f, -1.0f, 0.0f),
      QVector3D(0.0f, 1.0f, 0.0f)
  };

  const std::vector<QVector3D> colors = {
      QVector3D(1.0f, 0.0f, 0.0f),
      QVector3D(0.0f, 1.0f, 0.0f),
      QVector3D(0.0f, 0.0f, 1.0f)
  };

  QOpenGLBuffer positionBuffer;
  QOpenGLBuffer colorBuffer;

  // AUFGABE 1: ENDE

  // to measure the time since the program started (can be used for animation)
  int frame;
};

#endif // SCENE_H
