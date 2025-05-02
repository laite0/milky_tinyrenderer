#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : vectices(), faces() {
    std::ifstream input;
    input.open(filename, std::ifstream::in);
    if (input.fail()) {
        return;
    }

    std::string line;
    while (!input.eof()) {
        std::getline(input, line);

        std::istringstream iss(line);
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (float &i: v.raw) {
                iss >> i;
            }
            vectices.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
            }
            faces.push_back(f);
        }
    }
    std::cerr << "# v# " << vectices.size() << " f# "  << faces.size() << std::endl;
}

Model::~Model() = default;

int Model::numberOfVertices() {
    return vectices.size();
}

int Model::numberOfFaces() {
    return faces.size();
}

Vec3f Model::vertexAt(int index) {
    return vectices[index];
}

std::vector<int> Model::faceAt(int index) {
    return faces[index];
}

