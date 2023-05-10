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
    Object3d(std::string_view filename, bool is_auto_rotation = false);

    const Vertex& GetVertexByInd(int i) const;

    size_t SizeOfPolygons() const;

    bool IsAutoTransformed() const;

private:
    std::vector<Vertex> vertices_;
    std::vector<size_t>indexes_;
    bool is_auto_rotation_;
};
}