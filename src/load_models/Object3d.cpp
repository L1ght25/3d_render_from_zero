#include "Object3d.h"

unsigned int load::countWordsInString(std::string const& str) {
    std::stringstream stream(str);
    return std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>());
}

load::Object3d::Object3d(std::string_view filename) {
    std::vector<geometry::Vector4d> texture_coords_;
    std::ifstream in(filename, std::ios::in);
    if (!in) {
        throw std::runtime_error("Cannot read file");
    }
    std::string curr_line;
    std::array<int, 4> vertices;
    std::array<int, 4>texture;
    std::array<int, 4> tmp;  // ignore for good times

    while (getline(in, curr_line)) {
        if (curr_line.starts_with("v ") or curr_line.starts_with("vt ")) {
            std::istringstream in_vert(curr_line.substr(2));
            double x, y, z;
            in_vert >> x;
            in_vert >> y;
            if (curr_line.starts_with("v ")) {
                in_vert >> z;
                vertices_.emplace_back(x, y, z);
            }
            else {
                texture_coords_.emplace_back(x, y, 0, 0);
            }
        } else if(curr_line.starts_with("f ")) {
            bool has_4_dim = false;
            if (countWordsInString(curr_line) == 5) {
                sscanf(curr_line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i",
                        &vertices[0], &texture[0], &tmp[0],
                        &vertices[1], &texture[1], &tmp[1],
                        &vertices[2], &texture[2], &tmp[2],
                        &vertices[3], &texture[3], &tmp[3]);
                has_4_dim = true;
            } else {
                sscanf(curr_line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i",
                        &vertices[0], &texture[0], &tmp[0],
                        &vertices[1], &texture[1], &tmp[1],
                        &vertices[2], &texture[2], &tmp[2]);
            }
            for (int i = 0; i < (has_4_dim ? 4 : 3); ++i) {
                if (i == 3) {
                    if (vertices[i] < 1) {
                        break;
                    }
                    indexes_.push_back(vertices[0]);
                    indexes_.push_back(vertices[2]);
                }
                indexes_.push_back(--vertices[i]);
                if (texture[i] > 0) {
                    vertices_[vertices[i]].SetTexturePos(texture_coords_[--texture[i]]);
                }
            }
        }
    }
}
