#include "../../include/Utils/ConfigManager.h"
#include "../../include/Exceptions/LibraryException.h"
#include <fstream>
#include <sstream>

using namespace std;

// Khoi tao gia tri mac dinh cho cac tham so he thong
ConfigManager::ConfigManager() {
    finePerDay = 5000.0;
    maxBorrowDays = 14;
    hashTableSize = 101;
    minYear = 1900;
    maxYear = 2026;

    booksPath = "data/books.txt";
    readersPath = "data/readers.txt";
    transactionsPath = "data/transactions.txt";
    waitlistQueuePath = "data/waitlist_queue.txt";
    historyStackPath = "data/history_stack.txt";
    systemLogPath = "data/system.log";
    configPath = "data/config.ini";

    // Tu dong doc file cau hinh neu file da ton tai
    try {
        loadFromFile("data/config.ini");
    } catch (...) {
        // Neu chua co file thi giu nguyen cac gia tri mac dinh
    }
}

// Phuong thuc tinh tra ve the hien duy nhat cua ConfigManager
ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

// Ham xoa khoang trang thua o dau va cuoi chuoi
string ConfigManager::trim(const string& chuoi) {
    int batDau = 0;
    while (batDau < (int)chuoi.length() && (chuoi[batDau] == ' ' || chuoi[batDau] == '\t' || chuoi[batDau] == '\r' || chuoi[batDau] == '\n')) {
        batDau++;
    }

    int ketThuc = (int)chuoi.length() - 1;
    while (ketThuc >= batDau && (chuoi[ketThuc] == ' ' || chuoi[ketThuc] == '\t' || chuoi[ketThuc] == '\r' || chuoi[ketThuc] == '\n')) {
        ketThuc--;
    }

    if (batDau > ketThuc) {
        return "";
    }
    return chuoi.substr(batDau, ketThuc - batDau + 1);
}

// Doc file cau hinh data/config.ini theo tung dong
void ConfigManager::loadFromFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        throw FileIOException("Khong the mo file cau hinh de doc: " + filePath, "ConfigManager");
    }

    configPath = filePath;
    string dong;
    while (getline(file, dong)) {
        string dongDaXoaKhoangTrang = trim(dong);

        // Bo qua dong trong hoac dong chu thich bat dau bang # hoac ;
        if (dongDaXoaKhoangTrang.empty() || dongDaXoaKhoangTrang[0] == '#' || dongDaXoaKhoangTrang[0] == ';') {
            continue;
        }

        // Tim vi tri dau '=' ngan cach giua Key va Value
        size_t viTriDauBang = dongDaXoaKhoangTrang.find('=');
        if (viTriDauBang == string::npos) {
            continue;
        }

        string khoa = trim(dongDaXoaKhoangTrang.substr(0, viTriDauBang));
        string giaTri = trim(dongDaXoaKhoangTrang.substr(viTriDauBang + 1));

        // Gan gia tri vao tung thuoc tinh tuong ung
        if (khoa == "FINE_PER_DAY") {
            try {
                finePerDay = stod(giaTri);
            } catch (...) {}
        } else if (khoa == "MAX_BORROW_DAYS") {
            try {
                maxBorrowDays = stoi(giaTri);
            } catch (...) {}
        } else if (khoa == "HASH_TABLE_SIZE") {
            try {
                hashTableSize = stoi(giaTri);
            } catch (...) {}
        } else if (khoa == "MIN_YEAR") {
            try {
                minYear = stoi(giaTri);
            } catch (...) {}
        } else if (khoa == "MAX_YEAR") {
            try {
                maxYear = stoi(giaTri);
            } catch (...) {}
        } else if (khoa == "BOOKS_PATH" || khoa == "BOOKS_FILE") {
            if (!giaTri.empty()) booksPath = giaTri;
        } else if (khoa == "READERS_PATH" || khoa == "READERS_FILE") {
            if (!giaTri.empty()) readersPath = giaTri;
        } else if (khoa == "TRANSACTIONS_PATH" || khoa == "TRANSACTIONS_FILE") {
            if (!giaTri.empty()) transactionsPath = giaTri;
        } else if (khoa == "WAITLIST_QUEUE_PATH" || khoa == "WAITLIST_QUEUE_FILE") {
            if (!giaTri.empty()) waitlistQueuePath = giaTri;
        } else if (khoa == "HISTORY_STACK_PATH" || khoa == "HISTORY_STACK_FILE") {
            if (!giaTri.empty()) historyStackPath = giaTri;
        } else if (khoa == "SYSTEM_LOG_PATH" || khoa == "SYSTEM_LOG_FILE") {
            if (!giaTri.empty()) systemLogPath = giaTri;
        }
    }
    file.close();
}

