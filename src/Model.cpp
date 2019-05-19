#include "Model.h"
#include <QMessageBox>
#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

int Model::nameCount;

std::shared_ptr<Model> Model::importOBJFile(QString fileName) {
  ifstream file;
  file.open(fileName.toStdString().c_str());

  if (!file)
    return std::make_shared<Model>();

  vector<QVector3D> positions;
  vector<Face> faces;
  vector<QVector3D> normals;
  vector<Mesh> meshes;
  vector<Material> materials;
  vector<QVector2D> tex_coords;
  while (!file.eof()) {
    string s;
    file >> s;
    // materials
    if (s.compare("mtllib") == 0) {
      string m_fn;
      file >> m_fn;
      QString path; // path in were the
      path = fileName;
      path.chop(fileName.length() - fileName.lastIndexOf("/"));
      path.append(QString("/%1").arg(QString(m_fn.c_str())));
      readMTLFile(path, materials);
    }
    // vertex
    if (s.compare("v") == 0) {
      float vx, vy, vz;
      file >> vx >> vy >> vz;
      positions.push_back(QVector3D(vx, vy, vz));
    }
    // tex coords
    if (s.compare("vt") == 0) {
      float u, v;
      file >> u >> v;
      tex_coords.push_back(QVector2D(u, v));
    }
    // normals
    if (s.compare("vn") == 0) {
      float vx, vy, vz;
      file >> vx >> vy >> vz;
      normals.push_back(QVector3D(vx, vy, vz));
    }
    // Mesh - characterized by the same material
    if (s.compare("usemtl") == 0) {
      string matName;
      file >> matName;
      // look what index should be used
      Mesh m;
      int index = 0;
      for (uint i = 1; i < materials.size(); ++i) {
        if (materials[i].name.compare(matName) == 0)
          index = i;
      }
      m.material = index;

      meshes.push_back(m);
    }
    // face (int our case: TRIANGLES)
    if (s.compare("f") == 0) {
      string v1, v2, v3;
      // the programm assumes we only read triangles! Otherwise holes will
      // occur
      file >> v1 >> v2 >> v3;
      // decompose the string of the form v/vt/vn
      QString str1(v1.c_str());
      QString str2(v2.c_str());
      QString str3(v3.c_str());
      QStringList list1 = str1.split("/");
      QStringList list2 = str2.split("/");
      QStringList list3 = str3.split("/");
      if (list1.size() > 0 && list2.size() > 0 && list3.size() > 0) {
        Face f;
        // save the vertices
        f.positions.push_back(list1[0].toInt() - 1);
        f.positions.push_back(list2[0].toInt() - 1);
        f.positions.push_back(list3[0].toInt() - 1);
        // save the tex_coords TODO: check if it is necessary to save
        // them
        if (list1.size() < 3 || list2.size() < 3 || list3.size() < 3) {
          f.normals.push_back(list1[1].toInt() - 1);
          f.normals.push_back(list2[1].toInt() - 1);
          f.normals.push_back(list3[1].toInt() - 1);
        } else {
          f.tex_coords.push_back(list1[1].toInt() - 1);
          f.tex_coords.push_back(list2[1].toInt() - 1);
          f.tex_coords.push_back(list3[1].toInt() - 1);
          f.normals.push_back(list1[2].toInt() - 1);
          f.normals.push_back(list2[2].toInt() - 1);
          f.normals.push_back(list3[2].toInt() - 1);
        }
        faces.push_back(f);
        meshes.back().faces.push_back(faces.size() - 1);
      }
    }
  }

  // give at least on (grey) standard material
  if (materials.empty()) {
    qDebug() << "No Materials found creating dummy...\n";
    Material m;
    m.name = string("dummy");
    // for consistency: init the other values
    m.ambient.setX(0.2);
    m.ambient.setY(0.2);
    m.ambient.setZ(0.2);
    m.diffuse.setX(0.6);
    m.diffuse.setY(0.6);
    m.diffuse.setZ(0.6);
    m.specular.setX(1.0);
    m.specular.setY(1.0);
    m.specular.setZ(1.0);
    m.shininess = 100.0;
    m.alpha = 1.0;
    m.density = 1.0;
    m.isTexture = false;
    materials.push_back(m);
  }
  // a sanity check (can happen that the model is not exported correctly)
  bool hasTexture = false;
  for (unsigned int i = 0; i < materials.size(); ++i) {
    if (materials[i].isTexture)
      hasTexture = true;
  }
  if (hasTexture && tex_coords.empty()) {
    QMessageBox msgBox;
    msgBox.setText("Warning! Model has Texture(s) but no Texture Coordinates.");
    msgBox.setInformativeText("Proceed? (will most likely result in a crash)");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Cancel)
      return std::make_shared<Model>();
  }
  qDebug() << "New Model: \n";
  qDebug() << materials.size() << " Materials " << meshes.size() << " Meshes "
           << positions.size() << " Vertices, " << faces.size() << " Faces "
           << normals.size() << " Normals " << tex_coords.size()
           << " TexCoords\n";

  auto m = std::make_shared<Model>(fileName, Model::nameCount++, meshes, faces,
                                   materials, positions, normals, tex_coords);

  return m;
}

