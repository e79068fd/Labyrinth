#include "geometryengine.h"

AbstractDrawObject::AbstractDrawObject() : indexBuf(QOpenGLBuffer::IndexBuffer) {
    initializeOpenGLFunctions();

    //Create VBOs
    coordinateBuf.create();
    normalBuf.create();
    indexBuf.create();
    texcoordBuf.create();

    isTextured = false;
}

AbstractDrawObject::~AbstractDrawObject() {
    //Destroy VBOs
    coordinateBuf.destroy();
    normalBuf.destroy();
    indexBuf.destroy();
    texcoordBuf.destroy();
}

void AbstractDrawObject::setTextured(bool value) {
    isTextured = value;
}

bool AbstractDrawObject::getTextured() {
    return isTextured;
}

QVector3D defaultCoord[] = {
    QVector3D(-1.0f, -1.0f,  0.0f),
    QVector3D( 1.0f, -1.0f,  0.0f),
    QVector3D(-1.0f,  1.0f,  0.0f),

    QVector3D( 1.0f, -1.0f,  0.0f),
    QVector3D(-1.0f,  1.0f,  0.0f),
    QVector3D( 1.0f,  1.0f,  0.0f)
};
PlaneDrawObject::PlaneDrawObject() : PlaneDrawObject(defaultCoord) { }

PlaneDrawObject::PlaneDrawObject(QVector3D coordinates[]) : AbstractDrawObject() {
    coordinateBuf.bind();
    coordinateBuf.allocate(coordinates, 6 * sizeof(QVector3D));

    QVector3D normalForPlane = QVector3D::normal(coordinates[0], coordinates[1], coordinates[2]);

    QVector3D normals[] = {normalForPlane, normalForPlane, normalForPlane, normalForPlane, normalForPlane, normalForPlane};
    normalBuf.bind();
    normalBuf.allocate(normals, 6 * sizeof(QVector3D));

    GLushort indeces[] = {
        0, 1, 2, 3, 4, 5
    };
    indexBuf.bind();
    indexBuf.allocate(indeces, 6 * sizeof(GLushort));
}

PlaneDrawObject::~PlaneDrawObject() { }

void PlaneDrawObject::draw(QOpenGLShaderProgram *program) {
    coordinateBuf.bind();
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    normalBuf.bind();
    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    if(isTextured) {
        texcoordBuf.bind();
        int texcoordLocation = program->attributeLocation("a_texcoord");
        program->enableAttributeArray(texcoordLocation);
        program->setAttributeBuffer(texcoordLocation, GL_FLOAT, 0, 2, sizeof(QVector2D));
    }

    indexBuf.bind();
    glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, 0);
}

void PlaneDrawObject::setTexture(QVector2D* texcoords) {
    texcoordBuf.bind();
    texcoordBuf.allocate(texcoords, 4 * sizeof(QVector2D));
}

BoxDrawObject::BoxDrawObject() : AbstractDrawObject() {
    mask = 0;
    faces = new PlaneDrawObject*[6];
    QVector3D coordinates[] = {
        // Vertex data for face 0
        QVector3D(-1.0f, -1.0f,  1.0f), // v0
        QVector3D( 1.0f, -1.0f,  1.0f), // v1
        QVector3D(-1.0f,  1.0f,  1.0f), // v2

        QVector3D( 1.0f, -1.0f,  1.0f), // v1
        QVector3D(-1.0f,  1.0f,  1.0f), // v2
        QVector3D( 1.0f,  1.0f,  1.0f), // v3

        // Vertex data for face 1
        QVector3D( 1.0f, -1.0f,  1.0f), // v4
        QVector3D( 1.0f, -1.0f, -1.0f), // v5
        QVector3D( 1.0f,  1.0f,  1.0f), // v6

        QVector3D( 1.0f, -1.0f, -1.0f), // v5
        QVector3D( 1.0f,  1.0f,  1.0f), // v6
        QVector3D( 1.0f,  1.0f, -1.0f), // v7

        // Vertex data for face 2
        QVector3D( 1.0f, -1.0f, -1.0f), // v8
        QVector3D(-1.0f, -1.0f, -1.0f), // v9
        QVector3D( 1.0f,  1.0f, -1.0f), // v10

        QVector3D(-1.0f, -1.0f, -1.0f), // v9
        QVector3D( 1.0f,  1.0f, -1.0f), // v10
        QVector3D(-1.0f,  1.0f, -1.0f), // v11

        // Vertex data for face 3
        QVector3D(-1.0f, -1.0f, -1.0f), // v12
        QVector3D(-1.0f, -1.0f,  1.0f), // v13
        QVector3D(-1.0f,  1.0f, -1.0f), // v14

        QVector3D(-1.0f, -1.0f,  1.0f), // v13
        QVector3D(-1.0f,  1.0f, -1.0f), // v14
        QVector3D(-1.0f,  1.0f,  1.0f), // v15

        // Vertex data for face 4
        QVector3D(-1.0f, -1.0f, -1.0f), // v16
        QVector3D( 1.0f, -1.0f, -1.0f), // v17
        QVector3D(-1.0f, -1.0f,  1.0f), // v18

        QVector3D( 1.0f, -1.0f, -1.0f), // v17
        QVector3D(-1.0f, -1.0f,  1.0f), // v18
        QVector3D( 1.0f, -1.0f,  1.0f), // v19

        // Vertex data for face 5
        QVector3D(-1.0f,  1.0f,  1.0f), // v20
        QVector3D( 1.0f,  1.0f,  1.0f), // v21
        QVector3D(-1.0f,  1.0f, -1.0f), // v22

        QVector3D( 1.0f,  1.0f,  1.0f), // v21
        QVector3D(-1.0f,  1.0f, -1.0f), // v22
        QVector3D( 1.0f,  1.0f, -1.0f)  // v23
    };

    for(int i = 0; i < 6; i++)
        faces[i] = new PlaneDrawObject(coordinates + i * 6);
}

BoxDrawObject::~BoxDrawObject() { }

void BoxDrawObject::draw(QOpenGLShaderProgram *program) {
    for(int i = 0; i < 6; i++)
        if(!((1 << i) & mask))
            faces[i]->draw(program);
    mask = 0;
}

void BoxDrawObject::setTexture(QVector2D* texcoords) {
    for(int i = 0; i < 6; i++)
        faces[i]->setTexture(texcoords + i * 4);
}

void BoxDrawObject::setTextured(bool value) {
    for(int i = 0; i < 6; i++)
        faces[i]->setTextured(value);
    isTextured = value;
}

void BoxDrawObject::setMask(int m) {
    mask = m;
}
