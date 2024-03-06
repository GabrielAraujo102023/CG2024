#include <utility>
#include <valarray>
#include <fstream>
#include <sstream>
#include "DrawManager.h"

DrawManager* instance;

DrawManager::DrawManager(int windowWidth, int windowHeight, float px, float py, float pz, float lx, float ly, float lz,
                         float ux, float uy, float uz, float fov, float near, float far, int argc,
                         char **argv, std::list<std::string> modelList) {
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
    this->s = 10;
    this->argc = argc;
    this->argv = argv;
    this->modelList = std::move(modelList);
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
    gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void DrawManager::renderScene() {
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    float rcosbeta = instance->r * std::cos(instance->beta);
    instance->px = rcosbeta * std::cos(instance->alpha);
    instance->pz = rcosbeta * std::sin(instance->alpha);
    instance->py = instance->r * std::sin(instance->beta);
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

    for (const std::string& model : instance->modelList)
    {
        DrawManager::drawMyStuff(model);
    }
    // End of frame
    glutSwapBuffers();
}

void DrawManager::drawMyStuff(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        printf("There was a problem with the file.");
        return;
    }
    std::string line;
    std::getline(file, line);
    float x,y,z;
    if(line != "cone")
    {
        glBegin(GL_TRIANGLES);
        while(file >> x >> y >> z)
        {
            glVertex3f(x, y, z);
        }
    }
    else
    {
        std::getline(file, line);
        std::stringstream aux(line);
        glBegin(GL_TRIANGLE_FAN);
        while(line != "triang")
        {
            aux >> x >> y >> z;
            glVertex3f(x, y, z);

            fflush(stdout);
            std::getline(file, line);
            printf("%s\n", line.c_str());
        }
        glBegin(GL_TRIANGLES);
        while(file >> x >> y >> z)
        {
            glVertex3f(x, y, z);
        }
    }

    glEnd();
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
        case 'm':
            instance->s++;
            break;
        case 'n':
            instance->s--;
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
    glutCreateWindow("wow");
    // Required callback registry
    ::glutDisplayFunc(renderScene);
    ::glutReshapeFunc(changeSize);

    // Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glEnable(GL_CULL_FACE);
    glutMainLoop();
}
