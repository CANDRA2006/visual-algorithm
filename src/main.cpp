#include "core/application.h"
#include <iostream>

int main() {
    try {
        vas::Application app;
        
        if (!app.initialize()) {
            std::cerr << "Failed to initialize application!" << std::endl;
            return -1;
        }
        
        std::cout << "Visual Algorithm Simulator started!" << std::endl;
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}