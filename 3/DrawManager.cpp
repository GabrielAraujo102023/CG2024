#include "DrawManager.h"
#include <utility>
#include <valarray>
#include <fstream>
#include <sstream>

DrawManager* instance;
int frameCount = 0;
int previousTime = 0;
int startX, startY, tracking = 0;
int a = 0, b = 45, r = 0;
float camX, camY, camZ;
float moveSpeed = 0.5f;
// Modelo -> (VBO ID, Número de pontos)
map<string, GLuint[2]> vboIds;

void update(int value) {
    int elapsedTime = glutGet(GLUT_ELAPSED_TIME);
    int deltaTime = elapsedTime - previousTime;
    if (deltaTime > 1000) {
        float fps = (float) frameCount / ((float) deltaTime / 1000.0f);

        ostringstream windowTitle;
        windowTitle << fps;
        glutSetWindowTitle(windowTitle.str().c_str());

        frameCount = 0;
        previousTime = elapsedTime;
    }
    frameCount++;
    glutTimerFunc(1000 / 60, update, 0);
}


DrawManager::DrawManager(int windowWidth, int windowHeight, float px, float py, float pz, float lx, float ly, float lz,
                         float ux, float uy, float uz, float fov, float near, float far, int argc,
                         char **argv, list<Group> rootGroup) {
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
    this->px = px;
    this->py = py;
    this->pz = pz;
    this->lx = lx;
    this->ly = ly;
    this->lz = lz;
    this->ux = ux;
    this->uy = uy;
    this->uz = uz;
    this->fov = fov;
    this->near = near;
    this->far = far;
    this->argc = argc;
    this->argv = argv;
    this->rootGroup = std::move(rootGroup);
}

void DrawManager::changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if(instance->windowHeight == 0)
        instance->windowHeight = 1;

    // compute window's aspect ratio
    float ratio = instance->windowWidth * 1.0 / instance->windowHeight;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, instance->windowWidth, instance->windowHeight);

    // Set perspective
    gluPerspective(instance->fov ,ratio, instance->near ,instance->far);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void DrawManager::renderScene() {
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // set the camera
    glLoadIdentity();

    gluLookAt(camX,camY,camZ,
              instance->lx,instance->ly,instance->lz,
              instance->ux,instance->uy,instance->uz);

    glBegin(GL_LINES);
    // X axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f( 100.0f, 0.0f, 0.0f);
    // Y Axis in Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    // Z Axis in Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();

    DrawManager::drawMyStuff(instance->rootGroup.front());

    // End of frame
    glutSwapBuffers();
}

void buildRotMatrix(const float *x, const float *y, const float *z, float *m) {

    m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
    m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
    m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

void cross(const float *aa, const float *bb, float *res) {

    res[0] = aa[1]*bb[2] - aa[2]*bb[1];
    res[1] = aa[2]*bb[0] - aa[0]*bb[2];
    res[2] = aa[0]*bb[1] - aa[1]*bb[0];
}


void normalize(float *aa) {

    float l = sqrt(aa[0] * aa[0] + aa[1] * aa[1] + aa[2] * aa[2]);
    aa[0] = aa[0] / l;
    aa[1] = aa[1] / l;
    aa[2] = aa[2] / l;
}

void multMatrixVector(const float *m, const float *v, float *res) {

    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j * 4 + k];
        }
    }

}

