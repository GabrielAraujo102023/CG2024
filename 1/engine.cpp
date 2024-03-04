#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <cstdio>

#include <fstream>
#include <string>
#include <list>
#include "Reader.h"

float r = 5;
float alpha = 0;
float beta = 0;
float px, py, pz, lx, ly, lz, ux, uy, uz, fov, near, far;
int s = 10;
char* FILENAME;
std::list<float> pointList;
void changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if(h == 0)
        h = 1;

    // compute window's aspect ratio
    float ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

float frontAndPop(std::list<float> &list)
{
    float n = list.front();
    list.pop_front();
    return n;
}

void drawMyStuff()
{
    if (pointList.empty())
    {
        std::ifstream file(FILENAME);
        if (!file.is_open())
        {
            printf("There was a problem with the file.");
            return;
        }
        std::string line;
        std::getline(file, line);
        float x,y,z;
        glBegin(GL_TRIANGLES);
        while(file >> x >> y >> z)
        {
            glVertex3f(x, y, z);
            pointList.emplace_back(x);
            pointList.emplace_back(y);
            pointList.emplace_back(z);
        }
        glEnd();
    }
    else
    {
        glBegin(GL_TRIANGLES);
        std::list<float> auxList(pointList.size());
        std::copy(pointList.begin(), pointList.end(), auxList.begin());
        while(!auxList.empty())
        {
            glVertex3f(frontAndPop(auxList), frontAndPop(auxList), frontAndPop(auxList));
        }
        glEnd();
    }
}


void renderScene() {

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    float rcosbeta = r * std::cos(beta);
    px = rcosbeta * std::cos(alpha);
    pz = rcosbeta * std::sin(alpha);
    py = r * std::sin(beta);
    gluLookAt(px,py,pz,
              lx,ly,lz,
              ux,uy,uz);

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

    //drawCylinder(1,2,s);
    drawMyStuff();

    // End of frame
    glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {
    switch(c)
    {
        case 'w':
            beta += 0.1;
            break;
        case 'd':
            alpha -= 0.1;
            break;
        case 'a':
            alpha += 0.1;
            break;
        case 's':
            beta -= 0.1;
            break;
        case 'm':
            s++;
            break;
        case 'n':
            s--;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}


void processSpecialKeys(int key, int xx, int yy) {
    switch(key)
    {
        case GLUT_KEY_UP:
            r -= 0.1;
            break;
        case GLUT_KEY_DOWN:
            r += 0.1;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void loadDocument(char* filename){
    Reader reader(filename);
    reader.getCamera(px, py, pz, lx, ly, lz, ux, uy, uz, fov, near
                     , far);
    int width, height;
    reader.getWindow(width, height);
    glutInitWindowSize(width, height);
}

int main(int argc, char **argv) {
    FILENAME = argv[1];
// init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutCreateWindow("CG@DI-UM");

// Required callback registry
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

// Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

//  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glEnable(GL_CULL_FACE);


// enter GLUT's main cycle
    glutMainLoop();

    return 1;
}