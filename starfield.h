#include <random>
#include "bitmap.h"
#include "RenderContext.h"
#include <cmath>

class Stars3d {
public:
    explicit Stars3d(int num_of_stars, double speed, double spread) : stars_(num_of_stars), speed_(speed),
    spread_(spread), rng_(dev_()), rand_coord_(0, 1) {
        for (size_t i = 0; i < num_of_stars; ++i) {
            InitStar(i);
        }
    }

    void Update(Bitmap& field, double delta) {
        field.Fill(0);

        double half_width = static_cast<double>(field.Width()) / 2;
        double half_height = static_cast<double>(field.Height()) / 2;

        for (auto &star : stars_) {
            star.z -= delta * speed_;
            if (star.z <= 0) {
                InitStar(star);
            }
            int x = static_cast<int>(half_width * (star.x / (star.z * tan_of_view_) + 1));
            int y = static_cast<int>(half_height * (star.y / (star.z * tan_of_view_) + 1));
            if (!field.IsInsideMap(x, y)) {
                InitStar(star);
            } else {
                field.SetPixel(x, y, 255, 255 * star.z, 255, 255 * star.z);
            }
        }
    }

    void Update(RenderContext& field, double delta) {
        field.Fill(0);
        double half_width = static_cast<double>(field.Width()) / 2;
        double half_height = static_cast<double>(field.Height()) / 2;
        std::vector<Vertex> dots;
        for (auto &star : stars_) {
            star.z -= delta * speed_;
            if (star.z <= 0) {
                InitStar(star);
//                field.ClearBuffer();
            }
            int x = static_cast<int>(half_width * (star.x / (star.z * tan_of_view_) + 1));
            int y = static_cast<int>(half_height * (star.y / (star.z * tan_of_view_) + 1));
            if (!field.IsInsideMap(x, y)) {
                InitStar(star);
//                field.ClearBuffer();
//                dots.push_back({star.x, star.y});
            } else {
//                field.SetPixel(x, y, 255, 255 * star.z, 255, 255 * star.z);
//                    dots.push_back({star.x / (star.z * tan_of_view_), star.y / (star.z * tan_of_view_)});
            }
//            dots.push_back({static_cast<double>(x), static_cast<double>(y)});
//            dots.push_back({x, y});
        }
//        int y_min = std::min({dots[0].GetY(), dots[1].GetY(), dots[2].GetY()});
//        int y_max = std::max({dots[0].GetY(), dots[1].GetY(), dots[2].GetY()}) + 1;
        field.FillTriangle(dots[0], dots[1], dots[2]);
//        field.FillPixels(y_min, y_max);
//        field.ClearBuffer();
    }

private:
    void InitStar(size_t i) {
        stars_[i].x = 2 * (rand_coord_(rng_) - 0.5) * spread_;
        stars_[i].y = 2 * (rand_coord_(rng_) - 0.5) * spread_;
        stars_[i].z = (rand_coord_(rng_) + 1e-9) * spread_;
    }

    struct Star{
        double x;
        double y;
        double z;
    };

    void InitStar(Star& star) {
        star.x = 2 * (rand_coord_(rng_) - 0.5) * spread_;
        star.y = 2 * (rand_coord_(rng_) - 0.5) * spread_;
        star.z = (rand_coord_(rng_) + 1e-8) * spread_;
    }

    std::vector<Star> stars_;
    double speed_;
    double spread_;
    double tan_of_view_ = tan(90. / 2);

    std::random_device dev_;
    std::mt19937 rng_;
    std::uniform_real_distribution<double> rand_coord_;
};