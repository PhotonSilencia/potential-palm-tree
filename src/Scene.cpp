#include <cmath>

#include <QMessageBox>
#include <fstream>

#include "CGFunctions.h"
#include "EditWidgets.h"
#include "Light.h"
#include "Model.h"
#include "Scene.h"
#include "Skybox.h"


inline void OpenGLError() {
  GLenum errCode;
  if ((errCode = glGetError()) != GL_NO_ERROR) {
    qDebug() << "OpenGL Error: \n" << errCode << endl;
  }
}

Scene::Scene(QWidget *parent) : QGLWidget(parent) {
  m_program = nullptr;
  m_skybox = nullptr;

  mousepressed = false;
  isDragging = false;

  frame = 0;

  Model::nameCount = 10;
  m_selectedModel = -1;

  showFloor = true;

  // start the update Timer (30fps)
  connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
  updateTimer.start(33);

  OpenGLError();
}

void Scene::resetScene() {
  // delete lights and models
  m_models.clear();
  m_lights.clear();
  m_selectedModel = -1;
  // reset camera
  xRot = 0;
  yRot = 0;
  zRot = 0;
  zoom = -10.0f;
  transx = 0;
  transy = -2.0;
  // load plane
  loadModelFromOBJFile(QString("plane/plane.obj"));
  m_models.back()->rotate(QVector3D(1.0, 0.0, 0.0), -90);
  m_models.back()->scale(10, 0, 10);
}

Scene::~Scene() {
  if (m_program) {
    m_program = 0;
  }
}

void Scene::saveScene(QString filepath) {
  // create File
  std::ofstream file(filepath.toStdString().c_str());

  // 1. write header
  file << m_models.size() - 1 - m_lights.size() << " " << m_lights.size()
       << "\n";
  file << xRot << " " << yRot << " " << zRot << " "
       << " " << zoom << " " << transx << " " << transy;
  file << "\n";

  // 2. save Models
  for (size_t m = 1; m < m_models.size(); ++m) {
    std::shared_ptr<Light> l = std::dynamic_pointer_cast<Light>(m_models[m]);
    if (l)
      continue;
    file << m_models[m]->getPath().toStdString() << "\n";
    QMatrix4x4 trafo = m_models[m]->getTransformations();
    for (size_t i = 0; i < 4; ++i)
      for (size_t j = 0; j < 4; ++j)
        file << trafo(i, j) << " ";
    file << "\n";
  }
  file << "\n";

  // 3. save lights
  for (size_t l = 0; l < m_lights.size(); ++l) {
    Light *light = m_lights[l].get();
    QVector3D a = light->getAmbient();
    QVector3D d = light->getDiffuse();
    QVector3D s = light->getSpecular();
    file << a.x() << " " << a.y() << " " << a.z() << "\n";
    file << d.x() << " " << d.y() << " " << d.z() << "\n";
    file << s.x() << " " << s.y() << " " << s.z() << "\n";
    QMatrix4x4 trafo = m_lights[l]->getTransformations();
    for (size_t i = 0; i < 4; ++i)
      for (size_t j = 0; j < 4; ++j)
        file << trafo(i, j) << " ";
    file << "\n";
  }
}

void Scene::loadScene(QString filepath) {
  std::ifstream file(filepath.toStdString().c_str());
  size_t nModels, nLights;
  file >> nModels >> nLights;
  file >> xRot >> yRot >> zRot >> zoom >> transx >> transy;
  float m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42,
      m43, m44;
  // read models
  for (size_t m = 0; m < nModels; ++m) {
    std::string path;
    file >> path;
    file >> m11 >> m12 >> m13 >> m14 >> m21 >> m22 >> m23 >> m24 >> m31 >>
        m32 >> m33 >> m34 >> m41 >> m42 >> m43 >> m44;
    loadModelFromOBJFile(QString(path.c_str()));
    QMatrix4x4 trafo = QMatrix4x4(m11, m12, m13, m14, m21, m22, m23, m24, m31,
                                  m32, m33, m34, m41, m42, m43, m44);
    m_models.back()->setTransformations(trafo);
  }
  // read lights
  for (size_t l = 0; l < nLights; ++l) {
    double ar, ag, ab, dr, dg, db, sr, sg, sb;
    file >> ar >> ag >> ab >> dr >> dg >> db >> sr >> sg >> sb;
    file >> m11 >> m12 >> m13 >> m14 >> m21 >> m22 >> m23 >> m24 >> m31 >>
        m32 >> m33 >> m34 >> m41 >> m42 >> m43 >> m44;
    QMatrix4x4 trafo = QMatrix4x4(m11, m12, m13, m14, m21, m22, m23, m24, m31,
                                  m32, m33, m34, m41, m42, m43, m44);
    auto light = std::shared_ptr<Light>(new Light(Model::nameCount++));
    light->setAmbient(ar, ag, ab);
    ;
    light->setDiffuse(dr, dg, db);
    light->setSpecular(sr, sg, sb);
    light->setTransformations(trafo);
    m_lights.push_back(light);
    m_models.push_back(m_lights.back());
  }
}

