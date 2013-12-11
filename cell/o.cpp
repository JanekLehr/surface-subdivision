#ifdef _ABIO32		/* we're on an SGI, compiling -o32, so no bool! */
    typedef enum {false, true} bool;
#endif

bool foo;
