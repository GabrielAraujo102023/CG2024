#include <list>
#include "Reader.h"
#include "tinyxml.h"

void Reader::getWindow(int &width, int &height) const {
    TiXmlElement* window = WORLD->FirstChildElement("window");
    if (window)
    {
        window->QueryIntAttribute("width", &width);
        window->QueryIntAttribute("height", &height);
    }
}

void Reader::getCamera(float &posX, float &posY, float &posZ, float &lookX, float &lookY, float &lookZ,
                         float &upX, float &upY, float &upZ, float &fov, float &near, float &far) const
{
    TiXmlElement* position = WORLD->FirstChildElement("position");
    if (position)
    {
        position->QueryFloatAttribute("x", &posX);
        position->QueryFloatAttribute("y", &posY);
        position->QueryFloatAttribute("z", &posZ);
    }
    TiXmlElement* lookAt = WORLD->FirstChildElement("lookAt");
    if (lookAt)
    {
        lookAt->QueryFloatAttribute("x", &lookX);
        lookAt->QueryFloatAttribute("y", &lookY);
        lookAt->QueryFloatAttribute("z", &lookZ);
    }
    TiXmlElement* up = WORLD->FirstChildElement("up");
    if (up)
    {
        up->QueryFloatAttribute("x", &upX);
        up->QueryFloatAttribute("y", &upY);
        up->QueryFloatAttribute("z", &upZ);
    }
    TiXmlElement* projection = WORLD->FirstChildElement("projection");
    if (projection)
    {
        projection->QueryFloatAttribute("fov", &fov);
        projection->QueryFloatAttribute("near", &near);
        projection->QueryFloatAttribute("far", &far);
    }
}

std::list<std::string> Reader::getModels() const
{
    std::list<std::string> models;
    TiXmlElement* elements = WORLD->FirstChildElement("models");
    if (elements)
    {
        for(TiXmlElement* modelElement = elements->FirstChildElement("model");
            modelElement;
            modelElement = modelElement->NextSiblingElement("model"))
        {
            models.emplace_back(modelElement->Attribute("file"));
        }
    }
    return models;
}

Reader::Reader(char *filename) {
    TiXmlDocument doc;
    if (doc.LoadFile(filename) != TIXML_SUCCESS) {
        std::cerr << "Failed to load XML file.";
        return;
    }
    WORLD = doc.FirstChildElement("");
}
