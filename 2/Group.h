#ifndef ENGINE_GROUP_H
#define ENGINE_GROUP_H
#include <cstdlib>
#include <string>
#include <map>
#include <list>
using namespace std;


class Group {
public:
    Group(map<char,map<char, float>> transformation, list<string> models, list<Group> groups);
    Group();
    map<char,map<char, float>> transformation;
    list<string> models;
    list<Group> groups;
};


#endif //ENGINE_GROUP_H
