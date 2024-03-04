#ifndef INC_1_READER_H
#define INC_1_READER_H
#include "tinyxml.h"

class Reader{
public:
    explicit Reader(char* filename);
    void getWindow(int &width, int &heigh) const;
    void getCamera(float &posX, float &posY, float &posZ, float &lookX, float &lookY, float &lookZ,
                     float &upX, float &upY, float &upZ, float &fov, float &near, float &far) const;
    [[nodiscard]] std::list<std::string> getModels() const;
    TiXmlElement* WORLD;
};
#endif //INC_1_READER_H
