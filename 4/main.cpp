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
list<DrawManager::Light> lights;
list<Group> loadGroups(TiXmlElement* root)
{
    list<Group> res = {};
    int translateId = 0;
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
                float x, y, z, time = -1, align, nPoints = 0;
                string alignAux;
                translate->QueryFloatAttribute("x", &x);
                translate->QueryFloatAttribute("y", &y);
                translate->QueryFloatAttribute("z", &z);
                translate->QueryFloatAttribute("time", &time);
                translate->QueryStringAttribute("align", &alignAux);

                if(!alignAux.empty()){
                    align = alignAux == "true";
                    vector<vector<float>> points(40, vector<float>(3));
                    float point_x, point_y, point_z;
                    int i = 0;
                    for(TiXmlElement* point = translate->FirstChildElement("point");
                        point;
                        point = point->NextSiblingElement("point"))
                    {
                        point->QueryFloatAttribute("x", &point_x);
                        point->QueryFloatAttribute("y", &point_y);
                        point->QueryFloatAttribute("z", &point_z);
                        points[i][0] = point_x;
                        points[i][1] = point_y;
                        points[i][2] = point_z;
                        i++;
                        nPoints++;
                    }
                    GROUP.points[translateId] = points;
                }
                transformation['t'] = {
                                {'x', x},
                                {'y', y},
                                {'z', z},
                                {'t', time},
                                {'a', align},
                                {'i', translateId++},
                                {'n', nPoints}
                };
            }

            TiXmlElement* rotate = transform->FirstChildElement("rotate");
            if (rotate)
            {
                float angle, x, y, z, time = -1;
                rotate->QueryFloatAttribute("angle", &angle);
                rotate->QueryFloatAttribute("x", &x);
                rotate->QueryFloatAttribute("y", &y);
                rotate->QueryFloatAttribute("z", &z);
                rotate->QueryFloatAttribute("time", &time);
                transformation['r'] = {
                        {'a', angle},
                        {'x', x},
                        {'y', y},
                        {'z', z},
                        {'t', time},
                        {'i', 0}, //Tempo inicial
                        {'b', 0}, //Bool se ja calculou tempo inicial
                        {'s', 360.0f / (time * 1000)}, //O quanto deve rodar a cada segundo
                        {'r', 0}  //Acomulador para os ângulos
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
                Group::Model new_model;
                new_model.file = modelElement->Attribute("file");

                TiXmlElement *texture = modelElement->FirstChildElement("texture");
                if (texture)
                {
                    new_model.texture = texture->Attribute("file");
                }
                TiXmlElement *color_element = modelElement->FirstChildElement("color");
                if (color_element)
                {
                    Group::Model::Color color{};
                    TiXmlElement *diffuse = color_element->FirstChildElement("diffuse");
                    if (diffuse)
                    {
                        diffuse->QueryIntAttribute("R", &color.diffuse[0]);
                        diffuse->QueryIntAttribute("G", &color.diffuse[1]);
                        diffuse->QueryIntAttribute("B", &color.diffuse[2]);
                    }
                    TiXmlElement *ambient = color_element->FirstChildElement("ambient");
                    if (ambient)
                    {
                        ambient->QueryIntAttribute("R", &color.ambient[0]);
                        ambient->QueryIntAttribute("G", &color.ambient[1]);
                        ambient->QueryIntAttribute("B", &color.ambient[2]);
                    }
                    TiXmlElement *specular = color_element->FirstChildElement("specular");
                    if (specular)
                    {
                        specular->QueryIntAttribute("R", &color.specular[0]);
                        specular->QueryIntAttribute("G", &color.specular[1]);
                        specular->QueryIntAttribute("B", &color.specular[2]);
                    }
                    TiXmlElement *emissive = color_element->FirstChildElement("emissive");
                    if (emissive)
                    {
                        emissive->QueryIntAttribute("R", &color.emissive[0]);
                        emissive->QueryIntAttribute("G", &color.emissive[1]);
                        emissive->QueryIntAttribute("B", &color.emissive[2]);
                    }
                    TiXmlElement *shininess = color_element->FirstChildElement("shininess");
                    if (shininess)
                    {
                        shininess->QueryIntAttribute("value", &color.shininess);
                    }
                    new_model.color = color;
                }
                GROUP.models.emplace_back(new_model);
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

    TiXmlElement *lights_element = WORLD->FirstChildElement("lights");
    if (lights_element) {
        for(TiXmlElement* light_element = lights_element->FirstChildElement("light");
            light_element;
            light_element = light_element->NextSiblingElement("light"))
        {
            DrawManager::Light new_light;
            new_light.type = light_element->Attribute("type");
            light_element->QueryFloatAttribute("posX", &new_light.posX);
            light_element->QueryFloatAttribute("posY", &new_light.posY);
            light_element->QueryFloatAttribute("posZ", &new_light.posZ);
            light_element->QueryFloatAttribute("dirX", &new_light.dirX);
            light_element->QueryFloatAttribute("dirY", &new_light.dirY);
            light_element->QueryFloatAttribute("dirZ", &new_light.dirZ);
            light_element->QueryFloatAttribute("cutoff", &new_light.cutoff);
            lights.emplace_back(new_light);
        }
    }

    rootGroup = loadGroups(WORLD);
}

int main(int argc, char **argv) {
    int width, height;
    loadDocument(argv[1], width, height);
    DrawManager drawer = DrawManager(width, height, px, py, pz, lx, ly, lz, ux, uy, uz, fov,
                                     near, far, argc, argv, rootGroup, lights);
    drawer.Draw();
    return 0;
}