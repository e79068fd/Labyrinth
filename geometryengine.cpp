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

    QVector3D( 1.0f,  1.0f,  0.0f),
    QVector3D(-1.0f,  1.0f,  0.0f),
    QVector3D( 1.0f, -1.0f,  0.0f)
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
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
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

        QVector3D( 1.0f,  1.0f,  1.0f), // v3
        QVector3D(-1.0f,  1.0f,  1.0f), // v2
        QVector3D( 1.0f, -1.0f,  1.0f), // v1

        // Vertex data for face 1
        QVector3D( 1.0f, -1.0f,  1.0f), // v4
        QVector3D( 1.0f, -1.0f, -1.0f), // v5
        QVector3D( 1.0f,  1.0f,  1.0f), // v6

        QVector3D( 1.0f,  1.0f, -1.0f), // v7
        QVector3D( 1.0f,  1.0f,  1.0f), // v6
        QVector3D( 1.0f, -1.0f, -1.0f), // v5

        // Vertex data for face 2
        QVector3D( 1.0f, -1.0f, -1.0f), // v8
        QVector3D(-1.0f, -1.0f, -1.0f), // v9
        QVector3D( 1.0f,  1.0f, -1.0f), // v10

        QVector3D(-1.0f,  1.0f, -1.0f), // v11
        QVector3D( 1.0f,  1.0f, -1.0f), // v10
        QVector3D(-1.0f, -1.0f, -1.0f), // v9

        // Vertex data for face 3
        QVector3D(-1.0f, -1.0f, -1.0f), // v12
        QVector3D(-1.0f, -1.0f,  1.0f), // v13
        QVector3D(-1.0f,  1.0f, -1.0f), // v14

        QVector3D(-1.0f,  1.0f,  1.0f), // v15
        QVector3D(-1.0f,  1.0f, -1.0f), // v14
        QVector3D(-1.0f, -1.0f,  1.0f), // v13

        // Vertex data for face 4
        QVector3D(-1.0f, -1.0f, -1.0f), // v16
        QVector3D( 1.0f, -1.0f, -1.0f), // v17
        QVector3D(-1.0f, -1.0f,  1.0f), // v18

        QVector3D( 1.0f, -1.0f,  1.0f), // v19
        QVector3D(-1.0f, -1.0f,  1.0f), // v18
        QVector3D( 1.0f, -1.0f, -1.0f), // v17

        // Vertex data for face 5
        QVector3D(-1.0f,  1.0f,  1.0f), // v20
        QVector3D( 1.0f,  1.0f,  1.0f), // v21
        QVector3D(-1.0f,  1.0f, -1.0f), // v22

        QVector3D( 1.0f,  1.0f, -1.0f), // v23
        QVector3D(-1.0f,  1.0f, -1.0f), // v22
        QVector3D( 1.0f,  1.0f,  1.0f)  // v21
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


SphereDrawObject::SphereDrawObject(unsigned int num) {
    num = qMin((unsigned int)5, num);
    int cur = 0;
    QVector<QVector3D> container[2];
    container[0].push_back(QVector3D(0, 1, 0));
    container[0].push_back(QVector3D(0, 0, 1));
    container[0].push_back(QVector3D(1, 0, 0));

    container[0].push_back(QVector3D(0, 1,  0));
    container[0].push_back(QVector3D(1, 0,  0));
    container[0].push_back(QVector3D(0, 0, -1));

    container[0].push_back(QVector3D( 0, 1,  0));
    container[0].push_back(QVector3D( 0, 0, -1));
    container[0].push_back(QVector3D(-1, 0,  0));

    container[0].push_back(QVector3D( 0, 1, 0));
    container[0].push_back(QVector3D(-1, 0, 0));
    container[0].push_back(QVector3D( 0, 0, 1));

    container[0].push_back(QVector3D(0, -1, 0));
    container[0].push_back(QVector3D(1,  0, 0));
    container[0].push_back(QVector3D(0,  0, 1));

    container[0].push_back(QVector3D(0, -1,  0));
    container[0].push_back(QVector3D(0,  0, -1));
    container[0].push_back(QVector3D(1,  0,  0));

    container[0].push_back(QVector3D( 0, -1,  0));
    container[0].push_back(QVector3D(-1,  0,  0));
    container[0].push_back(QVector3D( 0,  0, -1));

    container[0].push_back(QVector3D( 0, -1, 0));
    container[0].push_back(QVector3D( 0,  0, 1));
    container[0].push_back(QVector3D(-1,  0, 0));

    numVertex = container[cur].size();
    coordinateBuf.bind();
    coordinateBuf.allocate(container[cur].data(), numVertex * sizeof(QVector3D));

    // Allocate normal
    QVector<QVector3D> normals;
    for(int i = 0; i < numVertex; i += 3) {
        QVector3D triangleNormal = QVector3D::normal(container[cur][i], container[cur][i + 1], container[cur][i + 2]);
        normals.push_back(triangleNormal);
        normals.push_back(triangleNormal);
        normals.push_back(triangleNormal);
    }

    normalBuf.bind();
    normalBuf.allocate(normals.data(), numVertex * sizeof(QVector3D));

    QVector<GLushort> indeces;
    for(int i = 0; i < numVertex; i++)
        indeces.push_back(i);

    indexBuf.bind();
    indexBuf.allocate(indeces.data(), numVertex * sizeof(GLushort));
}

SphereDrawObject::~SphereDrawObject() {

}

void SphereDrawObject::draw(QOpenGLShaderProgram *program) {
    coordinateBuf.bind();
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    normalBuf.bind();
    int normalLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normalLocation);
    program->setAttributeBuffer(normalLocation, GL_FLOAT, 0, 3, sizeof(QVector3D));

    if(false && isTextured) { //TODO add textrured
        texcoordBuf.bind();
        int texcoordLocation = program->attributeLocation("a_texcoord");
        program->enableAttributeArray(texcoordLocation);
        program->setAttributeBuffer(texcoordLocation, GL_FLOAT, 0, 2, sizeof(QVector2D));
    }

    indexBuf.bind();
    glDrawElements(GL_TRIANGLES, numVertex, GL_UNSIGNED_SHORT, 0);
}

void SphereDrawObject::setTexture(QVector2D* texcoords) {
    //TODO add textrured
}
