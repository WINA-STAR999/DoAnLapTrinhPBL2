#ifndef APP_INTERFACE_H
#define APP_INTERFACE_H

#include "../BLL/BookService.h"
#include "../BLL/ReaderService.h"
#include "../BLL/BorrowService.h"
#include "../Repositories/BookRepository.h"
#include "../Repositories/ReaderRepository.h"
#include "../Repositories/TransactionRepository.h"
#include "../Models/NguoiDung.h"
#include "../Models/ThuThu.h"
#include "../Models/DocGia.h"
#include "../Exceptions/LibraryException.h"
#include <string>

using namespace std;

// Lop AppInterface quan ly toan bo giao dien Console UI va dieu huong man hinh (SiteMap)
// Bao gom: SCR-00, SCR-A0 (A1, A2, A3, A4), SCR-R0 (R1, R2)
class AppInterface {
private:
    BookService* bookService;               // Dich vu nghiep vu sach
    ReaderService* readerService;           // Dich vu nghiep vu nguoi dung / doc gia
    BorrowService* borrowService;           // Dich vu nghiep vu muon tra va hang doi
    BookRepository* bookRepo;               // Kho du lieu sach
    ReaderRepository* readerRepo;           // Kho du lieu nguoi dung
    TransactionRepository* transRepo;       // Kho du lieu giao dich

    NguoiDung* currentUser;                 // Con tro toi nguoi dung hien tai dang dang nhap (nullptr neu chua dang nhap)

    // === CAC HAM HO TRO GIAO DIEN CONSOLE ===
    void printBanner(const string& title) const;
    void printTableBookHeader() const;
    void printBookRow(const Sach& s) const;
    void pauseScreen() const;
    string readInput(const string& prompt) const;
    int readMenuChoice(int minVal, int maxVal) const;

    // === SCR-00: MAN HINH DANG NHAP ===
    bool showLoginScreen();

    // === SCR-A0: DASHBOARD QUAN TRI VIEN (ADMIN) ===
    void showAdminDashboard();

    // === SCR-A1: QUAN LY KHO SACH ===
    void showBookManagementScreen();
    void handleViewBooks();
    void handleSearchBookById();
    void handleSearchBookByKeyword();
    void handleSortBooks();
    void handleAddBook();
    void handleUpdateBook();
    void handleDeleteBook();
    void handleUndoBookAction();

    // === SCR-A2: QUAN LY DOC GIA & NGUOI DUNG ===
    void showReaderManagementScreen();
    void handleViewReaders();
    void handleSearchReaderById();
    void handleAddReader();
    void handleUpdateReader();
    void handleDeleteReader();
    void handleManageFines();

    // === SCR-A3: QUAN LY MUON / TRA & HANG DOI CHO ===
    void showBorrowReturnManagementScreen();
    void handleBorrowBook();
    void handleReturnBook();
    void handleViewActiveBorrowings();
    void handleViewAllTransactions();
    void handleViewWaitQueue();

    // === SCR-A4: HE THONG, NGAN XEP LICH SU, LOG & CAU HINH ===
    void showSystemScreen();
    void handleViewHistoryStack();
    void handleViewSystemLogs();
    void handleViewEditConfig();
    void handleManualBackup();

    // === SCR-R0: PORTAL BAN DOC (READER) ===
    void showReaderPortal();

    // === SCR-R1: TRA CUU SACH & DANG KY HANG DOI CHO (READER) ===
    void showReaderBookSearchScreen();
    void handleReaderRegisterWaitlist();

    // === SCR-R2: SACH CUA TOI & HO SO CA NHAN (READER) ===
    void showMyProfileScreen();
    void handleViewMyBorrowedBooks();
    void handleViewMyFines();
    void handleViewMyHistory();
    void handleUpdateMyProfile();
    void handleChangeMyPassword();

public:
    // Ham tao Constructor Dependency Injection
    AppInterface(BookService* bService, ReaderService* rService, BorrowService* brService,
                 BookRepository* bRepo, ReaderRepository* rRepo, TransactionRepository* tRepo);

    // Ham huy mac dinh
    virtual ~AppInterface() = default;

    // Khong cho phep sao chep giao dien
    AppInterface(const AppInterface&) = delete;
    AppInterface& operator=(const AppInterface&) = delete;

    // Khoi chay he thong menu dieu huong
    void run();
};

#endif // APP_INTERFACE_H