void Model::readMTLFile(QString fileName, vector<Material> &materials) {
  // vector <Material> materials;
  ifstream file;
  file.open(fileName.toStdString().c_str());
  if (!file)
    return;
  while (!file.eof()) {
    string s;
    file >> s;
    // create a new material
    if (s.compare("newmtl") == 0) {
      string mtlName;
      file >> mtlName;
      Material m;
      m.name = mtlName;
      // for consistency: init the other values
      m.ambient.setX(0.5);
      m.ambient.setY(0.5);
      m.ambient.setZ(0.5);
      m.diffuse.setX(0.6);
      m.diffuse.setY(0.6);
      m.diffuse.setZ(0.6);
      m.specular.setX(1.0);
      m.specular.setY(1.0);
      m.specular.setZ(1.0);
      m.shininess = 20.0;
      m.alpha = 1.0;
      m.sharpness = 0.0;
      m.density = 1.0;
      m.isTexture = false;
      m.hasNormalMap = false;
      materials.push_back(m);
    }
    if (s.compare("Ns") == 0) {
      float ns;
      file >> ns;
      if (ns > 128.0)
        ns = 128.0;
      materials.back().shininess = ns;
    }
    if (s.compare("Ni") == 0) {
      float ni;
      file >> ni;
      materials.back().density = ni;
    }
    if (s.compare("Ka") == 0) {
      float x, y, z;
      file >> x >> y >> z;
      materials.back().ambient.setX(x);
      materials.back().ambient.setY(y);
      materials.back().ambient.setZ(z);
    }
    if (s.compare("Kd") == 0) {
      float x, y, z;
      file >> x >> y >> z;
      materials.back().diffuse.setX(x);
      materials.back().diffuse.setY(y);
      materials.back().diffuse.setZ(z);
    }
    if (s.compare("Ks") == 0) {
      float x, y, z;
      file >> x >> y >> z;
      materials.back().specular.setX(x);
      materials.back().specular.setY(y);
      materials.back().specular.setZ(z);
    }
    if (s.compare("d") == 0) {
      float d;
      file >> d;
      materials.back().alpha = d;
    }
    if (s.compare("sharpness") == 0) {
      float s;
      file >> s;
      materials.back().sharpness = s / 1000.0; // obj defines sharpness
                                               // from 0 - 1000 but we
                                               // need it from 0.0 to 1.0
    }

    // TODO: Textures are not completed
    // diffuse and ambient maps are treated the same way
    if (s.compare("map_Kd") == 0 || s.compare("map_Ka") == 0) {
      string tex_name;
      file >> tex_name;
      QImage *texture = NULL;
      QString path; // path in were the
      path = fileName;
      path.chop(fileName.length() - fileName.lastIndexOf("/"));
      path.append(QString("/%1").arg(QString(tex_name.c_str())));
      texture = new QImage(QString(path));
      if (!texture->isNull()) {
        QImage texImage = QGLWidget::convertToGLFormat(*texture);
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR); // Linear Filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR); // Linear Filtering
        if (texImage.hasAlphaChannel())
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texImage.width(),
                       texImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                       texImage.bits());
        else
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texImage.width(),
                       texImage.height(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                       texImage.bits());
        materials.back().isTexture = true;
        materials.back().texName = tex_name;
        materials.back().tex_id = tex;
        materials.back().texture = texImage;
        // cout << "Load TexID: " << materials.back().tex_id << endl;
        // cout << "Loaded color texture: " << path.toStdString() <<
        // endl;
      } else
        cout << "Warning: REAL Texture " << path.toStdString()
             << " couldn't be loaded!\n";
      // QMessageBox::warning(this, tr("Texture Loader"), QString("Texture
      // %1\ncouldn't be loaded!").arg(path),QMessageBox::Ok);
      texture->~QImage();
      texture = NULL;
    }

    // normal maps
    if (s.compare("map_bump") == 0 || s.compare("bump") == 0) {
      string bump_name;
      file >> bump_name;
      QImage *texture = NULL;
      QString path; // path in were the
      path = fileName;
      path.chop(fileName.length() - fileName.lastIndexOf("/"));
      path.append(QString("/%1").arg(QString(bump_name.c_str())));
      texture = new QImage(QString(path));
      if (!texture->isNull()) {
        QImage texImage = QGLWidget::convertToGLFormat(*texture);
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR); // Linear Filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR); // Linear Filtering
        if (texImage.hasAlphaChannel())
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texImage.width(),
                       texImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                       texImage.bits());
        else
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texImage.width(),
                       texImage.height(), 0, GL_RGB, GL_UNSIGNED_BYTE,
                       texImage.bits());
        materials.back().hasNormalMap = true;
        materials.back().normalMapName = bump_name;
        materials.back().normalMap_id = tex;
        materials.back().normalMap = texImage;
        // cout << "Load NormalID: " << materials.back().normalMap_id <<
        // endl;
        // cout << "Loaded Normal/Bump Map: " << path.toStdString()  <<
        // endl;
      } else
        cout << "Warning:NormalMap Texture " << path.toStdString()
             << " couldn't be loaded!\n";
      //    QMessageBox::warning(this, tr("Texture Loader"),
      //    QString("Texture %1\ncouldn't be
      //    loaded!").arg(path),QMessageBox::Ok);
      texture->~QImage();
      texture = NULL;
    }
  }
}

