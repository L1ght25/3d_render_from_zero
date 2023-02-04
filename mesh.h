//#pragma once
//#include <vector>
//#include <string_view>
//#include <fstream>
//#include <sstream>
//
//#include "RenderContext.h"
//
//class Object3d {
//public:
//    Object3d(std::string_view filename) {
//        std::ifstream in(filename, std::ios::in);
//        if (!in) {
//            throw std::runtime_error("Cannot read file");
//        }
//        std::string curr_line;
//
//        while (getline(in, curr_line)) {
//            if (curr_line.starts_with("v ")) {
//                std::istringstream in_vert(curr_line.substr(2));
//                double x, y, z;
//                in_vert >> x;
//                in_vert >> y;
//                in_vert >> z;
//                vertices_.push_back({x, y, z, {0, 0, 0, 0}});
////            } else if(curr_line.starts_with("vt") or curr_line.starts_with("vn")) {
////                continue;
//            } else if(curr_line.starts_with("f ")) {
//                int a, b, c;  // vertices
//                int A, B, C;  // texture
//                int AA, BB, CC; // ignore for good times
//                const char *curr_str = curr_line.c_str();
//                sscanf(curr_str, "f %i/%i/%i %i/%i/%i %i/%i/%i", &a, &A, &AA, &b, &B, &BB, &c, &C, &CC);
//                --a, --b, --c, --A, --B, --C;
//                indexes_.push_back(a);
//                indexes_.push_back(b);
//                indexes_.push_back(c);
//            }
//        }
//    }
//
//    const Vertex& GetVertexByInd(int i) const {
//        return vertices_[indexes_[i]];
//    }
//
//    size_t SizeOfPolygons() const {
//        return vertices_.size();
//    }
//
//private:
//    std::vector<Vertex> vertices_;
//    std::vector<size_t>indexes_;
//};