void Scene::deleteModel() {
  if (m_selectedModel == -1)
    return;

  updateTimer.stop();

  // check, if model is also a light
  std::shared_ptr<Light> l =
      std::dynamic_pointer_cast<Light>(m_models[m_selectedModel]);
  if (l) {
    // delete the corresponding Light
    int name = m_models[m_selectedModel]->getName();
    int lightIndex = -1;
    for (size_t i = 0; i < m_lights.size(); ++i) {
      if (m_lights[i]->getName() == name)
        lightIndex = i;
    }
    if (lightIndex >= 0) {
      std::vector<std::shared_ptr<Light>>::iterator it = m_lights.begin();
      m_lights.erase(it + lightIndex);
    } else
      qDebug() << "Strange error: Did not find light to erase.\n";
  }

  // delete the model
  std::vector<std::shared_ptr<Model>>::iterator it = m_models.begin();
  m_models.erase(it + m_selectedModel);

  // unselect Model
  m_selectedModel = -1;

  updateTimer.start(33);
}

bool Scene::loadModelFromOBJFile(QString path) {
  m_models.push_back(std::shared_ptr<Model>(Model::importOBJFile(path)));
  if (m_models.back()->getNpositions() == 0) {
    QMessageBox::warning(this, QString("Error"),
                         QString("The file could not be opened."));
    return false;
  }
  m_models.back()->rotate(QVector3D(1.0, 0.0, 0.0), -90);
  return true;
}

void Scene::scaleModel(double sx, double sy, double sz) {
  if (m_selectedModel == -1)
    return;
  m_models[m_selectedModel]->scale(sx, sy, sz);
}

void Scene::translateModel(double dx, double dy, double dz) {
  if (m_selectedModel == -1)
    return;
  m_models[m_selectedModel]->translate(dx, dy, dz);
}

void Scene::rotateModel(double ax, double ay, double az, double angle) {
  if (m_selectedModel == -1)
    return;
  m_models[m_selectedModel]->rotate(QVector3D(ax, ay, az), angle);
}

void Scene::addLight() {
  auto l = std::shared_ptr<Light>(new Light(Model::nameCount++));
  LightDialog ld(l.get());
  if (ld.exec()) {
    m_lights.push_back(l);
    m_models.push_back(m_lights.back());
  }
}

std::shared_ptr<QOpenGLShaderProgram>
Scene::loadShaders(QString vertexShaderSource, QString fragmentShaderSource) {
  auto pr = std::make_shared<QOpenGLShaderProgram>();
  pr->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderSource);
  qDebug() << "Compile VertexShader: ";
  qDebug() << pr->log();

  pr->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderSource);
  qDebug() << "Compile FragmentShader: ";
  qDebug() << pr->log();

  pr->link();
  qDebug() << "Linking Shader Program: ";
  qDebug() << pr->log();

  return pr;
}

void Scene::reloadShader() {
  m_program = loadShaders(QString("shader/vertex.glsl"),
                          QString("shader/fragment.glsl"));
  s_program = loadShaders(QString("shader/vertex.glsl"),
                          QString("shader/selectedfragment.glsl"));
  skybox_program = loadShaders(QString("shader/skyboxvertex.glsl"),
                               QString("shader/skyboxfragment.glsl"));
  sphere_program = loadShaders(QString("shader/spherevertex.glsl"),
                               QString("shader/spherefragment.glsl"));
}

void Scene::setFloor() { showFloor = !showFloor; }

void Scene::initializeGL() {

  vao.create();
  vao.bind();

  glClearColor(1.0, 1.0, 1.0, 0.0);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  reloadShader();

  resetScene();

  OpenGLError();  

}

