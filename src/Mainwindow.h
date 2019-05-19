#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Scene;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void loadOBJFile();
  void saveScene();
  void loadScene();
  void addLight();
  void scale();
  void translate();
  void rotate();
  void deleteObject();

private:
  Scene *scene;
};

#endif // MAINWINDOW_H
