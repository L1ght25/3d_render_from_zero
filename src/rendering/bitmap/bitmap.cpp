#include "bitmap.h"

rendering::Bitmap::Bitmap(int width, int height) : width_(width), height_(height), pixels_(width * height * 4) {
}

void rendering::Bitmap::Fill(Uint8 color) {
    std::fill(pixels_.begin(), pixels_.end(), color);
}

void rendering::Bitmap::SetPixel(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int ind = GetIndByCoords(x, y);
    pixels_[ind] = r;
    pixels_[ind + 1] = g;
    pixels_[ind + 2] = b;
    pixels_[ind + 3] = a;
}

void rendering::Bitmap::CopyPixel(const Bitmap& map, int cur_x, int cur_y, int x, int y, double light) {
    int ind = map.GetIndByCoords(x, y);
    SetPixel(cur_x, cur_y, light * map.pixels_[ind], light * map.pixels_[ind + 1], light * map.pixels_[ind + 2], light * map.pixels_[ind + 3]);
}

auto rendering::Bitmap::GetPointerToPixels() -> const Uint8* {
    return pixels_.data();
}

int rendering::Bitmap::Width() const {
    return width_;
}

int rendering::Bitmap::Height() const {
    return height_;
}

bool rendering::Bitmap::IsInsideMap(int x, int y) const {
    return 0 <= x && x < width_ && 0 <= y && y < height_;
}

int rendering::Bitmap::GetIndByCoords(int x, int y) const {
    return (x + y * width_) * 4;
}
