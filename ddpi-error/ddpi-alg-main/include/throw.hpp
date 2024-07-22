#pragma once

// Define few macros for a simpler exception usage
#define THROW_CRITICAL_ARGS(X, CODEX, ARGS...) \
    throw X( \
        basic::exception::XException<X>::CRITICAL, \
        __FILE__, \
        __LINE__, \
        __FUNCTION__, \
        CODEX, \
        ARGS);

// Define few macros for a simpler exception usage
#define THROW_CRITICAL_NOARGS(X, CODEX) \
    throw X( \
        basic::exception::XException<X>::CRITICAL, \
        __FILE__, \
        __LINE__, \
        __FUNCTION__, \
        CODEX);

#define THROW_ERROR_ARGS(X, CODEX, ARGS...) \
    throw X( \
        XException<X>::ERROR, \
        __FILE__, \
        __LINE__, \
        __FUNCTION__, \
        CODEX, \
        ARGS);

#define THROW_ERROR_NOARGS(X, CODEX) \
    throw X( \
        basic::exception::XException<X>::ERROR, \
        __FILE__, \
        __LINE__, \
        __FUNCTION__, \
        CODEX);
