#include <utility>
#include <valarray>
#include <fstream>
#include <sstream>
#include "DrawManager.h"

DrawManager* instance;
int frameCount = 0;
int previousTime = 0;

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
    this->r = 5;
    this->alpha = 0;
    this->beta = 0;
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

    if (instance->beta > M_PI / 2)
        instance->beta = M_PI / 2;
    else if (instance->beta < -M_PI / 2)
        instance->beta = -M_PI / 2;

    gluLookAt(instance->px,instance->py,instance->pz,
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
        ifstream file(model);
        if (!file.is_open()) {
            printf("There was a problem with the file: %s\n", model.c_str());
            return;
        }

        float x, y, z;
        stringstream p;
        string line;
        getline(file, line);
        if (line != "cone")
        {
            glBegin(GL_TRIANGLES);
            while (file >> x >> y >> z)
            {
                glVertex3f(x, y, z);
                p << x << " " << y << " " << z;
                p.str("");
            }
            glEnd();
        }
        else
        {
            getline(file, line);
            stringstream aux(line);
            glBegin(GL_TRIANGLE_FAN);
            while (line != "triang")
            {
                aux >> x >> y >> z;
                glVertex3f(x, y, z);
                getline(file, line);
                aux.str(line);
            }
            glEnd();
            glBegin(GL_TRIANGLES);
            while (file >> x >> y >> z)
            {
                glVertex3f(x, y, z);
                p << x << " " << y << " " << z;
                p.str("");
            }
            glEnd();
        }
    }

    for(const Group& g : rootGroup.groups)
    {
        DrawManager::drawMyStuff(g);
    }
    glPopMatrix();
}

void DrawManager::processKeys(unsigned char c, int xx, int yy) {
    switch(c)
    {
        case 'w':
            instance->beta += 0.1;
            break;
        case 'd':
            instance->alpha -= 0.1;
            break;
        case 'a':
            instance->alpha += 0.1;
            break;
        case 's':
            instance->beta -= 0.1;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}


void DrawManager::processSpecialKeys(int key, int xx, int yy) {
    switch(key)
    {
        case GLUT_KEY_UP:
            instance->r -= 0.1;
            break;
        case GLUT_KEY_DOWN:
            instance->r += 0.1;
            break;
        default:
            break;
    }
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

    glutTimerFunc(0, update, 0);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    glutMainLoop();
}
