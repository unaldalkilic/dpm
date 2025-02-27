#ifndef DPM_EXCEPTIONS_HPP
#define DPM_EXCEPTIONS_HPP

#include <exception>
#include <string>

// Base class for all custom exceptions
class DPMException : public std::exception {
private:
    std::string message;

public:
    explicit DPMException(const std::string& msg) : message(msg) {}

    // Override what() and mark it noexcept
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// Derived exception classes with proper constructors
class InvalidParameter : public DPMException {
public:
    explicit InvalidParameter(const std::string& msg) : DPMException(msg) {}
};

class TargetNotExist : public DPMException {
public:
    explicit TargetNotExist(const std::string& msg) : DPMException(msg) {}
};

class InvalidPackage : public DPMException {
public:
    explicit InvalidPackage(const std::string& msg) : DPMException(msg) {}
};

class UnknownPlatform : public DPMException {
public:
    explicit UnknownPlatform(const std::string& msg) : DPMException(msg) {}
};

#endif
