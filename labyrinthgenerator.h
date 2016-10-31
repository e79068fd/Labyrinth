#ifndef LABYRINTHGENERATOR_H
#define LABYRINTHGENERATOR_H

#include "labyrinth.h"
#include "geometryengine.h"
#include "preferences.h"

#include <QVector>

const int dx[] = { 0, -1, 0, 1};
const int dy[] = {-1,  0, 1, 0};
const int numDir = 4;

class LabyrinthGenerator {
public:
    LabyrinthGenerator();
    void generate();
    void fill(Labyrinth* labyrinth);

private:
    enum {WALL, FLOOR, CHECKED};
    QVector<QVector<int>> labyrinthMap;
    int width, length;
    bool validate(const QPoint& v);
    void dfs(const QPoint& current);
};

#endif // LABYRINTHGENERATOR_H
