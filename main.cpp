#include "src/user_api/api.h"

int main() {
    auto executor = api::View("../models/monkey.obj", "../textures/gradient.jpeg");
    executor.Execute();
    return 0;
}
