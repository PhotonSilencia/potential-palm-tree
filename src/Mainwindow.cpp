#include "Mainwindow.h"
#include "EditWidgets.h"
#include "Scene.h"
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

  scene = new Scene();
  setCentralWidget(scene);

  menuBar()->setNativeMenuBar(false);

  QMenu *file = new QMenu(QString("File"), this);
  file->addAction("Import OBJ-File", this, SLOT(loadOBJFile()),
                  Qt::CTRL + Qt::Key_I);
  file->addAction("Load Scene", this, SLOT(loadScene()), Qt::CTRL + Qt::Key_L);
  file->addAction("Save Scene", this, SLOT(saveScene()), Qt::CTRL + Qt::Key_S);
  menuBar()->addMenu(file);

  QMenu *sceneMenu = new QMenu(QString("Scene"), this);
  sceneMenu->addAction("Reset Scene", scene, SLOT(resetScene()));
  QAction *setFloor = sceneMenu->addAction(QString("Show Floor"));
  connect(setFloor, SIGNAL(changed()), scene, SLOT(setFloor()));
  setFloor->setCheckable(true);
  setFloor->setChecked(true);
  menuBar()->addMenu(sceneMenu);

  QMenu *options = new QMenu(QString("Edit"), this);
  options->addAction("Add Light", this, SLOT(addLight()));
  options->addAction("Scale Object", this, SLOT(scale()));
  options->addAction("Translate Object", this, SLOT(translate()));
  options->addAction("Rotate Object", this, SLOT(rotate()));
  options->addAction("Delete Object", this, SLOT(deleteObject()));
  options->addAction("Reload Shader", scene, SLOT(reloadShader()),
                     Qt::CTRL + Qt::Key_R);
  menuBar()->addMenu(options);

  statusBar()->showMessage(tr("Ready"));

  setWindowTitle(QString("CGViewer"));
  setGeometry(100, 100, 640, 480);
}

MainWindow::~MainWindow() {}

void MainWindow::loadOBJFile() {
  QString path;
  path = QFileDialog::getOpenFileName(this, "Choose a file to open",
                                      QString::null, QString::null);
  if (!path.isEmpty()) {
    if (scene->loadModelFromOBJFile(path))
      statusBar()->showMessage(tr("File Loaded"));
    else
      statusBar()->showMessage(tr("Error on Loading: Couldn't open OBJ-file!"));
  } else
    statusBar()->showMessage(tr("Loading aborted"));
}

void MainWindow::scale() {
  double sx, sy, sz;
  sx = sy = sz = 1.0;
  ScaleDialog sd(&sx, &sy, &sz);
  if (sd.exec())
    scene->scaleModel(sx, sy, sz);
}

void MainWindow::translate() {
  double dx, dy, dz;
  dx = dy = dz = 0.0;
  TranslateDialog td(&dx, &dy, &dz);
  if (td.exec())
    scene->translateModel(dx, dy, dz);
}

void MainWindow::rotate() {
  double ax, ay, az;
  ax = ay = az = 0.0;
  double angle = 0.0;
  RotateDialog rd(&ax, &ay, &az, &angle);
  if (rd.exec())
    scene->rotateModel(ax, ay, az, angle);
}

void MainWindow::deleteObject() { scene->deleteModel(); }

void MainWindow::addLight() { scene->addLight(); }

void MainWindow::saveScene() {
  QString path;
  path = QFileDialog::getSaveFileName(this, "Choose a file to save",
                                      QString::null, QString::null);
  if (!path.isEmpty()) {
    scene->saveScene(path);
    statusBar()->showMessage(tr("File Saved"));
  } else
    statusBar()->showMessage(tr("Saving aborted"));
}

void MainWindow::loadScene() {
  QString path;
  path = QFileDialog::getOpenFileName(this, "Choose a file to open",
                                      QString::null, QString::null);
  if (!path.isEmpty()) {
    scene->loadScene(path);
    statusBar()->showMessage(tr("File Loaded"));
  } else
    statusBar()->showMessage(tr("Loading aborted"));
}
