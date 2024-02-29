#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
double r = 5;
double alpha = 0;
double beta = 0;
int s = 10;
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


void drawCylinder(float radius, float height, int slices) {
    float centerTop = height / 2;
    float centerBottom = centerTop * -1;
    float fullCircle = (2 * M_PI);
    float alpha = fullCircle / slices;
    int drawnSlices = 1;
    glBegin(GL_TRIANGLES);
    float x, x2, z, z2;
    while (drawnSlices <= slices)
    {
        x = radius * cos(alpha * drawnSlices);
        x2 = radius * cos(alpha * (drawnSlices + 1));
        z = radius * sin(alpha * drawnSlices);
        z2 = radius * sin(alpha * (drawnSlices + 1));

        // Top and Bottom
        glVertex3f(x2,centerTop,z2);
        glVertex3f(x,centerTop,z);
        glVertex3f(0,centerTop,0);

        glVertex3f(x,centerBottom,z);
        glVertex3f(x2,centerBottom,z2);
        glVertex3f(0,centerBottom,0);

        // Sides
        glVertex3f(x,centerBottom,z);
        glVertex3f(x,centerTop,z);
        glVertex3f(x2,centerTop,z2);

        glVertex3f(x2,centerTop,z2);
        glVertex3f(x2,centerBottom,z2);
        glVertex3f(x,centerBottom,z);

        drawnSlices++;
    }
    glEnd();
}

void drawMyStuff()
{
    std::ifstream file("./plane.3d");
    if (!file.is_open())
    {
        printf("There was a problem with the file.");
    }
    std::string line;
    std::getline(file, line);
    int x,y,z;
    glBegin(GL_TRIANGLES);
    while(file >> x >> y >> z)
    {
        glVertex3f(x,y,z);
    }
    glEnd();
}


void renderScene(void) {

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    double rcosbeta = r * cos(beta);
    double px = rcosbeta * cos(alpha);
    double pz = rcosbeta * sin(alpha);
    double py = r * sin(beta);
    gluLookAt(px,py,pz,
              0.0,0.0,0.0,
              0.0f,1.0f,0.0f);

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
            r -= 0.1;
            break;
        case 'd':
            alpha -= 0.1;
            break;
        case 'a':
            alpha += 0.1;
            break;
        case 's':
            r += 0.1;
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
            beta += 0.1;
            break;
        case GLUT_KEY_DOWN:
            beta -= 0.1;
            break;
    }
    glutPostRedisplay();
}


int main(int argc, char **argv) {

// init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,800);
    glutCreateWindow("CG@DI-UM");

// Required callback registry
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

// Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

//  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

// enter GLUT's main cycle
    glutMainLoop();

    return 1;
}