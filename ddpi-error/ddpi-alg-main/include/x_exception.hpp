//
// Copyright (c) 2017-2020 Santini Designs. All rights reserved.
//

#pragma once

#include <utility>

#include "throw.hpp"
#include "x_exception_base.hpp"

/**
 * @brief The XException class provides a template for all Eyeris exceptions.
 */
template <typename EXCEPTION>
class [[maybe_unused]] XException : public XExceptionBase
{
public:
    /// Error code type
    using error_code_t = int;

    /**
     * @brief Default constructor.
     *
     * @param[in] severity Severity of the exception
     * @param[in] fileName File name in which the exception has occurred
     * @param[in] line Line number
     * @param[in] function Function in which the exception has occurred
     * @param[in] code Error code for this exception
     * @param[in] arguments Additional arguments for the exception
     */
    template<typename... ARGUMENTS>
    XException(const severity_e severity,
            std::string fileName,
            unsigned int line,
            std::string function,
            const error_code_t code,
            const ARGUMENTS... arguments) :
        XExceptionBase(severity),
        code_(code),
        fileName_(std::move(fileName)),
        line_(line),
        function_(std::move(function))
    {
        initializeStaticMethods();

        auto format = staticMethod_.decoder(code);
        std::string completeMessage = format.empty() ? unknown_exception(code) : std::string(arguments...);

        message_ = where() + " " + completeMessage;
    }

    /**
     * @brief Return the error code of the exception.
     *
     * @return The error code of this exception.
     */
    [[nodiscard]]
    error_code_t getCode() const
    {
        return code_;
    }

    /**
     * @brief Return the file name where the exception was generated.
     *
     * @return The name of the file where the exception was generated.
     */
    [[nodiscard]]
    std::string getFileName() const
    {
        return fileName_;
    }

    /**
     * @brief Return the function where the exception was generated.
     *
     * @return The name of the function where the exception was generated.
     */
    [[maybe_unused]] [[nodiscard]]
    std::string getFunction() const
    {
        return function_;
    }

    /**
     * @brief Return the error code of the exception.
     *
     * @return The error code of this exception.
     */
    [[nodiscard]]
    unsigned int getLine() const
    {
        return line_;
    }

    /// Get the error string for this exception
    [[nodiscard]]
    const char* what() const noexcept override
    {
        return message_.c_str();
    }

protected:
    /**
     * @brief Return an error string (with format) of the specified exception code.
     *
     * @param[in] code Exception code
     *
     * @return An error string with format
     */
    std::string decoder(error_code_t code)
    {
        return staticMethod_.decoder(code);
    }

    /**
     * @brief Get the function/module name in which the exception occurred.
     *
     * @return The function/module name in which the exception occurred.
     */
    [[nodiscard]]
    std::string where() const
    {
        return fileName_ + ": " + std::to_string(line_) + " -- " + function_ + "()";
    }

    /**
     * @brief Return a standard message if the exception is unknown.
     *
     * @param[in] code Error code to return
     *
     * @return Standard unknown exception message
     */
    [[nodiscard]]
    std::string unknown_exception(error_code_t code) const
    {
        return "Unknown error code [" + std::to_string(code) + "]";
    }

private:
    struct static_methods_t
    {
        std::string (*decoder)(int);
    };

    error_code_t code_;  ///< Error code of this exception

    std::string fileName_;  ///< Name of the file in which the exception has occurred
    std::string function_;  ///< Name of the function in which the exception has occurred

    unsigned int line_;  ///< Line number of the exception

    static std::string message_;  ///< Final message of the exception

    static_methods_t staticMethod_;

    /// Initialize the static methods.
    void initializeStaticMethods()
    {
        staticMethod_.decoder = &EXCEPTION::decoder;
    }
};

template <typename EXCEPTION> [[maybe_unused]] std::string XException<EXCEPTION>::message_;

