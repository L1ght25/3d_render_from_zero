#pragma once
#include <array>
#include <fstream>
#include <sstream>
#include "../geometric_primitives/Vertex.h"

namespace load {
unsigned int countWordsInString(std::string const& str);

class Object3d {
public:
using Vertex = geometry::Vertex;
    Object3d(std::string_view filename);

    const Vertex& GetVertexByInd(int i) const {
        return vertices_[indexes_[i]];
    }

    size_t SizeOfPolygons() const {
        return indexes_.size();
    }

private:
    std::vector<Vertex> vertices_;
    std::vector<size_t>indexes_;
};
}