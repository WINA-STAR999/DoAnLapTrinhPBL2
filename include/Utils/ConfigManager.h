#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>

using namespace std;

// Lop ConfigManager quan ly cac tham so cau hinh toan cuc (Meyers' Singleton)
class ConfigManager {
private:
    double finePerDay;          // Tien phat moi ngay tre han (mac dinh 5000 VND)
    int maxBorrowDays;          // So ngay muon toi da cho mot luot muon (mac dinh 14 ngay)
    int hashTableSize;          // Kich thuoc khoi tao cho bang bam HashTable (mac dinh 101)
    int minYear;                // Nam xuat ban toi thieu hop le (1900)
    int maxYear;                // Nam xuat ban toi da hop le (2026)

    // Duong dan den cac file du lieu
    string booksPath;           // Duong dan file books.txt
    string readersPath;         // Duong dan file readers.txt
    string transactionsPath;    // Duong dan file transactions.txt
    string waitlistQueuePath;   // Duong dan file waitlist_queue.txt
    string historyStackPath;    // Duong dan file history_stack.txt
    string systemLogPath;       // Duong dan file system.log
    string configPath;          // Duong dan file config.ini

    // Ham tao private theo dung mau thiet ke Singleton
    ConfigManager();

    // Ham phu tro cat khoang trang o hai dau chuoi
    static string trim(const string& chuoi);

public:
    // Phuong thuc tinh tra ve the hien duy nhat (Meyers' Singleton)
    static ConfigManager& getInstance();

    // Xoa ham tao sao chep va toan tu gan de dam bao tinh duy nhat
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // Doc va luu cau hinh tu file .ini
    void loadFromFile(const string& filePath = "data/config.ini");
    void saveToFile(const string& filePath = "data/config.ini");

    // Cac ham getter va setter cho cac tham so he thong
    double getFinePerDay() const;
    void setFinePerDay(double value);

    int getMaxBorrowDays() const;
    void setMaxBorrowDays(int value);

    int getHashTableSize() const;
    void setHashTableSize(int value);

    int getMinYear() const;
    void setMinYear(int value);

    int getMaxYear() const;
    void setMaxYear(int value);

    // Getter va Setter cho cac duong dan file
    const string& getBooksPath() const;
    void setBooksPath(const string& path);

    const string& getReadersPath() const;
    void setReadersPath(const string& path);

    const string& getTransactionsPath() const;
    void setTransactionsPath(const string& path);

    const string& getWaitlistQueuePath() const;
    void setWaitlistQueuePath(const string& path);

    const string& getHistoryStackPath() const;
    void setHistoryStackPath(const string& path);

    const string& getSystemLogPath() const;
    void setSystemLogPath(const string& path);

    const string& getConfigPath() const;
    void setConfigPath(const string& path);
};

#endif // CONFIG_MANAGER_H
