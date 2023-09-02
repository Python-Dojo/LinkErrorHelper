
#ifndef LINK_TO_LINK_ERROR_HELPER_STATICALLY // if linking statically ( built as lib) avoid exports
    // Define normal dll export macros
    #ifdef LINKERRORHELPERTOOL_EXPORTS
        #define LINKERRORHELPERTOOL_API __declspec(dllexport)
    #else
        #define LINKERRORHELPERTOOL_API __declspec(dllimport)
    #endif
#else 
    #define LINKERRORHELPERTOOL_API // No exports (linking statically)
#endif