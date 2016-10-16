#include "box.h"

Box::Box()
{

}

Box::Box(const QVector3D& first, const QVector3D& last, const QColor& color) {
    setSize(first, last);
    setColor(color);
}

void Box::setSize(const QVector3D& f, const QVector3D& l) {
    first = f; last = l;
    QVector3D diff = (last - first) / 2;
    matrix.setToIdentity();
    matrix.translate(last - diff);
    matrix.scale(diff);
}

const QMatrix4x4& Box::getMatrix() const {
    return this->matrix;
}

void Box::setColor(const QColor& color) {
    this->color = color;
}

const QColor& Box::getColor() const {
    return this->color;
}

const QPair<QVector3D, QVector3D>& Box::getPoints() const {
    return QPair<QVector3D, QVector3D>(first, last);
}
