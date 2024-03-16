#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cstdio>

#include <string>
#include <list>
#include "tinyxml.h"
#include "DrawManager.h"

float px, py, pz, lx, ly, lz, ux, uy, uz, fov, near, far;
std::list<std::string> models;

void loadDocument(char* filename, int &width, int &height){
    TiXmlDocument d;
    if (!d.LoadFile(filename)) {
        printf("Failed to load XML file.");
        return;
    }
    TiXmlElement *WORLD = d.RootElement();
    TiXmlElement* window = WORLD->FirstChildElement("window");
    if (window)
    {
        window->QueryIntAttribute("width", &width);
        window->QueryIntAttribute("height", &height);
    }

    TiXmlElement* camera = WORLD->FirstChildElement("camera");
    if (camera)
    {
        TiXmlElement *position = camera->FirstChildElement("position");
        position->QueryFloatAttribute("x", &px);
        position->QueryFloatAttribute("y", &py);
        position->QueryFloatAttribute("z", &pz);

        TiXmlElement* lookAt = camera->FirstChildElement("lookAt");
        if (lookAt)
        {
            lookAt->QueryFloatAttribute("x", &lx);
            lookAt->QueryFloatAttribute("y", &ly);
            lookAt->QueryFloatAttribute("z", &lz);
        }
        TiXmlElement* up = camera->FirstChildElement("up");
        if (up)
        {
            up->QueryFloatAttribute("x", &ux);
            up->QueryFloatAttribute("y", &uy);
            up->QueryFloatAttribute("z", &uz);
        }
        TiXmlElement* projection = camera->FirstChildElement("projection");
        if (projection)
        {
            projection->QueryFloatAttribute("fov", &fov);
            projection->QueryFloatAttribute("near", &near);
            projection->QueryFloatAttribute("far", &far);
        }
    }
    TiXmlElement* group = WORLD->FirstChildElement("group");
    if (group)
    {
        TiXmlElement* elements = group->FirstChildElement("models");
        if (elements)
        {
            for(TiXmlElement* modelElement = elements->FirstChildElement("model");
                modelElement;
                modelElement = modelElement->NextSiblingElement("model"))
            {
                models.emplace_back(modelElement->Attribute("file"));
            }
        }
    }
}

int main(int argc, char **argv) {
    int width, height;
    loadDocument(argv[1], width, height);
    DrawManager drawer = DrawManager(width, height, px, py, pz, lx, ly, lz, ux, uy, uz, fov,
                                     near, far, argc, argv, models);
    drawer.Draw();
    return 0;
}