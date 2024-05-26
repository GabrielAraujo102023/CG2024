#ifndef ENGINE_GROUP_H
#define ENGINE_GROUP_H
#include <cstdlib>
#include <string>
#include <map>
#include <list>
#include <vector>
using namespace std;


class Group {
public:

    class Model{
    public:

        class Color {
        public:
            int diffuse[3] = {200, 200, 200};
            int ambient[3] = {50, 50, 50};
            int specular[3] = {0, 0, 0};
            int emissive[3] = {0, 0, 0};
            int shininess = 0;
        };

        string file;
        string texture;
        Color color;
        unsigned int vboID;
        unsigned int normalID;
        unsigned int texIDBuffer;
        unsigned int texIDIL;
        unsigned int nPoints;
    };

    Group(map<char,map<char, float>> transformation, list<Model> models, list<Group> groups,
          map<int, vector<vector<float>>> points);
    Group();
    map<char,map<char, float>> transformation;
    list<Model> models;
    list<Group> groups;
    map<int, vector<vector<float>>> points;
};


#endif //ENGINE_GROUP_H
