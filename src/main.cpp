#include "Mainwindow.h"
#include <QApplication>
#include <QGLFormat>

int main(int argc, char *argv[]) {

  QApplication a(argc, argv);


  QGLFormat glFormat;
  glFormat.setVersion(3, 3);
  glFormat.setProfile(QGLFormat::CoreProfile); // Requires >=Qt-4.8.0
  QGLFormat::setDefaultFormat(glFormat);


  MainWindow w;
  w.show();

  return a.exec();
}
