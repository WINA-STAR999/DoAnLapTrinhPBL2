#ifndef BORROW_SERVICE_H
#define BORROW_SERVICE_H

#include "../Repositories/BookRepository.h"
#include "../Repositories/ReaderRepository.h"
#include "../Repositories/TransactionRepository.h"
#include "../Interfaces/IFineStrategy.h"
#include "../Interfaces/ILogger.h"
#include "../Models/GiaoDich.h"
#include "../Validators/DataValidators.h"
#include "../Exceptions/LibraryException.h"
#include "../Utils/ConfigManager.h"
#include "../Utils/DateUtils.h"
#include <string>

using namespace std;

// Lop BorrowService xu ly toan bo nghiep vu Muon - Tra sach va Hang doi cho (BLL)
// Ket noi giua BookRepository, ReaderRepository, TransactionRepository va IFineStrategy
class BorrowService {
private:
    BookRepository* bookRepo;               // Kho sach (Constructor Injection)
    ReaderRepository* readerRepo;           // Kho doc gia (Constructor Injection)
    TransactionRepository* transRepo;       // Kho giao dich, hang doi va lich su (Constructor Injection)
    IFineStrategy* fineStrategy;            // Chien luoc tinh tien phat (Strategy Pattern)
    ILogger* logger;                        // Dich vu ghi nhat ky he thong (DIP)

    // Ham phu tro tu dong sinh ma giao dich tiep theo (GD001, GD002...)
    string generateTransactionId() const;

    // Ham phu tro tu dong sinh ma nhat ky tiep theo (LG101, LG102...)
    string generateHistoryId() const;

public:
    // Ham tao ap dung Constructor Dependency Injection
    BorrowService(BookRepository* bRepo, ReaderRepository* rRepo,
                  TransactionRepository* tRepo, IFineStrategy* fStrategy, ILogger* log);

    // Ham huy mac dinh
    virtual ~BorrowService() = default;

    // Cam sao chep service
    BorrowService(const BorrowService&) = delete;
    BorrowService& operator=(const BorrowService&) = delete;

    // Thay doi chien luoc tinh tien phat dong thoi gian chay
    void setFineStrategy(IFineStrategy* newStrategy);

    // 1. Muon sach:
    // - Neu sach con: lap phieu muon DANG_MUON, giam soLuongHienCo, ghi nhan LichSuGiaoDich
    // - Neu sach het tren gia (soLuongHienCo == 0): tu dong dua vao hang doi waitQueue neu autoEnqueueIfOut = true
    GiaoDich* borrowBook(const string& maDocGia, const string& maSach, const string& ngayMuon, bool autoEnqueueIfOut = true);

    // 2. Tra sach:
    // - Tinh so ngay qua han qua DateUtils::diffDays
    // - Tinh tien phat qua IFineStrategy va cong vao tongTienPhat cua DocGia neu tra tre
    // - Cap nhat trang thai DA_TRA va tang soLuongHienCo cua Sach
    // - Auto-Queue Trigger: Tu dong dequeue doc gia dau tien trong hang doi cho va cap sach ngay lap tuc!
    double returnBook(const string& maGD, const string& ngayTraThucTe);

    // 3. Lay danh sach tat ca cac giao dich dang muon (chua tra)
    void getActiveTransactions(DoublyLinkedList<GiaoDich>& resultList) const;

    // 4. Lay danh sach giao dich cua mot doc gia cu the
    void getTransactionsByReader(const string& maDocGia, DoublyLinkedList<GiaoDich>& resultList) const;

    // 5. Lay lich su giao dich ca nhan cua mot doc gia tu historyStack
    void getHistoryByReader(const string& maDocGia, DoublyLinkedList<LichSuGiaoDich>& resultList) const;

    // 6. Truy cap hang doi cho
    queue<YeuCauMuon>& getWaitQueue();
    const queue<YeuCauMuon>& getWaitQueue() const;

    // 7. Truy cap ngan xep lich su
    stack<LichSuGiaoDich>& getHistoryStack();
    const stack<LichSuGiaoDich>& getHistoryStack() const;
};

#endif // BORROW_SERVICE_H
