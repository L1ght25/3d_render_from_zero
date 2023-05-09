#include "src/user_api/api.h"

int main() {
    auto executor = api::View("../models/monkey.obj", "../textures/gradient.jpeg", 1000, 1000);
    executor.Execute(false);
    return 0;
}
