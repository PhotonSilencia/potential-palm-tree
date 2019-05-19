#ifndef CGFUNCTIONS_H
#define CGFUNCTIONS_H

#include <QMatrix4x4>
#include <QVector4D>

// 2 helper functions for the object picking
QVector4D normalizedViewToWorld(QVector4D p, QMatrix4x4 &imvpMatrix) {
  QVector4D q = imvpMatrix * p;
  return q / q.w(); // homogenize
}

QVector4D unprojectScreenCoordinates(int x, int y, float depth, int width,
                                     int height, QMatrix4x4 imvpMatrix) {
  // unproject the mouse points
  float u = ((float)x + 0.5f) / (float)width * 2.0f - 1.0f;
  float v = ((float)y + 0.5f) / (float)height * 2.0f - 1.0f;
  // opengl and qt have a diiferent definition of "up" and "down"
  v *= -1.0;
  return normalizedViewToWorld(QVector4D(u, v, depth, 1.0f), imvpMatrix);
}

bool intersectBox(QVector4D r_o, QVector4D r_d, QVector4D bbmin,
                  QVector4D bbmax, float *tnear, float *tfar) {
  // compute intersection of ray with all six bbox planes
  QVector4D invR =
      QVector4D(1.0 / r_d.x(), 1.0 / r_d.y(), 1.0 / r_d.z(), 1.0 / r_d.w());
  QVector4D tbot = invR * (bbmin - r_o);
  QVector4D ttop = invR * (bbmax - r_o);

  // re-order intersections to find smallest and largest on each axis
  QVector4D tmin =
      QVector4D(std::min(ttop.x(), tbot.x()), std::min(ttop.y(), tbot.y()),
                std::min(ttop.z(), tbot.z()), std::min(ttop.w(), tbot.w()));
  QVector4D tmax =
      QVector4D(std::max(ttop.x(), tbot.x()), std::max(ttop.y(), tbot.y()),
                std::max(ttop.z(), tbot.z()), std::max(ttop.w(), tbot.w()));

  // find the largest tmin and the smallest tmax
  float largest_tmin = std::max(std::max(tmin.x(), tmin.y()), tmin.z());
  float smallest_tmax = std::min(std::min(tmax.x(), tmax.y()), tmax.z());

  *tnear = largest_tmin;
  *tfar = smallest_tmax;

  return smallest_tmax > largest_tmin;
}

#endif // CGFUNCTIONS_H
