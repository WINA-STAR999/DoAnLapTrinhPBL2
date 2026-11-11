#include "../include/Utils/ConfigManager.h"
#include "../include/Utils/FileLogger.h"
#include "../include/Repositories/BookRepository.h"
#include "../include/Repositories/ReaderRepository.h"
#include "../include/Repositories/TransactionRepository.h"
#include "../include/Strategies/StandardFineStrategy.h"
#include "../include/BLL/BookService.h"
#include "../include/BLL/ReaderService.h"
#include "../include/BLL/BorrowService.h"
#include "../include/UI/AppInterface.h"
#include <iostream>

using namespace std;

// Diem vao chinh cua he thong Quan ly Thu vien PBL 2
int main() {
    // 1. Khoi tao tham so cau hinh tu file data/config.ini
    ConfigManager& config = ConfigManager::getInstance();
    try {
        config.loadFromFile("data/config.ini");
    } catch (...) {
        // Neu file chua ton tai thi he thong tu dong ap dung gia tri mac dinh
    }

    // 2. Khoi tao he thong ghi log FileLogger
    FileLogger& logger = FileLogger::getInstance();
    logger.logInfo("System", "Bootstrapped Library Management System PBL 2");

    try {
        // 3. Khoi tao cac kho du lieu (Repositories)
        BookRepository bookRepo(config.getBooksPath());
        ReaderRepository readerRepo(config.getReadersPath());
        TransactionRepository transRepo(config.getTransactionsPath(), 
                                        config.getWaitlistQueuePath(), 
                                        config.getHistoryStackPath());

        // 4. Khoi tao chien luoc tinh tien phat Strategy
        StandardFineStrategy fineStrategy;

        // 5. Khoi tao cac dich vu nghiep vu BLL thong qua Constructor Dependency Injection
        BookService bookService(&bookRepo, &logger, &transRepo);
        ReaderService readerService(&readerRepo, &transRepo, &logger);
        BorrowService borrowService(&bookRepo, &readerRepo, &transRepo, &fineStrategy, &logger);

        // 6. Khoi tao va khoi chay giao dien Console UI AppInterface
        AppInterface app(&bookService, &readerService, &borrowService, 
                         &bookRepo, &readerRepo, &transRepo);
        app.run();

        logger.logInfo("System", "Shutdown Library Management System PBL 2 cleanly");
    } catch (const LibraryException& e) {
        cerr << "\n[!] LOI KHOI TAO HE THONG: " << e.what() << "\n";
        logger.logError("System", string("Fatal initialization error: ") + e.what());
        return 1;
    } catch (const exception& e) {
        cerr << "\n[!] LOI CHUA DU LIEU HE THONG: " << e.what() << "\n";
        logger.logError("System", string("Fatal runtime exception: ") + e.what());
        return 1;
    }

    return 0;
}
