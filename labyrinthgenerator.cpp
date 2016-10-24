#include "labyrinthgenerator.h"

LabyrinthGenerator::LabyrinthGenerator() {

}

void LabyrinthGenerator::generate() {
    Preferences* pref = Preferences::instance();
    width = pref->take("width").toInt(); length = pref->take("length").toInt();
    srand(time(0));
    QVector<int> filler;
    filler.fill(Wall, length * 2 + 1);
    labyrinthMap.fill(filler, width * 2 + 1);
    labyrinthMap[1][1] = true;
    dfs(QPoint(1, 1));
}

void LabyrinthGenerator::matchLabyrinth(Labyrinth *labyrinth) {
    for(int j = 0; j < length * 2 + 1; j++)
        labyrinthMap[0][j] = Checked;
    for(int j = 0; j < length * 2 + 1; j++)
        labyrinthMap[width * 2][j] = Checked;
    for(int i = 0; i < width * 2 + 1; i++)
        labyrinthMap[i][0] = Checked;
    for(int i = 0; i < width * 2 + 1; i++)
        labyrinthMap[i][length * 2] = Checked;
    qreal halfWidth = (width * 5.0 + width + 1) / 2, halfLength = (length *  5.0 + length + 0.5) / 2;
    labyrinth->addWall(QVector3D(halfLength, 0.5, halfWidth), QVector3D(0, -0.5, 0));
    labyrinth->addWall(QVector3D(halfLength, 0.5, halfWidth), QVector3D(0,  5.5, 0));

    labyrinth->addWall(QVector3D(0.5, 2.5, 0.5), QVector3D(-halfLength + 0.5, 2.5, -halfWidth + 0.5));
    labyrinth->addWall(QVector3D(0.5, 2.5, 0.5), QVector3D( halfLength - 0.5, 2.5, -halfWidth + 0.5));
    labyrinth->addWall(QVector3D(0.5, 2.5, 0.5), QVector3D( halfLength - 0.5, 2.5,  halfWidth - 0.5));
    labyrinth->addWall(QVector3D(0.5, 2.5, 0.5), QVector3D(-halfLength + 0.5, 2.5,  halfWidth - 0.5));

    labyrinth->addWall(QVector3D(halfLength - 1, 2.5,            0.5), QVector3D(               0, 2.5, -halfWidth + 0.5));
    labyrinth->addWall(QVector3D(           0.5, 2.5,  halfWidth - 1), QVector3D( halfLength - 0.5, 2.5,                 0));
    labyrinth->addWall(QVector3D(halfLength - 1, 2.5,            0.5), QVector3D(               0, 2.5,  halfWidth - 0.5));
    labyrinth->addWall(QVector3D(           0.5, 2.5,  halfWidth - 1), QVector3D(-halfLength + 0.5, 2.5,                 0));

    labyrinth->addWallMask(0, 127 ^ BoxDrawObject::Top);
    labyrinth->addWallMask(1, 127 ^ BoxDrawObject::Bottom);
    labyrinth->addWallMask(6, BoxDrawObject::Back);
    labyrinth->addWallMask(7, BoxDrawObject::Right);
    labyrinth->addWallMask(8, BoxDrawObject::Front);
    labyrinth->addWallMask(9, BoxDrawObject::Left);

    qreal lowX = -halfLength + 1, lowZ = -halfWidth + 1;
    for(int i = 1; i < width * 2; i++) {
        for(int j = 1; j < length * 2; j++) {
            if(labyrinthMap[i][j] == Wall) {
                qreal highX, highZ;
                if(labyrinthMap[i + 1][j] == Wall) { //South
                    highX = lowX + 1; highZ = lowZ;
                    for(int k = i; labyrinthMap[k][j] == Wall; k++) {
                        highZ = (k % 2) ? highZ + 5 : highZ + 1;
                        labyrinthMap[k][j] = Checked;
                    }
                } else if(labyrinthMap[i][j + 1] == Wall) { //East
                    highX = lowX; highZ = lowZ + 1;
                    for(int k = j; labyrinthMap[i][k] == Wall; k++) {
                        highX = (k % 2) ? highX + 5 : highX + 1;
                        labyrinthMap[i][k] = Checked;
                    }
                } else { //Single
                    highX = (j % 2) ? lowX + 5 : lowX + 1;
                    highZ = (i % 2) ? lowZ + 5 : lowZ + 1;
                    labyrinthMap[i][j] = Checked;
                }
                qreal halfLengthX = (highX - lowX) / 2, halfLengthZ = (highZ - lowZ) / 2;
                labyrinth->addWall(QVector3D(halfLengthX, 2.5, halfLengthZ), QVector3D(highX - halfLengthX, 2.5, highZ - halfLengthZ));
            }
            lowX = (j % 2) ? lowX + 5 : lowX + 1;
        }
        lowX = -halfLength + 1;
        lowZ = (i % 2) ? lowZ + 5 : lowZ + 1;
    }

    labyrinth->addBall(QVector3D(-halfLength + 3, 2.5, -halfWidth + 3));
    labyrinth->addFinish(QVector3D(halfLength - 3, 2.5, halfWidth - 3));
}

bool LabyrinthGenerator::validate(const QPoint& v) {
    return v.x() > 0 && v.x() < 2 * length && v.y() > 0 && v.y() < 2 * width && labyrinthMap[v.y()][v.x()] == Wall;
}

void LabyrinthGenerator::dfs(const QPoint &v) {
    QVector<QPoint> path;

    for(int i = 0; i < numDir; i++) {
        QPoint u(v.x() + dx[i] * 2, v.y() + dy[i] * 2);
        if(validate(u))
            path.push_back(QPoint(dx[i], dy[i]));
    }

    while(!path.empty()) {
        int i = rand() % path.size();
        QPoint u(v.x() + path[i].x() * 2, v.y() + path[i].y() * 2);
        if(validate(u)) {
            labyrinthMap[u.y() - path[i].y()][u.x() - path[i].x()] = Floor;
            labyrinthMap[u.y()][u.x()] = Floor;
            dfs(u);
        }
        path.remove(i);
    }
}