void Scene::triangleInit() {

  // AUFGABE 1: erzeugen Sie hier die Buffer und initialisieren sie alles
  // notwendige

    positionBuffer = QOpenGLBuffer( QOpenGLBuffer :: VertexBuffer );
    positionBuffer.setUsagePattern( QOpenGLBuffer :: StaticDraw);
    positionBuffer.create();
    positionBuffer.bind();
    positionBuffer.allocate( &positions[0], positions.size()*sizeof(QVector3D));
    positionBuffer.release();

    colorBuffer = QOpenGLBuffer( QOpenGLBuffer :: VertexBuffer );
    colorBuffer.setUsagePattern( QOpenGLBuffer :: StaticDraw);
    colorBuffer.create();
    colorBuffer.bind();
    colorBuffer.allocate( &colors[0], colors.size()*sizeof(QVector3D));
    colorBuffer.release();
}

void Scene::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
  qreal aspect = qreal(width) / qreal(height ? height : 1);
  const qreal zNear = 0.1f, zFar = 400.0f, fov = 60.0f;
  m_projection.setToIdentity();
  m_projection.perspective(fov, aspect, zNear, zFar);
}

void Scene::mouseMoveEvent(QMouseEvent *event) {
  if (lastPos.x() == -1 || lastPos.y() == -1)
    return;

  int dx = event->x() - lastPos.x();
  int dy = event->y() - lastPos.y();

  if (event->buttons() & Qt::LeftButton && m_selectedModel == -1) {
    setXRotation(xRot + 4 * dy);
    setYRotation(yRot + 4 * dx);
  } else if (event->buttons() & Qt::RightButton) {
    setXRotation(xRot + 4 * dy);
    setZRotation(zRot - 4 * dx);
  } else if (event->buttons() & Qt::MidButton) {
    transx += (double)dx * 0.01;
    transy -= (double)dy * 0.01;
  } else if (event->buttons() & Qt::LeftButton &&
             m_selectedModel != -1) // move the selected model
  {
    QMatrix4x4 imvpMatrix = (m_projection * m_view).inverted();
    QVector4D center = m_models[m_selectedModel]->getBoundingBox().center;
    // determine the last dragging position
    if (!isDragging) // init with bounding box
    {
      // project the center to view (so that we look in z direction -> z
      // is now the depth)
      QVector4D centerView = m_view * center;
      double z = centerView.z();
      // calculate the depth buffer value of z
      draggingDepth = 2.0 * (z - 0.1) / (1000.0 - 0.1) - 1.0;
      lastDraggingPos = unprojectScreenCoordinates(
          event->x(), event->y(), draggingDepth, width(), height(), imvpMatrix);
      isDragging = true;
    }
    QVector4D draggingPos = unprojectScreenCoordinates(
        event->x(), event->y(), draggingDepth, width(), height(), imvpMatrix);
    QVector4D translation = draggingPos - lastDraggingPos;
    float factor = fabs(zoom * 10);
    m_models[m_selectedModel]->translate(translation.x() * factor,
                                         translation.y() * factor,
                                         translation.z() * factor);
    lastDraggingPos = draggingPos;
  }

  lastPos = event->pos();
}

void Scene::wheelEvent(QWheelEvent *event) { zoom -= event->delta() * 0.0025; }

void Scene::mousePressEvent(QMouseEvent *event) {
  mousepressed = true;
  lastPos = event->pos();
}

void Scene::mouseDoubleClickEvent(QMouseEvent *event) {
  // intersect ray with the bounding boxes of all models
  // the functions for this technique are defined in CGFunctions.h

  // calculate intersections of ray in world space
  QMatrix4x4 imvpMatrix = (m_projection * m_view).inverted();
  QVector4D eyeRay_n = unprojectScreenCoordinates(
      event->x(), event->y(), -1.0, width(), height(), imvpMatrix);
  QVector4D eyeRay_z = unprojectScreenCoordinates(
      event->x(), event->y(), 1.0, width(), height(), imvpMatrix);
  float tnear, tfar;
  float smallest_t = 1e33;
  int nearestModel = -1;
  for (size_t i = 0; i < m_models.size(); ++i) {
    BoundingBox bb = m_models[i]->getBoundingBox();
    if (intersectBox(eyeRay_n, eyeRay_z - eyeRay_n, bb.bbmin, bb.bbmax, &tnear,
                     &tfar)) {
      if (tnear < smallest_t) {
        smallest_t = fabs(tnear);
        nearestModel = i;
      }
    }
  }
  if (nearestModel >= 0 && (nearestModel != m_selectedModel)) {
    m_selectedModel = nearestModel;
    std::cout << "Model: " << m_models[m_selectedModel]->getName()
              << " was selected\n";
  } else
    m_selectedModel = -1;
}

void Scene::mouseReleaseEvent(QMouseEvent *event) {
  mousepressed = false;
  isDragging = false;
}

void Scene::setXRotation(int angle) {
  normalizeAngle(&angle);
  if (angle != xRot) {
    xRot = angle;
  }
}

