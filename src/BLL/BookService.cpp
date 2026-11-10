#include "../../include/BLL/BookService.h"
#include <cstdio>
#include <iostream>

using namespace std;

// Ham chuyen chuoi ky tu ve chu thuong de tim kiem khong phan biet hoa thuong
static string toLowerString(const string& str) {
    string ketQua = "";
    for (int i = 0; i < (int)str.length(); i++) {
        char c = str[i];
        if (c >= 'A' && c <= 'Z') {
            ketQua += (char)(c + 32);
        } else {
            ketQua += c;
        }
    }
    return ketQua;
}

// Ham khoi tao nhan cac phu thuoc qua Dependency Injection
BookService::BookService(BookRepository* bRepo, ILogger* log, TransactionRepository* tRepo)
    : bookRepo(bRepo), logger(log), transRepo(tRepo) {}

// Ham sinh ma nhat ky tiep theo (vi du: LG104)
string BookService::generateHistoryId() const {
    int maxId = 100;
    if (transRepo != nullptr) {
        stack<LichSuGiaoDich> tempStack = transRepo->getHistoryStack();
        while (!tempStack.empty()) {
            string ma = tempStack.top().maLog;
            if (ma.length() >= 5 && ma.substr(0, 2) == "LG") {
                try {
                    int num = stoi(ma.substr(2));
                    if (num > maxId) {
                        maxId = num;
                    }
                } catch (...) {}
            }
            tempStack.pop();
        }
    }
    int nextId = maxId + 1;
    char buffer[15];
    snprintf(buffer, sizeof(buffer), "LG%03d", nextId);
    return string(buffer);
}

// 1. Them sach moi vao kho
void BookService::addBook(const Sach& s) {
    // Kiem tra hop le du lieu bang BookValidator
    BookValidator::validate(s);

    // Kiem tra trung ma sach O(1) qua HashTable
    if (bookRepo->findById(s.getMaSach()) != nullptr) {
        throw BusinessRuleException("Ma sach da ton tai trong he thong: " + s.getMaSach(), "BookService");
    }

    // Them vao repository
    bookRepo->add(s);

    // Day thao tac vao Undo Stack
    undoStack.push(ActionLog(ADD_BOOK, s, s));

    // Ghi nhat ky hoat dong
    if (logger != nullptr) {
        logger->logInfo("BookService", "Them sach thanh cong: " + s.getMaSach() + " - " + s.getTenSach());
    }

    // Ghi nhan lich su he thong
    if (transRepo != nullptr) {
        transRepo->pushHistory(LichSuGiaoDich(generateHistoryId(), "THEM_SACH", s.getMaSach(), "ADMIN", DateUtils::getCurrentDate()));
    }
}

// 2. Cap nhat thong tin sach
void BookService::updateBook(const Sach& s) {
    // Kiem tra hop le du lieu sach
    BookValidator::validate(s);

    // Kiem tra xem sach co ton tai trong kho hay khong
    Sach* oldBook = bookRepo->findById(s.getMaSach());
    if (oldBook == nullptr) {
        throw EntityNotFoundException("Khong tim thay sach can cap nhat: " + s.getMaSach(), "BookService");
    }

    // Luu lai trang thai cu de phuc vu Undo
    Sach snapshotCu = *oldBook;

    // Cap nhat trong repository
    bookRepo->update(s);

    // Day vao Undo Stack voi trang thai cu va trang thai moi
    undoStack.push(ActionLog(UPDATE_BOOK, snapshotCu, s));

    // Ghi log
    if (logger != nullptr) {
        logger->logInfo("BookService", "Cap nhat sach thanh cong: " + s.getMaSach());
    }

    // Ghi nhan lich su he thong
    if (transRepo != nullptr) {
        transRepo->pushHistory(LichSuGiaoDich(generateHistoryId(), "SUA_SACH", s.getMaSach(), "ADMIN", DateUtils::getCurrentDate()));
    }
}

// 3. Xoa sach khoi kho
void BookService::deleteBook(const string& maSach) {
    // Kiem tra ton tai
    Sach* book = bookRepo->findById(maSach);
    if (book == nullptr) {
        throw EntityNotFoundException("Khong tim thay sach can xoa: " + maSach, "BookService");
    }

    // Quy tac nghiep vu quan trong: Neu sach dang co nguoi muon thi khong duoc phep xoa
    if (book->getSoLuongHienCo() < book->getTongSoLuong()) {
        throw BusinessRuleException("Khong the xoa sach dang co doc gia muon!", "BookService");
    }

    // Kiem tra them trong danh sach giao dich dang muon
    if (transRepo != nullptr && transRepo->isBookBeingBorrowed(maSach)) {
        throw BusinessRuleException("Khong the xoa sach dang co doc gia muon!", "BookService");
    }

    // Luu lai thong tin cu de phuc vu Undo
    Sach snapshot = *book;

    // Xoa khoi repository
    bookRepo->remove(maSach);

    // Day hanh dong DELETE_BOOK vao Undo Stack
    undoStack.push(ActionLog(DELETE_BOOK, snapshot, snapshot));

    // Ghi log
    if (logger != nullptr) {
        logger->logInfo("BookService", "Xoa sach thanh cong: " + maSach);
    }

    // Ghi nhan lich su he thong
    if (transRepo != nullptr) {
        transRepo->pushHistory(LichSuGiaoDich(generateHistoryId(), "XOA_SACH", maSach, "ADMIN", DateUtils::getCurrentDate()));
    }
}

