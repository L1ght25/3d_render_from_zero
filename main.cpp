#include "src/user_api/api.h"

int main() {
    auto executor = api::View(1000, 1000);
    // executor.LoadModel("../models/monkey.obj", "../textures/gradient.jpeg", false);
    executor.LoadModel("../models/Wood_Table.obj", "../textures/large_wood_table_with_glasplatte_3d_model_3ds_fbx_obj_blend_dae__2843409e-fba2-4baa-b817-88c046efa70c.jpg");
    executor.Execute();
    return 0;
}
