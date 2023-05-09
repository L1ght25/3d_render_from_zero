#include "bitmap.h"

rendering::Bitmap::Bitmap(int width, int height) : width_(width), height_(height), pixels_(width * height * 4) {
}

std::vector<sf::Uint8> rendering::Bitmap::Fill(sf::Uint8 color) {
    std::fill(pixels_.begin(), pixels_.end(), color);
    return pixels_;
}

void rendering::Bitmap::SetPixel(int x, int y, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a) {
    int ind = (x + y * width_) * 4;
    pixels_[ind] = r;
    pixels_[ind + 1] = g;
    pixels_[ind + 2] = b;
    pixels_[ind + 3] = a;
}

void rendering::Bitmap::CopyPixel(const Bitmap& map, int cur_x, int cur_y, int x, int y) {
    int ind = (x + y * map.width_) * 4;
    SetPixel(cur_x, cur_y, map.pixels_[ind], map.pixels_[ind + 1], map.pixels_[ind + 2], map.pixels_[ind + 3]);
}

auto rendering::Bitmap::GetPointerToPixels() -> const sf::Uint8* {
    return pixels_.data();
}

int rendering::Bitmap::Width() const {
    return width_;
}

int rendering::Bitmap::Height() const {
    return height_;
}

bool rendering::Bitmap::IsInsideMap(int x, int y) const {
    return 0 <= x and x < width_ and 0 <= y and y < height_;
}