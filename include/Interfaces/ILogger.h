#ifndef ILOGGER_H
#define ILOGGER_H

#include <string>

using namespace std;

// Giao dien Logger ap dung nguyen ly Dependency Inversion Principle (DIP)
class ILogger {
public:
    virtual ~ILogger() = default;

    // Ghi log cap do thong tin binh thuong
    virtual void logInfo(const string& module, const string& message) = 0;

    // Ghi log cap do canh bao he thong
    virtual void logWarning(const string& module, const string& message) = 0;

    // Ghi log cap do loi he thong
    virtual void logError(const string& module, const string& message) = 0;
};

#endif // ILOGGER_H
