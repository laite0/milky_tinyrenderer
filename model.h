#ifndef __MODEL_H__
#define __MODEL_H__

#include "vec.h"
#include <vector>

class Model {
private:
    std::vector<Vec3f> vectices;
    std::vector<std::vector<int>> faces;

public:
    explicit Model(const char* filename);
    ~Model();
    int numberOfVertices();
    int numberOfFaces();
    Vec3f vertexAt(int index);
    std::vector<int> faceAt(int index);
};

#endif