#include "src/load_models/Object3d.h"
#include "src/rendering/bitmap/bitmap.h"
#include "src/user_api/api.h"
#include <exception>

int main() {
    try {
        auto executor = api::Application(
            {
                {"../models/smooth_monkey.obj", "../textures/gradient.jpeg", model::IsAutoRotated},
                {"../models/10438_Circular_Grass_Patch_v1_iterations-2.obj", "../textures/10438_Circular_Grass_Patch_v1_Diffuse.jpg", model::Static, model::IsInvertedZComp, 0, -12, 0},
                {"../models/Wood_Table.obj", "../textures/large_wood_table_with_glasplatte_3d_model_3ds_fbx_obj_blend_dae__2843409e-fba2-4baa-b817-88c046efa70c.jpg", model::Static, model::IsNotInversedZComp, 1, -2.5, 0},
                {"../models/cottage_obj.obj", "../textures/cottage_diffuse.png", model::Static, model::IsNotInversedZComp, -20, -2, 0}
            }, constants::default_width, constants::default_height);
        executor.Execute();
    } catch(const std::exception& exc) {
        std::cerr << exc.what() << std::endl;
    }
}
