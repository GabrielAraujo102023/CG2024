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
    Group(map<char,map<char, float>> transformation, list<string> models, list<Group> groups,
          map<int, vector<vector<float>>> points);
    Group();
    map<char,map<char, float>> transformation;
    list<string> models;
    list<Group> groups;
    map<int, vector<vector<float>>> points;
};


#endif //ENGINE_GROUP_H
