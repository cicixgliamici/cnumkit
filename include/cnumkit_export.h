#ifndef CNUMKIT_EXPORT_H
#define CNUMKIT_EXPORT_H

#if defined(_WIN32) && defined(CNUMKIT_SHARED)
    #if defined(cnumkit_EXPORTS)
        #define CNUMKIT_EXPORT __declspec(dllexport)
    #else
        #define CNUMKIT_EXPORT __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #define CNUMKIT_EXPORT __attribute__((visibility("default")))
#else
    #define CNUMKIT_EXPORT
#endif

#endif /* CNUMKIT_EXPORT_H */