void getCatmullRomPoint(float t,
                        const float *p0, const float *p1, const float *p2, const float *p3,
                        float *pos, float *deriv) {
    // Catmull-Rom matrix

    float m[4][4] = { {-0.5f, 1.5f, -1.5f, 0.5f},
                      { 1.0f, -2.5f, 2.0f, -0.5f},
                      {-0.5f, 0.0f, 0.5f, 0.0f},
                      { 0.0f, 1.0f, 0.0f, 0.0f}};


    float P[4]; // Control point vector
    float A[4]; // Resultant vector after matrix multiplication

    // Loop through each component (x, y, z)
    for (int i = 0; i < 3; ++i) {
        // Extract the control points for this component
        P[0] = p0[i]; P[1] = p1[i]; P[2] = p2[i]; P[3] = p3[i];

        // Compute vector A = M * P
        multMatrixVector((float *)m, P, A);

        // Compute pos[i] = T * A
        pos[i] = ((t * t * t) * A[0]) + ((t * t) * A[1]) + (t * A[2]) + ((1.0f) * A[3]);

        // Compute deriv[i] = T' * A
        deriv[i] = ((3.0f * t * t) * A[0]) + ((2.0f * t) * A [1]) + (1.0f * A[2]) + (0.0f * A[3]);
    }
}

void getGlobalCatmullRomPoint(float gt, float *pos, float *deriv, int nPoints,
                              float p[][3]) {

    float t = gt * (float) nPoints; // this is the real global t
    int index = floor(t);  // which segment
    t = t - (float) index; // where within  the segment

    // indices store the points
    int indices[4];
    indices[0] = (index + nPoints-1)%nPoints;
    indices[1] = (indices[0]+1)%nPoints;
    indices[2] = (indices[1]+1)%nPoints;
    indices[3] = (indices[2]+1)%nPoints;

    getCatmullRomPoint(t, p[indices[0]], p[indices[1]], p[indices[2]], p[indices[3]], pos, deriv);
}

void renderCatmullRomCurve(int nPoints, float points[][3]) {
    glBegin(GL_LINE_LOOP);

    for (float t = 0; t <= 100; t++) {
        float pos[3], deriv[3];
        // Compute the point and its derivative on the curve at parameter t
        getGlobalCatmullRomPoint(t/100, pos, deriv, nPoints, points);
        // Render the interpolated point
        glVertex3f(pos[0], pos[1], pos[2]);
    }

    glEnd();
}

void doCurveTranslation(const Group& rootGroup, map<char, float> values)
{
    static float t = 0;
    float pos[3], deriv[3];
    int nPoints = (int) values['n'] / 3;
    float p[nPoints][3];
    int j = 0;
    for(int i = 0; i < nPoints; i++, j += 3)
    {
        p[i][0] = rootGroup.points.at((int) values['i'])[j];
        p[i][1] = rootGroup.points.at((int) values['i'])[j + 1];
        p[i][2] = rootGroup.points.at((int) values['i'])[j + 2];
    }
    renderCatmullRomCurve(nPoints, p);
    getGlobalCatmullRomPoint(t, pos, deriv, (int)values['n'], p);
    glTranslatef(pos[0], pos[1], pos[2]);
    // Align
    if (values['a'] == 1)
    {
        float zaxis[3];
        float yaxis[3] = {0, 1, 0};
        cross(deriv, yaxis, zaxis);
        cross(zaxis, deriv, yaxis);
        normalize(deriv);
        normalize(yaxis);
        normalize(zaxis);

        float rotationMatrix[16];
        buildRotMatrix(deriv, yaxis, zaxis, rotationMatrix);
        glMultMatrixf(rotationMatrix);
    }

    glutSwapBuffers();
    t += 0.001;
    //glutPostRedisplay();
}

void doTransformations(Group& rootGroup)
{
    for(const auto& pair : rootGroup.transformation){
        char type = pair.first;
        map<char, float> values = pair.second;
        switch (type){
            case 't':
                if(values['t'] == -1)
                {
                    glTranslatef(values['x'], values['y'], values['z']);
                }
                else
                {
                    doCurveTranslation(rootGroup, values);
                }
                break;
            case 'r':
                if(values['t'] == -1)
                {
                    glRotatef(values['a'], values['x'], values['y'], values['z']);
                }
                else
                {
                    if(values['b'] == 0)
                    {
                        values['i'] = (float) glutGet(GLUT_ELAPSED_TIME);
                        values['b'] = 1;
                    }
                    float now = (float) glutGet(GLUT_ELAPSED_TIME);
                    values['r'] += (now - values['i']) * values['s'];
                    values['i'] = now;
                    glRotatef(values['r'], values['x'], values['y'], values['z']);
                    glutPostRedisplay();
                    rootGroup.transformation['r'] = values;
                }
                break;
            case 's':
                glScalef(values['x'], values['y'], values['z']);
                break;
            default:
                break;
        }
    }
}

