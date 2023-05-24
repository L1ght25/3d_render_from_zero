#pragma once
#include <SFML/Graphics/Export.hpp>
#include <vector>
#include <algorithm>

namespace rendering {
class Bitmap {
  public:
    using Uint8 = sf::Uint8;

    Bitmap() = default;

    Bitmap(int width, int height);

    void Fill(Uint8 color);

    void SetPixel(int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

    void CopyPixel(const Bitmap& map, int cur_x, int cur_y, int x, int y, double light);

    const Uint8* GetPointerToPixels();

    int Width() const;

    int Height() const;

    bool IsInsideMap(int x, int y) const;

  protected:

    int GetIndByCoords(int x, int y) const;
    int width_;
    int height_;
    std::vector<Uint8> pixels_;
};
}  // namespace rendering
