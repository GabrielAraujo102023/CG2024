#include "Group.h"
#include <utility>

Group::Group() {
    this->transformation = {};
    this->models = {};
    this->groups = {};
}

Group::Group(map<char, map<char, float>> transformation, list<string> models, list<Group> groups) {
    this->transformation = std::move(transformation);
    this->models = std::move(models);
    this->groups = std::move(groups);
}
