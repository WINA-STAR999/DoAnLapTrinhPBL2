#ifndef FILE_LOGGER_H
#define FILE_LOGGER_H

#include "../Interfaces/ILogger.h"
#include <string>

using namespace std;

// Lop FileLogger ghi nhat ky he thong ra file system.log (Meyers' Singleton)
class FileLogger : public ILogger {
private:
    string logFilePath; // Duong dan den file log (mac dinh data/system.log)

    // Ham tao private de tranh tao doi tuong tuy tien ben ngoai
    FileLogger();

    // Ham phu tro ghi dong log theo dung dinh dang vao file
    void writeLog(const string& level, const string& module, const string& message);

    // Ham lay thoi gian hien tai theo dinh dang YYYY-MM-DD HH:MM:SS
    static string getCurrentTimestamp();

public:
    // Phuong thuc tinh tra ve the hien duy nhat
    static FileLogger& getInstance();

    // Xoa ham tao sao chep va phep gan
    FileLogger(const FileLogger&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;

    // Ghi de cac phuong thuc trong interface ILogger
    void logInfo(const string& module, const string& message) override;
    void logWarning(const string& module, const string& message) override;
    void logError(const string& module, const string& message) override;

    // Doc nguoc maxLines dong log gan nhat phuc vu man hinh quan tri SCR-A4
    string readRecentLogs(int maxLines = 50);

    // Getter va Setter cho duong dan file log
    const string& getLogFilePath() const;
    void setLogFilePath(const string& path);
};

#endif // FILE_LOGGER_H
