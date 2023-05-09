#include "src/user_api/api.h"

int main() {
    auto executor = api::View("../models/monkey.obj", "../textures/wood_texture2.jpeg", 1000, 1000);
    executor.Execute();
    return 0;
}
