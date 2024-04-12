#include "Group.h"
#include <utility>

Group::Group() {
    this->transformation = {};
    this->models = {};
    this->groups = {};
    this->points = {};
}

Group::Group(map<char, map<char, float>> transformation, list<string> models, list<Group> groups,
             map<int, vector<float>> points) {
    this->transformation = std::move(transformation);
    this->models = std::move(models);
    this->groups = std::move(groups);
    this->points = std::move(points);
}