void Scene::setYRotation(int angle) {
  normalizeAngle(&angle);
  if (angle != yRot) {
    yRot = angle;
  }
}

void Scene::setZRotation(int angle) {
  normalizeAngle(&angle);
  if (angle != zRot) {
    zRot = angle;
  }
}

void Scene::normalizeAngle(int *angle) {
  while (*angle < 0)
    *angle += 360 * 16;
  while (*angle > 360 * 16)
    *angle -= 360 * 16;
}

void Scene::setTransformations() {
  m_view = QMatrix4x4(); // init with idendity matrix
  // do the translation
  m_view.translate(transx, transy, zoom);
  // do the rotation
  m_view.rotate((xRot / 16.0f), 1.0f, 0.0f, 0.0f);
  m_view.rotate((yRot / 16.0f), 0.0f, 1.0f, 0.0f);
  m_view.rotate((zRot / 16.0f), 0.0f, 0.0f, 1.0f);
}


void Scene::paintGL() {
  ++frame;
  if (frame < 0)
    frame = 0;

  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // set the view matrix
  setTransformations();

  // Skybox
  if (m_skybox == nullptr){
      m_skybox = std::make_shared<Skybox>();
  }

  skybox_program->bind();

  skybox_program->setUniformValue("projection", m_projection);
  skybox_program->setUniformValue("view", m_view);

  m_skybox->render(skybox_program);

  skybox_program->release();

  //render all models

  // the floor is always the first model, so if (showFloor == false), we
  // simply start the rendering
  // with the second model

  size_t i;
  for (showFloor ? i = 0 : i = 1; i < m_models.size(); ++i) {

     m_program->bind();

     QMatrix4x4 modelMatrix = m_models[i]->getTransformations();

     m_program->setUniformValue("projection", m_projection);
     m_program->setUniformValue("view", m_view);
     m_program->setUniformValue("model", modelMatrix);

     QVector3D lightPosition = m_models[i]->getBoundingBox().center;

     m_program->setUniformValue("lightPosition", lightPosition);

     QVector3D lightPositions[9];
     QVector3D lightAmbient[9];
     QVector3D lightDiffuse[9];
     QVector3D lightSpecular[9];

     for (size_t j = 0; j < m_lights.size() && j < 9; j++){

         lightPositions[j] = m_lights[j]->getBoundingBox().center;
         lightAmbient[j] = m_lights[j]->getAmbient();
         lightDiffuse[j] = m_lights[j]->getDiffuse();
         lightSpecular[j] = m_lights[j]->getSpecular();

     }

     m_program->setUniformValueArray("lightPositions", lightPositions, 9);
     m_program->setUniformValue("lightCount", (int)m_lights.size());
     m_program->setUniformValueArray("lightAmbient", lightAmbient, 9);
     m_program->setUniformValueArray("lightDiffuse", lightDiffuse, 9);
     m_program->setUniformValueArray("lightSpecular", lightSpecular, 9);

     m_program->setUniformValue("textureSampler", 0);

     // render the model
     m_models[i]->render(m_program);

     if (m_selectedModel >= 0) {
         s_program->bind();

         QMatrix4x4 s_modelMatrix = m_models[m_selectedModel]->getTransformations();

         s_program->setUniformValue("projection", m_projection);
         s_program->setUniformValue("view", m_view);
         s_program->setUniformValue("model", s_modelMatrix);

         m_models[m_selectedModel]->render(s_program);

         s_program->release();
     }

    // release shader the program
    m_program->release();
  }
  // Aufgabe 2

  QVector3D cameraPosition = (m_view.inverted() * QVector4D(0.0, 0.0, 0.0, 1.0)).toVector3DAffine();

  std::vector<std::shared_ptr<Sphere>> spheres {

      //std::make_shared<Sphere>(QVector3D(0.0, 0.0, 0.0), 10.0),
      //std::make_shared<Sphere>(QVector3D(10.0, 0.0, -10.0), 1.0),
      //std::make_shared<Sphere>(QVector3D(-10.0, -10.0, -10.0), 1.0),
      std::make_shared<Sphere>(QVector3D(0.0, 0.0, 0.0), 1.0)

  };

  sphere_program->bind();

  sphere_program->setUniformValue("projection", m_projection);
  sphere_program->setUniformValue("view", m_view);
  sphere_program->setUniformValue("cameraPosition", cameraPosition);

  //m_skybox->bindTexture();

  for(auto&& sphere: spheres){
      sphere->render(sphere_program);
  }

  sphere_program->release();
}