void DrawManager::drawMyStuff(Group& rootGroup)
{
    glPushMatrix();
    // Fazer transformações
    doTransformations(rootGroup);

    // Desenhar modelos
    glColor3f(1,1,1);
    for(const string& model : rootGroup.models)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vboIds[model][0]);
        glVertexPointer(3, GL_FLOAT,0,nullptr);
        glDrawArrays(GL_TRIANGLES, 0, vboIds[model][1]);
    }

    for(Group& g : rootGroup.groups)
    {
        DrawManager::drawMyStuff(g);
    }
    glPopMatrix();
}

void DrawManager::processKeys(unsigned char c, int xx, int yy) {
    // Vetor que vai da câmara ao ponto que estamos a olhar
    float forwardX = instance->lx - camX;
    float forwardY = instance->ly - camY;
    float forwardZ = instance->lz - camZ;

    // Normallização
    float forwardLength = sqrt(forwardX * forwardX + forwardY * forwardY + forwardZ * forwardZ);
    forwardX /= forwardLength;
    forwardY /= forwardLength;
    forwardZ /= forwardLength;

    // Vetor da direita da câmara
    float rightX = forwardY * instance->uy - forwardZ * instance->uz;
    float rightY = forwardZ * instance->ux - forwardX * instance->uz;
    float rightZ = forwardX * instance->uy - forwardY * instance->ux;

    switch(c)
    {
        case 'a':
            camX -= moveSpeed * rightX;
            camY -= moveSpeed * rightY;
            camZ -= moveSpeed * rightZ;

            instance->lx -= moveSpeed * rightX;
            instance->ly -= moveSpeed * rightY;
            instance->lz -= moveSpeed * rightZ;
            break;
        case 'd':
            camX += moveSpeed * rightX;
            camY += moveSpeed * rightY;
            camZ += moveSpeed * rightZ;

            instance->lx += moveSpeed * rightX;
            instance->ly += moveSpeed * rightY;
            instance->lz += moveSpeed * rightZ;
            break;
        default:
            break;
    }
    r = (int) sqrt(camX * camX + camY * camY + camZ * camZ);
    glutPostRedisplay();
}


