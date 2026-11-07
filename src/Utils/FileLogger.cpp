#include "../../include/Utils/FileLogger.h"
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;

// Khoi tao duong dan mac dinh cho file nhat ky
FileLogger::FileLogger() {
    logFilePath = "data/system.log";
}

// Tra ve the hien duy nhat cua FileLogger
FileLogger& FileLogger::getInstance() {
    static FileLogger instance;
    return instance;
}

// Lay thoi gian hien tai theo dinh dang [YYYY-MM-DD HH:MM:SS]
string FileLogger::getCurrentTimestamp() {
    time_t hienTai = time(nullptr);
    tm thoiGian{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&thoiGian, &hienTai);
#else
    localtime_r(&hienTai, &thoiGian);
#endif
    char boDem[30];
    strftime(boDem, sizeof(boDem), "%Y-%m-%d %H:%M:%S", &thoiGian);
    return string(boDem);
}

// Ghi mot dong nhat ky vao cuoi file (che do append)
void FileLogger::writeLog(const string& level, const string& module, const string& message) {
    ofstream file(logFilePath, ios::app);
    if (file.is_open()) {
        file << "[" << getCurrentTimestamp() << "] [" << level << "] [" << module << "] " << message << "\n";
        file.flush();
        file.close();
    }
}

void FileLogger::logInfo(const string& module, const string& message) {
    writeLog("INFO", module, message);
}

void FileLogger::logWarning(const string& module, const string& message) {
    writeLog("WARNING", module, message);
}

void FileLogger::logError(const string& module, const string& message) {
    writeLog("ERROR", module, message);
}

// Doc toi da maxLines dong log gan nhat tu file system.log
string FileLogger::readRecentLogs(int maxLines) {
    ifstream file(logFilePath);
    if (!file.is_open()) {
        return "";
    }

    // Buoc 1: Dem tong so dong hien co trong file log
    int tongSoDong = 0;
    string dong;
    while (getline(file, dong)) {
        tongSoDong++;
    }

    // Buoc 2: Dong file va mo lai de doc tu dau (dam bao reset toan bo co stream va vi tri doc)
    file.close();
    file.open(logFilePath);
    if (!file.is_open()) {
        return "";
    }

    // Tinh so dong can bo qua de lay duoc maxLines dong cuoi cung
    int soDongBoQua = 0;
    if (tongSoDong > maxLines) {
        soDongBoQua = tongSoDong - maxLines;
    }

    int dongHienTai = 0;
    ostringstream oss;
    while (getline(file, dong)) {
        if (dongHienTai >= soDongBoQua) {
            oss << dong << "\n";
        }
        dongHienTai++;
    }

    file.close();
    return oss.str();
}

const string& FileLogger::getLogFilePath() const {
    return logFilePath;
}

void FileLogger::setLogFilePath(const string& path) {
    logFilePath = path;
}
