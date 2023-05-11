#include "src/camera/Camera.h"
#include "src/user_api/api.h"

int main() {
    auto executor = api::View(constants::DEFAULT_WIDTH, constants::DEFAULT_HEIGHT);
    executor.LoadModel("../models/smooth_monkey.obj", "../textures/gradient.jpeg", true, false);
    executor.LoadModel("../models/10438_Circular_Grass_Patch_v1_iterations-2.obj", "../textures/10438_Circular_Grass_Patch_v1_Diffuse.jpg", false, true, 0, -12, 0);
    executor.LoadModel("../models/Wood_Table.obj", "../textures/large_wood_table_with_glasplatte_3d_model_3ds_fbx_obj_blend_dae__2843409e-fba2-4baa-b817-88c046efa70c.jpg", false, false, 1, -2.5, 0);
    executor.LoadModel("../models/cottage_obj.obj", "../textures/cottage_diffuse.png", false, false, -20, -2, 0);
    executor.Execute();
    return 0;
}