//----------------------------------------------------------------------------------

Model::Model(QString p, int na, vector<Mesh> me, vector<Face> fa,
             vector<Material> ma, vector<QVector3D> po, vector<QVector3D> no,
             vector<QVector2D> tc)
    : path(p), name(na), meshes(me), faces(fa), materials(ma), positions(po),
      normals(no), tex_coords(tc) {
  reorder();
  initBufferObjects();
  calcBB();
  gl.initializeGLFunctions(QGLContext::currentContext());
}

Model::Model() {}

Model::~Model() {
  positionBuffer.destroy();
  normalBuffer.destroy();
  texCoordBuffer.destroy();
  for (unsigned int i = 0; i < materials.size(); ++i) {
    if (materials[i].isTexture)
      glDeleteTextures(1, &(materials[i].tex_id));
    if (materials[i].hasNormalMap)
      glDeleteTextures(1, &(materials[i].normalMap_id));
  }
}

void Model::calcBB() {
  double minX, minY, minZ, maxX, maxY, maxZ;
  minX = maxX = positions[0].x();
  minY = maxY = positions[0].y();
  minZ = maxZ = positions[0].z();

  for (size_t i = 1; i < positions.size(); ++i) {
    minX = min((double)positions[i].x(), minX);
    maxX = max((double)positions[i].x(), maxX);
    minY = min((double)positions[i].y(), minY);
    maxY = max((double)positions[i].y(), maxY);
    minZ = min((double)positions[i].z(), minZ);
    maxZ = max((double)positions[i].z(), maxZ);
  }

  center =
      QVector3D(minX + fabs(maxX - minX) * 0.5, minY + fabs(maxY - minY) * 0.5,
                minZ + fabs(maxZ - minZ) * 0.5);
  minPos = QVector3D(minX, minY, minZ);
  maxPos = QVector3D(maxX, maxY, maxZ);
}

void Model::reorder() {
  // create new normals and texCoords to comply with opengl format (nVertices
  // = nNormals = nColors = nTexCoords...)
  vector<QVector3D> pos_new;
  pos_new.reserve(positions.size());
  vector<QVector3D> norm_new;
  norm_new.reserve(positions.size());
  vector<QVector2D> tex_new;
  tex_new.reserve(positions.size());
  GLuint idx = 0;
  for (size_t i = 0; i < meshes.size(); ++i) {
    vector<GLuint> p;
    for (size_t j = 0; j < meshes[i].faces.size(); ++j) {
      for (size_t k = 0; k < faces[meshes[i].faces[j]].positions.size(); ++k) {
        p.push_back(idx++);
        pos_new.push_back(positions[faces[meshes[i].faces[j]].positions[k]]);
        norm_new.push_back(normals[faces[meshes[i].faces[j]].normals[k]]);
        if (materials[meshes[i].material].isTexture)
          tex_new.push_back(
              tex_coords[faces[meshes[i].faces[j]].tex_coords[k]]);
        else
          tex_new.push_back(QVector2D(-1, -1));
      }
    }
    indices.push_back(p);
  }

  positions = pos_new;
  normals = norm_new;
  tex_coords = tex_new;
}

void Model::initBufferObjects() {
  // the position buffer
  positionBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
  positionBuffer.create();
  positionBuffer.bind();
  positionBuffer.allocate(&positions[0], positions.size() * sizeof(QVector3D));
  positionBuffer.release();
  // the normal buffer
  normalBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  normalBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
  normalBuffer.create();
  normalBuffer.bind();
  normalBuffer.allocate(&normals[0], normals.size() * sizeof(QVector3D));
  normalBuffer.release();
  // the texCoord buffer
  texCoordBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  texCoordBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
  texCoordBuffer.create();
  texCoordBuffer.bind();
  texCoordBuffer.allocate(&tex_coords[0],
                          tex_coords.size() * sizeof(QVector2D));
  texCoordBuffer.release();
  // the index buffers (every material has its own index buffer)
  for (size_t i = 0; i < indices.size(); ++i) {
    indexBuffers.push_back(QOpenGLBuffer(QOpenGLBuffer::IndexBuffer));
    indexBuffers.back().setUsagePattern(QOpenGLBuffer::StaticDraw);
    indexBuffers.back().create();
    indexBuffers.back().bind();
    indexBuffers.back().allocate(&indices[i][0],
                                 indices[i].size() * sizeof(GLuint));
    indexBuffers.back().release();
  }
}

