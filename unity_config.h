#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

// Unity Configuration for State Machine Testing

// Enable floating point support
#ifndef UNITY_INCLUDE_DOUBLE
#define     UNITY_INCLUDE_DOUBLE
#define     UNITY_DOUBLE_PRECISION 1e-12f
#endif

// Enable formatted print support
#ifndef UNITY_INCLUDE_PRINT_FORMATTED
#define     UNITY_INCLUDE_PRINT_FORMATTED
#endif

// Color output for terminal
#define UNITY_OUTPUT_COLOR

// Increase test buffer size for larger tests
#define UNITY_OUTPUT_CHAR_HEADER_DECLARATION
#define UNITY_OUTPUT_CHAR_HEADER_DECLARATION extern void UnityOutputChar(char c);

// Memory allocation test support
#define UNITY_INCLUDE_SETUP_STUBS

// Support for pointer comparison
#define UNITY_SUPPORT_64

// Max string lengths for better error messages
#define UNITY_MAX_DETAILS 256

#endif // UNITY_CONFIG_H
