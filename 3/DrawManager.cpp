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
        float fps = frameCount / (deltaTime / 1000.0f);

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

void DrawManager::drawMyStuff(const Group& rootGroup)
{
    glPushMatrix();
    // Fazer transformações
    for(const auto& pair : rootGroup.transformation){
        char type = pair.first;
        map<char, float> values = pair.second;

        switch (type){
            case 't':
                glTranslatef(values['x'], values['y'], values['z']);
                break;
            case 'r':
                glRotatef(values['a'], values['x'], values['y'], values['z']);
                break;
            case 's':
                glScalef(values['x'], values['y'], values['z']);
                break;
            default:
                break;
        }
    }

    // Desenhar modelos
    for(const string& model : rootGroup.models)
    {
        glBindBuffer(GL_ARRAY_BUFFER, vboIds[model][0]);
        glVertexPointer(3, GL_FLOAT,0,nullptr);
        glDrawArrays(GL_TRIANGLES, 0, vboIds[model][1]);
    }

    for(const Group& g : rootGroup.groups)
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
    r = sqrt(camX * camX + camY * camY + camZ * camZ);
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
    r = sqrt(this->px * this->px + this->py * this->py + this->pz * this->pz);
    a = acos(instance->pz / r);
    b = atan2(instance->py, instance->px);

    glewInit();
    glEnableClientState(GL_VERTEX_ARRAY);
    loadModels(instance->rootGroup.front());

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

void DrawManager::initVBO(vector<GLfloat> vertex, string model) {
    GLuint id;
    unsigned int size = vertex.size();
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex.size(), vertex.data(),
                 GL_STATIC_DRAW);
    vboIds[model][0] = id;
    vboIds[model][1] = size;
}