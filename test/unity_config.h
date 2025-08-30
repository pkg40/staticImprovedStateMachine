#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

// Unity Configuration for State Machine Testing

// Enable floating point support
#ifndef UNITY_INCLUDE_DOUBLE
#define     UNITY_INCLUDE_DOUBLE
#define UNITY_DOUBLE_PRECISION 1e-12f
#endif


// Enable formatted print support
#ifndef UNITY_INCLUDE_PRINT_FORMATTED
#define     UNITY_INCLUDE_PRINT_FORMATTED
#endif

// Max string lengths for better error messages
#define UNITY_MAX_DETAILS 256

#endif // UNITY_CONFIG_H