// 4. Hoan tac thao tac vua thuc hien gan nhat tren kho sach
void BookService::undoLastAction() {
    if (undoStack.empty()) {
        throw BusinessRuleException("Khong co thao tac nao de Undo!", "BookService");
    }

    // Lay hanh dong gan nhat o dinh ngan xep
    ActionLog log = undoStack.top();
    undoStack.pop();

    if (log.type == ADD_BOOK) {
        // Hoan tac cua THEM la XOA sach vua them
        bookRepo->remove(log.newState.getMaSach());
        if (logger != nullptr) {
            logger->logInfo("BookService", "Undo them sach: Da xoa sach " + log.newState.getMaSach());
        }
        if (transRepo != nullptr) {
            transRepo->pushHistory(LichSuGiaoDich(generateHistoryId(), "UNDO_SACH", log.newState.getMaSach(), "ADMIN", DateUtils::getCurrentDate()));
        }
    } else if (log.type == UPDATE_BOOK) {
        // Hoan tac cua SUA la CAP NHAT LAI gia tri cu truoc khi sua
        bookRepo->update(log.oldState);
        if (logger != nullptr) {
            logger->logInfo("BookService", "Undo sua sach: Da phuc hoi trang thai cu cua sach " + log.oldState.getMaSach());
        }
        if (transRepo != nullptr) {
            transRepo->pushHistory(LichSuGiaoDich(generateHistoryId(), "UNDO_SACH", log.oldState.getMaSach(), "ADMIN", DateUtils::getCurrentDate()));
        }
    } else if (log.type == DELETE_BOOK) {
        // Hoan tac cua XOA la THEM LAI cuon sach da bi xoa
        bookRepo->add(log.oldState);
        if (logger != nullptr) {
            logger->logInfo("BookService", "Undo xoa sach: Da phuc hoi sach " + log.oldState.getMaSach());
        }
        if (transRepo != nullptr) {
            transRepo->pushHistory(LichSuGiaoDich(generateHistoryId(), "UNDO_SACH", log.oldState.getMaSach(), "ADMIN", DateUtils::getCurrentDate()));
        }
    }
}

// 5. Tim kiem sach theo ma trong O(1)
Sach* BookService::searchById(const string& maSach) const {
    return bookRepo->findById(maSach);
}

// 6. Tim kiem sach theo tu khoa (ten sach, tac gia, the loai) tuyen tinh O(n)
void BookService::searchByKeyword(const string& keyword, DoublyLinkedList<Sach>& resultList) const {
    resultList.clear();
    string kwLower = toLowerString(keyword);

    Node<Sach>* curr = bookRepo->getBookList().getHead();
    while (curr != nullptr) {
        string tenLower = toLowerString(curr->data.getTenSach());
        string tgLower = toLowerString(curr->data.getTacGia());
        string tlLower = toLowerString(curr->data.getTheLoai());

        // Kiem tra xem tu khoa co xuat hien trong ten sach, tac gia hoac the loai hay khong
        if (tenLower.find(kwLower) != string::npos ||
            tgLower.find(kwLower) != string::npos ||
            tlLower.find(kwLower) != string::npos) {
            resultList.insertTail(curr->data);
        }
        curr = curr->next;
    }
}

// 7. Sap xep danh sach sach theo chien luoc ISortStrategy (Merge Sort)
void BookService::sortBooks(int strategyChoice) {
    // Su dung Factory Method de tao chien luoc sap xep phu hop
    ISortStrategy<Sach>* strategy = SortStrategyFactory::createStrategy(strategyChoice);
    if (strategy != nullptr) {
        // Merge Sort thuc hien noi lai con tro tai cho nen khong pha vo anh xa trong HashTable
        bookRepo->sort(*strategy);
        delete strategy; // Giai phong con tro tho chien luoc sau khi sap xep xong

        if (logger != nullptr) {
            logger->logInfo("BookService", "Sap xep sach thanh cong theo tieu chi: " + to_string(strategyChoice));
        }
    }
}

// 8. Lay toan bo danh sach sach de hien thi
DoublyLinkedList<Sach>& BookService::getAllBooks() {
    return bookRepo->getBookList();
}

const DoublyLinkedList<Sach>& BookService::getAllBooks() const {
    return bookRepo->getBookList();
}

// 9. Kiem tra co the Undo hay khong
bool BookService::canUndo() const {
    return !undoStack.empty();
}

// 10. Lay so luong hanh dong trong Undo Stack
int BookService::getUndoCount() const {
    return (int)undoStack.size();
}
