#pragma once

// Don't think we need this, dumpbin is currently only on windows anyway
#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
    // Windows Header Files
    #include <windows.h>
#endif