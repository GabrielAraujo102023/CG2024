#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#endif
#include <cstdio>

#include <list>
#include "tinyxml.h"
#include "DrawManager.h"
#include "Group.h"

using namespace std;

float px, py, pz, lx, ly, lz, ux, uy, uz, fov, near, far;
list<Group> rootGroup;

list<Group> loadGroups(TiXmlElement* root)
{
    list<Group> res = {};
    for (TiXmlElement* group = root->FirstChildElement("group");
         group != nullptr;
         group = group->NextSiblingElement("group")) {
        Group GROUP;
        map<char, map<char, float>> transformation;
        TiXmlElement* transform = group->FirstChildElement("transform");
        if (transform)
        {
            TiXmlElement* translate = transform->FirstChildElement("translate");
            if (translate)
            {
                float x, y, z;
                translate->QueryFloatAttribute("x", &x);
                translate->QueryFloatAttribute("y", &y);
                translate->QueryFloatAttribute("z", &z);
                transformation['t'] = {
                                {'x', x},
                                {'y', y},
                                {'z', z}
                };
            }

            TiXmlElement* rotate = transform->FirstChildElement("rotate");
            if (rotate)
            {
                float angle, x, y, z;
                rotate->QueryFloatAttribute("angle", &angle);
                rotate->QueryFloatAttribute("x", &x);
                rotate->QueryFloatAttribute("y", &y);
                rotate->QueryFloatAttribute("z", &z);
                transformation['r'] = {
                        {'a', angle},
                        {'x', x},
                        {'y', y},
                        {'z', z}
                };
            }

            TiXmlElement* scale = transform->FirstChildElement("scale");
            if (scale)
            {
                float x, y, z;
                scale->QueryFloatAttribute("x", &x);
                scale->QueryFloatAttribute("y", &y);
                scale->QueryFloatAttribute("z", &z);
                transformation['s'] = {
                        {'x', x},
                        {'y', y},
                        {'z', z}
                };
            }
        }

        GROUP.transformation = transformation;

        TiXmlElement* m = group->FirstChildElement("models");
        if (m)
        {
            for(TiXmlElement* modelElement = m->FirstChildElement("model");
                modelElement;
                modelElement = modelElement->NextSiblingElement("model"))
            {
                GROUP.models.emplace_back(modelElement->Attribute("file"));
            }
        }

        GROUP.groups = loadGroups(group);
        res.emplace_back(GROUP);
    }

    return res;
}

void loadDocument(char* filename, int &width, int &height){
    TiXmlDocument d;
    if (!d.LoadFile(filename)) {
        printf("Failed to load XML file.");
        return;
    }
    TiXmlElement* WORLD = d.RootElement();
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
        if(position) {
            position->QueryFloatAttribute("x", &px);
            position->QueryFloatAttribute("y", &py);
            position->QueryFloatAttribute("z", &pz);
        }


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

    rootGroup = loadGroups(WORLD);
}

int main(int argc, char **argv) {
    int width, height;
    loadDocument(argv[1], width, height);
    DrawManager drawer = DrawManager(width, height, px, py, pz, lx, ly, lz, ux, uy, uz, fov,
                                     near, far, argc, argv, rootGroup);
    drawer.Draw();
    return 0;
}