#include "core/application.h"
#include "core/logger.h"
#include <iostream>
#include <exception>

int main()
{
    //  Logger setup  
    vas::Logger::instance().setLevel(vas::LogLevel::DEBUG);
    vas::Logger::instance().setOutputFile("vas_log.txt");

    VAS_LOG_INFO("=== Visual Algorithm Simulator v2.0 ===");

    try {
        vas::Application app;

        if (!app.initialize()) {
            VAS_LOG_ERROR("Initialization failed — aborting");
            return EXIT_FAILURE;
        }

        app.run();
    }
    catch (const std::exception& ex) {
        VAS_LOG_ERROR(std::string("Unhandled exception: ") + ex.what());
        std::cerr << "[FATAL] " << ex.what() << "\n";
        return EXIT_FAILURE;
    }
    catch (...) {
        VAS_LOG_ERROR("Unknown exception caught in main");
        return EXIT_FAILURE;
    }

    VAS_LOG_INFO("Application exited cleanly");
    return EXIT_SUCCESS;
}
