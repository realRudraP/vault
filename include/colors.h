    // Add this to a header file (e.g., ../include/colors.h)

    #ifndef COLORS_H
    #define COLORS_H
    #include<string>
    // ANSI color codes
    namespace Color {
        // Text colors
        const std::string RED = "\033[31m";
        const std::string GREEN = "\033[32m";
        const std::string YELLOW = "\033[33m";
        const std::string BLUE = "\033[34m";
        const std::string MAGENTA = "\033[35m";
        const std::string CYAN = "\033[36m";
        const std::string WHITE = "\033[37m";
        
        // Bright text colors
        const std::string BRIGHT_RED = "\033[91m";
        const std::string BRIGHT_GREEN = "\033[92m";
        const std::string BRIGHT_YELLOW = "\033[93m";
        const std::string BRIGHT_BLUE = "\033[94m";
        const std::string BRIGHT_MAGENTA = "\033[95m";
        const std::string BRIGHT_CYAN = "\033[96m";
        const std::string BRIGHT_WHITE = "\033[97m";
        
        // Text styles
        const std::string BOLD = "\033[1m";
        const std::string UNDERLINE = "\033[4m";
        
        // Reset all formatting
        const std::string RESET = "\033[0m";
    }

    #endif