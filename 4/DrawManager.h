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
    class Light {
    public:
        string type;
        float posX, posY, posZ, dirX, dirY, dirZ, cutoff;
    };
    DrawManager(int windowWidth, int windowHeight, float px, float py, float pz, float lx, float ly, float lz,
                float ux, float uy, float uz, float fov, float near, float far, int argc,
                char **argv, list<Group> rootGroup, list<Light> lights);
    void Draw();

    list<Light> lights;
    int windowWidth, windowHeight, argc;
    float px, py, pz, lx, ly, lz, ux, uy, uz, fov, near, far;
    char **argv;
    list<Group> rootGroup;
    static void changeSize(int w, int h);
    static void renderScene();
    static void drawMyStuff(Group& rootGroup);
    static void processSpecialKeys(int key, int xx, int yy);
    static void processKeys(unsigned char c, int xx, int yy);
    static void loadModels(Group *group);
};


#endif //ENGINE_DRAWMANAGER_H
