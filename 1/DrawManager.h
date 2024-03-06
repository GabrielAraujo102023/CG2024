#ifndef ENGINE_DRAWMANAGER_H
#define ENGINE_DRAWMANAGER_H
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <list>

#endif

class DrawManager
{
public:
    DrawManager(int windowWidth, int windowHeight, float px, float py, float pz, float lx, float ly, float lz,
                float ux, float uy, float uz, float fov, float near, float far, int argc,
                char **argv, std::list<std::string> modelList);
    void Draw();
private:
    int windowWidth, windowHeight, s, argc;
    float px, py, pz, lx, ly, lz, ux, uy, uz, fov, near, far, r, beta, alpha;
    char **argv;
    static void changeSize(int w, int h);
    static void renderScene();
    static void drawMyStuff(const std::string& filename);
    static void processSpecialKeys(int key, int xx, int yy);
    static void processKeys(unsigned char c, int xx, int yy);
    std::list<std::string> modelList;
};


#endif //ENGINE_DRAWMANAGER_H