void DrawManager::processSpecialKeys(int key, int xx, int yy) {
    switch(key)
    {
        case GLUT_KEY_UP:
            moveSpeed += 0.1;
            break;
        case GLUT_KEY_DOWN:
            moveSpeed -= 0.1;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void processMouseButtons(int button, int state, int xx, int yy) {
    if (state == GLUT_DOWN)  {
        startX = xx;
        startY = yy;
        if (button == GLUT_LEFT_BUTTON)
            tracking = 1;
        else if (button == GLUT_RIGHT_BUTTON)
            tracking = 2;
        else
            tracking = 0;
    }
    else if (state == GLUT_UP) {
        if (tracking == 1) {
            a += (xx - startX);
            b += (yy - startY);
        }
        else if (tracking == 2) {

            r -= yy - startY;
            if (r < 3)
                r = 3.0;
        }
        tracking = 0;
    }
}


void processMouseMotion(int xx, int yy) {
    int deltaX, deltaY;
    int alphaAux = 0, betaAux = 0, rAux = 0;

    if (!tracking)
        return;

    deltaX = xx - startX;
    deltaY = yy - startY;

    if (tracking == 1) {
        alphaAux = a + deltaX;
        betaAux = b + deltaY;

        if (betaAux > 85.0)
            betaAux = 85.0;
        else if (betaAux < -85.0)
            betaAux = -85.0;

        rAux = r;
    }
    else if (tracking == 2) {

        alphaAux = a;
        betaAux = b;
        rAux = r - deltaY;
        if (rAux < 3)
            rAux = 3;
    }
    camX = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
    camZ = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
    camY = rAux * sin(betaAux * 3.14 / 180.0);
    glutPostRedisplay();
}

void DrawManager::Draw() {
    instance = this;
    glutInit(&argc, argv);

    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Welcome back, Mr. Stark");
    // Required callback registry
    ::glutDisplayFunc(renderScene);
    ::glutReshapeFunc(changeSize);

    // Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(processMouseButtons);
    glutMotionFunc(processMouseMotion);

    glutTimerFunc(0, update, 0);

    camX = this->px;
    camY = this->py;
    camZ = this->pz;
    r = (int) sqrt(this->px * this->px + this->py * this->py + this->pz * this->pz);
    a = (int) acos(instance->pz / (float) r);
    b = (int) atan2(instance->py, instance->px);

    glewInit();
    glEnableClientState(GL_VERTEX_ARRAY);
    loadModels(instance->rootGroup.front());
    //initAxisVBO();

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glutMainLoop();
}

void DrawManager::loadModels(const Group& group) {
    for(const string& model : group.models)
    {
        ifstream file(model);
        if (!file.is_open()) {
            printf("There was a problem with the file: %s\n", model.c_str());
            return;
        }

        float point;
        stringstream p;
        string line;
        getline(file, line);
        vector<GLfloat> vertex;
        while (file >> point)
        {
            vertex.emplace_back(point);
        }
        initVBO(vertex, model);
    }

    for(const Group& g : group.groups)
    {
        loadModels(g);
    }
}

void DrawManager::initVBO(vector<GLfloat> vertex, const string& model) {
    GLuint id;
    unsigned int size = vertex.size();
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex.size(), vertex.data(),
                 GL_STATIC_DRAW);
    vboIds[model][0] = id;
    vboIds[model][1] = size;
}
/*
void DrawManager::initAxisVBO() {
    GLuint xid, yid, zid;
    vector<GLfloat> vertexX = {-100, 0, 0, 100, 0, 0};
    vector<GLfloat> vertexY = {0, -100, 0, 0, 100, 0};
    vector<GLfloat> vertexZ = {0, 0, -100, 0, 0, 100};

    vector<GLuint> ids = {xid, yid, zid};
    vector<vector<GLfloat>> vertexes = {vertexX, vertexY, vertexZ};
    vector<string> names = {"xaxis", "yaxis", "zaxis"};

    int vertexSize = 6;
    GLuint bufferSize = sizeof(GLfloat) * vertexSize;

    for(int i = 0; i < 3; i++){
        glGenBuffers(1, &ids[i]);
        glBindBuffer(GL_ARRAY_BUFFER, ids[i]);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, vertexes[i].data(), GL_STATIC_DRAW);
        vboIds[names[i]][0] = ids[i];
        vboIds[names[i]][1] = vertexSize;
    }
}

void DrawManager::drawAxis() {
    vector<string> names = {"xaxis", "yaxis", "zaxis"};
    //X vermelho, Y verde, Z azul
    vector<vector<float>> colors = {{1,0,0},{0,1,0},{0,0,1}};
    for(int i = 0; i < 3; i++)
    {
        glColor3f(colors[i][0], colors[i][1], colors[i][2]);
        glBindBuffer(GL_ARRAY_BUFFER, vboIds[names[i]][0]);
        glVertexPointer(3, GL_FLOAT,0,nullptr);
        glDrawArrays(GL_TRIANGLES, 0, vboIds[names[i]][1]);
    }
}
*/