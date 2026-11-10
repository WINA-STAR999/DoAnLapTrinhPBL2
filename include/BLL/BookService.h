#ifndef BOOK_SERVICE_H
#define BOOK_SERVICE_H

#include "../Repositories/BookRepository.h"
#include "../Repositories/TransactionRepository.h"
#include "../Interfaces/ILogger.h"
#include "../Models/Sach.h"
#include "../Models/GiaoDich.h"
#include "../Strategies/BookSortStrategies.h"
#include "../Validators/DataValidators.h"
#include "../Exceptions/LibraryException.h"
#include "../Utils/DateUtils.h"
#include <string>
#include <stack>

using namespace std;

// Lop BookService xu ly logic nghiep vu ve kho sach (BLL)
// Cung cap cac thao tac CRUD sach, hoan tac Undo Stack, tra cuu O(1) va tim kiem tuyen tinh
class BookService {
private:
    BookRepository* bookRepo;               // Kho du lieu sach (Constructor Injection)
    ILogger* logger;                        // Dich vu ghi nhat ky he thong (DIP)
    TransactionRepository* transRepo;       // Kho giao dich phuc vu ghi lich su he thong (tuy chon)
    stack<ActionLog> undoStack;             // Ngan xep luu tru hanh dong de phuc vu Undo

    // Ham phu tro tao ma nhat ky tiep theo (vi du: LG104)
    string generateHistoryId() const;

public:
    // Ham tao nhan cac phu thuoc qua Constructor Injection
    BookService(BookRepository* bRepo, ILogger* log, TransactionRepository* tRepo = nullptr);

    // Ham huy mac dinh
    virtual ~BookService() = default;

    // Cam phep sao chep service
    BookService(const BookService&) = delete;
    BookService& operator=(const BookService&) = delete;

    // 1. Them sach moi vao kho (co kiem tra hop le va chong trung ma)
    void addBook(const Sach& s);

    // 2. Cap nhat thong tin sach da co
    void updateBook(const Sach& s);

    // 3. Xoa sach (kiem tra nghiep vu: khong cho phep xoa sach dang co doc gia muon)
    void deleteBook(const string& maSach);

    // 4. Hoan tac thao tac vua thuc hien gan nhat tren kho sach (Thoa man ADD, UPDATE, DELETE)
    void undoLastAction();

    // 5. Tim kiem sach theo ma trong O(1) bang bang bam
    Sach* searchById(const string& maSach) const;

    // 6. Tim kiem sach theo tu khoa (ten sach, tac gia, the loai)
    void searchByKeyword(const string& keyword, DoublyLinkedList<Sach>& resultList) const;

    // 7. Sap xep danh sach sach theo chien luoc ISortStrategy (Merge Sort)
    void sortBooks(int strategyChoice);

    // 8. Lay toan bo danh sach sach trong kho de hien thi
    DoublyLinkedList<Sach>& getAllBooks();
    const DoublyLinkedList<Sach>& getAllBooks() const;

    // 9. Kiem tra xem co thao tac nao trong Undo Stack de hoan tac khong
    bool canUndo() const;

    // 10. Lay so luong thao tac dang luu trong Undo Stack
    int getUndoCount() const;
};

#endif // BOOK_SERVICE_H