// Luu cac thiet lap cau hinh hien tai xuong file
void ConfigManager::saveToFile(const string& filePath) {
    ofstream file(filePath);
    if (!file.is_open()) {
        throw FileIOException("Khong the mo file cau hinh de ghi: " + filePath, "ConfigManager");
    }

    configPath = filePath;
    file << "# Cau hinh tham so he thong Quan ly Thu vien PBL 2\n";
    file << "FINE_PER_DAY=" << (long long)finePerDay << "\n";
    file << "MAX_BORROW_DAYS=" << maxBorrowDays << "\n";
    file << "HASH_TABLE_SIZE=" << hashTableSize << "\n";
    file << "MIN_YEAR=" << minYear << "\n";
    file << "MAX_YEAR=" << maxYear << "\n";
    file << "BOOKS_PATH=" << booksPath << "\n";
    file << "READERS_PATH=" << readersPath << "\n";
    file << "TRANSACTIONS_PATH=" << transactionsPath << "\n";
    file << "WAITLIST_QUEUE_PATH=" << waitlistQueuePath << "\n";
    file << "HISTORY_STACK_PATH=" << historyStackPath << "\n";
    file << "SYSTEM_LOG_PATH=" << systemLogPath << "\n";

    file.close();
}

// Getter va Setter
double ConfigManager::getFinePerDay() const {
    return finePerDay;
}

void ConfigManager::setFinePerDay(double value) {
    finePerDay = value;
}

int ConfigManager::getMaxBorrowDays() const {
    return maxBorrowDays;
}

void ConfigManager::setMaxBorrowDays(int value) {
    maxBorrowDays = value;
}

int ConfigManager::getHashTableSize() const {
    return hashTableSize;
}

void ConfigManager::setHashTableSize(int value) {
    hashTableSize = value;
}

int ConfigManager::getMinYear() const {
    return minYear;
}

void ConfigManager::setMinYear(int value) {
    minYear = value;
}

int ConfigManager::getMaxYear() const {
    return maxYear;
}

void ConfigManager::setMaxYear(int value) {
    maxYear = value;
}

const string& ConfigManager::getBooksPath() const {
    return booksPath;
}

void ConfigManager::setBooksPath(const string& path) {
    booksPath = path;
}

const string& ConfigManager::getReadersPath() const {
    return readersPath;
}

void ConfigManager::setReadersPath(const string& path) {
    readersPath = path;
}

const string& ConfigManager::getTransactionsPath() const {
    return transactionsPath;
}

void ConfigManager::setTransactionsPath(const string& path) {
    transactionsPath = path;
}

const string& ConfigManager::getWaitlistQueuePath() const {
    return waitlistQueuePath;
}

void ConfigManager::setWaitlistQueuePath(const string& path) {
    waitlistQueuePath = path;
}

const string& ConfigManager::getHistoryStackPath() const {
    return historyStackPath;
}

void ConfigManager::setHistoryStackPath(const string& path) {
    historyStackPath = path;
}

const string& ConfigManager::getSystemLogPath() const {
    return systemLogPath;
}

void ConfigManager::setSystemLogPath(const string& path) {
    systemLogPath = path;
}

const string& ConfigManager::getConfigPath() const {
    return configPath;
}

void ConfigManager::setConfigPath(const string& path) {
    configPath = path;
}