BoundingBox Model::getBoundingBox() {
  BoundingBox bb;
  bb.center = trafoMatrix * center;
  bb.bbmin = trafoMatrix * minPos;
  bb.bbmax = trafoMatrix * maxPos;
  return bb;
}

QMatrix4x4 Model::getTransformations() { return trafoMatrix; }

void Model::setTransformations(QMatrix4x4 t) { trafoMatrix = t; }

QString Model::getPath() { return path; }

//---------------------------------------------------------------------------

void Model::rotate(QVector3D axis, float angle) {
  QMatrix4x4 r;
  r.rotate(angle, axis);
  trafoMatrix = r * trafoMatrix;
}

//----------------------------------------------------------------------------

void Model::translate(double dx, double dy, double dz) {
  QMatrix4x4 t;
  t.translate(dx, dy, dz);
  trafoMatrix = t * trafoMatrix;
}

//----------------------------------------------------------------------------

void Model::scale(double sx, double sy, double sz) {
  QMatrix4x4 s;
  s.scale(sx, sy, sz);
  trafoMatrix = s * trafoMatrix;
}

//----------------------------------------------------------------------------

int Model::getName() { return name; }

Mesh *Model::getMeshes() { return &meshes[0]; }

Face *Model::getFaces() { return &faces[0]; }

Material *Model::getMaterials() { return &materials[0]; }

QVector3D *Model::getNormals() { return &normals[0]; }

QVector3D *Model::getPositions() { return &positions[0]; }

QVector2D *Model::getTexCoords() { return &tex_coords[0]; }

size_t Model::getNmeshes() { return meshes.size(); }

size_t Model::getNfaces() { return faces.size(); }

size_t Model::getNmaterials() { return materials.size(); }

size_t Model::getNnormals() { return normals.size(); }

size_t Model::getNpositions() { return positions.size(); }

size_t Model::getNtexCoords() { return tex_coords.size(); }

//---------------------------------------------------------------------------------------

void Model::render(std::shared_ptr<QOpenGLShaderProgram> program) {

  int positionAttributeID, normalAttributeID, texCoordAttributeID;
  positionAttributeID = program->attributeLocation("position");
  normalAttributeID = program->attributeLocation("normal");
  texCoordAttributeID = program->attributeLocation("texCoords");

  // qDebug() <<  positionAttributeID << "  -  " << normalAttributeID << "  -
  // " << texCoordAttributeID;
  // enable the attributes
  program->enableAttributeArray(positionAttributeID);
  program->enableAttributeArray(normalAttributeID);
  program->enableAttributeArray(texCoordAttributeID);

  // set the geometry using the generated VBOs
  positionBuffer.bind();
  program->setAttributeBuffer(positionAttributeID, GL_FLOAT, 0, 3);
  positionBuffer.release();
  normalBuffer.bind();
  program->setAttributeBuffer(normalAttributeID, GL_FLOAT, 0, 3);
  normalBuffer.release();
  texCoordBuffer.bind();
  program->setAttributeBuffer(texCoordAttributeID, GL_FLOAT, 0, 2);
  texCoordBuffer.release();

  // give material to shader???
  //Material material;
  //program->setUniformValue("color", material);
  // ---------------------------------------

  for (size_t i = 0; i < meshes.size(); ++i) {
    program->setUniformValue("matAmbient",
                             materials[meshes[i].material].ambient);
    program->setUniformValue("matDiffuse",
                             materials[meshes[i].material].diffuse);
    program->setUniformValue("matSpecular",
                             materials[meshes[i].material].specular);
    program->setUniformValue("shine", materials[meshes[i].material].shininess);
    program->setUniformValue("alpha", materials[meshes[i].material].alpha);

    // set the texture (if there is any)
    if (materials[meshes[i].material].isTexture) {
      gl.glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, materials[meshes[i].material].tex_id);
      if (materials[meshes[i].material].hasNormalMap) {
        gl.glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,
                      materials[meshes[i].material].normalMap_id);
      }

    } else {
      //   qDebug() << Texture failed";
    }
    // draw the mesh
    indexBuffers[i].bind();
    glDrawElements(GL_TRIANGLES, indices[i].size(), GL_UNSIGNED_INT, 0);
    indexBuffers[i].release();
  }
}
