//
// Copyright (c) 2017-2020 Santini Designs. All rights reserved.
//

#pragma once

#include <string>
#include <stdexcept>


/**
 * @brief The x-exception base class provides the base class for all Eyeris exceptions.
 */
class [[maybe_unused]] XExceptionBase : public std::runtime_error
{
public:
    /// Exception severity
    enum severity_e
    {
        CRITICAL,  ///< The system will terminate with a core dump
        ERROR  ///< The system will terminate the experiment
    };

    /**
     * @brief Default constructor.
     *
     * @param[in] severity Severity of the exception
     */
    explicit XExceptionBase(severity_e severity) :
        std::runtime_error(""),
        severity_(severity)
    {
        // Nothing to do
    }

    /**
     * @brief Return the severity of the exception.
     *
     * @return The severity of this exception.
     */
    [[maybe_unused]] [[nodiscard]]
    severity_e getSeverity() const
    {
        return severity_;
    }

private:
    severity_e severity_;  ///< Severity of the exception
};
