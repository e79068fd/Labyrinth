#ifndef BOX_H
#define BOX_H

#include <QVector3D>
#include <QMatrix4x4>
#include <QColor>

class Box {
public:
    Box();
    Box(const QVector3D&, const QVector3D&, const QColor&);
    void setSize(const QVector3D&, const QVector3D&);
    const QMatrix4x4& getMatrix() const;
    void setColor(const QColor&);
    const QColor& getColor() const;
    const QPair<QVector3D, QVector3D>& getPoints() const;

private:
    QVector3D first, last;
    QMatrix4x4 matrix;
    QColor color;
};
#endif // BOX_H
