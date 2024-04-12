#ifndef ENGINE_DRAWMANAGER_H
#define ENGINE_DRAWMANAGER_H
#include <GL/glew.h>
#include <vector>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <list>
#include "Group.h"
#endif

using namespace std;
class DrawManager
{
public:
    DrawManager(int windowWidth, int windowHeight, float px, float py, float pz, float lx, float ly, float lz,
                float ux, float uy, float uz, float fov, float near, float far, int argc,
                char **argv, list<Group> rootGroup);
    void Draw();
private:
    int windowWidth, windowHeight, argc;
    float px, py, pz, lx, ly, lz, ux, uy, uz, fov, near, far;
    char **argv;
    static void changeSize(int w, int h);
    static void renderScene();
    static void drawMyStuff(const Group& rootGroup);
    static void processSpecialKeys(int key, int xx, int yy);
    static void processKeys(unsigned char c, int xx, int yy);
    static void loadModels(const Group& group);
    static void initVBO(vector<GLfloat> vertex, const string& model);
    //static void initAxisVBO();
    //static void drawAxis();
    list<Group> rootGroup;
};


#endif //ENGINE_DRAWMANAGER_H
