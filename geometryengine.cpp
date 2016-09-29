#include "geometryengine.h"

AbstractDrawObject::AbstractDrawObject() {
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

    //QVector3D first = QVector3D::normal(coordinates[0], coordinates[1], coordinates[2]);
    //QVector3D second = QVector3D::normal(coordinates[3], coordinates[4], coordinates[5]);
    //QVector3D normals[] = {
    //    first, first, first,
    //    second, second, second
    //};
    QVector3D normals[] = {
        QVector3D::normal(coordinates[0], coordinates[1]),
        QVector3D::normal(coordinates[1], coordinates[2]),
        QVector3D::normal(coordinates[2], coordinates[1]),

        QVector3D::normal(coordinates[3], coordinates[4]),
        QVector3D::normal(coordinates[4], coordinates[5]),
        QVector3D::normal(coordinates[5], coordinates[3]),
    };
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
    faces = new PlaneDrawObject*[6];
    QVector3D coordinates[] = {
        // Vertex data for face 0
        QVector3D(-1.0f, -1.0f,  1.0f), // v0
        QVector3D( 1.0f, -1.0f,  1.0f), // v1
        QVector3D(-1.0f,  1.0f,  1.0f), // v2
        QVector3D( 1.0f,  1.0f,  1.0f), // v3

        // Vertex data for face 1
        QVector3D( 1.0f, -1.0f,  1.0f), // v4
        QVector3D( 1.0f, -1.0f, -1.0f), // v5
        QVector3D( 1.0f,  1.0f,  1.0f), // v6
        QVector3D( 1.0f,  1.0f, -1.0f), // v7

        // Vertex data for face 2
        QVector3D( 1.0f, -1.0f, -1.0f), // v8
        QVector3D(-1.0f, -1.0f, -1.0f), // v9
        QVector3D( 1.0f,  1.0f, -1.0f), // v10
        QVector3D(-1.0f,  1.0f, -1.0f), // v11

        // Vertex data for face 3
        QVector3D(-1.0f, -1.0f, -1.0f), // v12
        QVector3D(-1.0f, -1.0f,  1.0f), // v13
        QVector3D(-1.0f,  1.0f, -1.0f), // v14
        QVector3D(-1.0f,  1.0f,  1.0f), // v15

        // Vertex data for face 4
        QVector3D(-1.0f, -1.0f, -1.0f), // v16
        QVector3D( 1.0f, -1.0f, -1.0f), // v17
        QVector3D(-1.0f, -1.0f,  1.0f), // v18
        QVector3D( 1.0f, -1.0f,  1.0f), // v19

        // Vertex data for face 5
        QVector3D(-1.0f,  1.0f,  1.0f), // v20
        QVector3D( 1.0f,  1.0f,  1.0f), // v21
        QVector3D(-1.0f,  1.0f, -1.0f), // v22
        QVector3D( 1.0f,  1.0f, -1.0f)  // v23
    };

    for(int i = 0; i < 6; i++)
        faces[i] = new PlaneDrawObject(coordinates + i * 4);
}

BoxDrawObject::~BoxDrawObject() { }

void BoxDrawObject::draw(QOpenGLShaderProgram *program) {
    for(int i = 0; i < 6; i++)
        faces[i]->draw(program);
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

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

GeometryEngine::GeometryEngine()
    : indexBuf(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();

    // Generate 2 VBOs
    arrayBuf.create();
    indexBuf.create();

    // Initializes cube geometry and transfers it to VBOs
    initCubeGeometry();
}

GeometryEngine::~GeometryEngine()
{
    arrayBuf.destroy();
    indexBuf.destroy();
}

void GeometryEngine::initCubeGeometry()
{
    // For cube we would need only 8 vertices but we have to
    // duplicate vertex for each face because texture coordinate
    // is different.
    VertexData vertices[] = {
        // Vertex data for face 0
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f)},  // v0
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.0f)}, // v1
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f, 0.5f)},  // v2
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v3

        // Vertex data for face 1
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D( 0.0f, 0.5f)}, // v4
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.5f)}, // v5
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.0f, 1.0f)},  // v6
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v7

        // Vertex data for face 2
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v8
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f, 0.5f)},  // v9
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}, // v10
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f, 1.0f)},  // v11

        // Vertex data for face 3
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v12
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(1.0f, 0.0f)},  // v13
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.66f, 0.5f)}, // v14
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(1.0f, 0.5f)},  // v15

        // Vertex data for face 4
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.33f, 0.0f)}, // v16
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.66f, 0.0f)}, // v17
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v18
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v19

        // Vertex data for face 5
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v20
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.66f, 0.5f)}, // v21
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.33f, 1.0f)}, // v22
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.66f, 1.0f)}  // v23
    };

    // Indices for drawing cube faces using triangle strips.
    // Triangle strips can be connected by duplicating indices
    // between the strips. If connecting strips have opposite
    // vertex order then last index of the first strip and first
    // index of the second strip needs to be duplicated. If
    // connecting strips have same vertex order then only last
    // index of the first strip needs to be duplicated.
    GLushort indices[] = {
         0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

    // Transfer vertex data to VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, 24 * sizeof(VertexData));

    // Transfer index data to VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, 34 * sizeof(GLushort));
}

void GeometryEngine::drawCubeGeometry(QOpenGLShaderProgram *program)
{
    // Tell OpenGL which VBOs to use
    arrayBuf.bind();
    indexBuf.bind();

    // Offset for position
    quintptr offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    //QColor color;
    //color.setRgb(0, 0, 255, 28);
    //program->setUniformValue("color", color);

    // Draw cube geometry using indices from VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
}
