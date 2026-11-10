#include "../../include/BLL/BorrowService.h"
#include <cstdio>
#include <iostream>

using namespace std;

// Ham khoi tao nhan cac phu thuoc thong qua Constructor Dependency Injection
BorrowService::BorrowService(BookRepository* bRepo, ReaderRepository* rRepo,
                             TransactionRepository* tRepo, IFineStrategy* fStrategy, ILogger* log)
    : bookRepo(bRepo), readerRepo(rRepo), transRepo(tRepo), fineStrategy(fStrategy), logger(log) {}

// Ham sinh ma giao dich tiep theo (vi du: GD001, GD002, GD004...)
string BorrowService::generateTransactionId() const {
    int maxId = 0;
    Node<GiaoDich>* curr = transRepo->getTransactionList().getHead();
    while (curr != nullptr) {
        string ma = curr->data.getMaGD();
        if (ma.length() >= 5 && ma.substr(0, 2) == "GD") {
            try {
                int num = stoi(ma.substr(2));
                if (num > maxId) {
                    maxId = num;
                }
            } catch (...) {}
        }
        curr = curr->next;
    }

    int nextId = maxId + 1;
    char buffer[15];
    snprintf(buffer, sizeof(buffer), "GD%03d", nextId);
    return string(buffer);
}

// Ham sinh ma nhat ky tiep theo (vi du: LG101, LG104...)
string BorrowService::generateHistoryId() const {
    int maxId = 100;
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

    int nextId = maxId + 1;
    char buffer[15];
    snprintf(buffer, sizeof(buffer), "LG%03d", nextId);
    return string(buffer);
}

// Thay doi chien luoc tinh tien phat dong thoi gian chay
void BorrowService::setFineStrategy(IFineStrategy* newStrategy) {
    fineStrategy = newStrategy;
}

// 1. Muon sach
GiaoDich* BorrowService::borrowBook(const string& maDocGia, const string& maSach,
                                    const string& ngayMuon, bool autoEnqueueIfOut) {
    // 1. Kiem tra dinh dang ngay muon bang TransactionValidator
    TransactionValidator::validateDate(ngayMuon);

    // 2. Kiem tra su ton tai cua doc gia trong O(1) qua bang bam
    NguoiDung* user = readerRepo->findById(maDocGia);
    if (user == nullptr) {
        throw EntityNotFoundException("Khong tim thay doc gia voi ma: " + maDocGia, "BorrowService");
    }

    // Rang buoc: Chi doc gia (READER) moi duoc phep muon sach
    if (user->layVaiTro() != "READER") {
        throw BusinessRuleException("Chi nguoi dung co vai tro Doc Gia (READER) moi co the muon sach!", "BorrowService");
    }

    // Rang buoc: Doc gia khong the muon cung 1 cuon sach 2 lan khi chua tra
    if (transRepo->findActiveTransaction(maDocGia, maSach) != nullptr) {
        throw BusinessRuleException("Doc gia dang muon cuon sach nay va chua tra!", "BorrowService");
    }

    // Rang buoc: Kiem tra doc gia co dang no tien phat chua thanh toan hay khong
    DocGia* dg = dynamic_cast<DocGia*>(user);
    if (dg != nullptr && dg->getTongTienPhat() > 0.0) {
        throw BusinessRuleException("Doc gia dang con no tien phat (" + to_string((long long)dg->getTongTienPhat()) + " VND), vui long thanh toan truoc khi muon sach!", "BorrowService");
    }

    // 3. Kiem tra su ton tai cua sach trong O(1) qua bang bam
    Sach* sach = bookRepo->findById(maSach);
    if (sach == nullptr) {
        throw EntityNotFoundException("Khong tim thay sach voi ma: " + maSach, "BorrowService");
    }

    // 4. Xu ly theo so luong sach hien co tren gia
    if (sach->getSoLuongHienCo() > 0) {
        // Giam 1 quyen sach tren gia va luu file books.txt
        sach->giamSoLuongHienCo();
        bookRepo->saveToFile();

        // Tinh han tra = ngayMuon + maxBorrowDays
        int maxDays = ConfigManager::getInstance().getMaxBorrowDays();
        string hanTra = DateUtils::addDays(ngayMuon, maxDays);

        // Sinh ma giao dich moi va them vao TransactionRepository
        string maGD = generateTransactionId();
        GiaoDich gd(maGD, maDocGia, maSach, ngayMuon, hanTra, "CHUA_TRA", "DANG_MUON", 0.0);
        transRepo->add(gd);

        // Day vao ngan xep LichSuGiaoDich (LIFO)
        string maLog = generateHistoryId();
        LichSuGiaoDich log(maLog, "MUON_SACH", maSach, maDocGia, ngayMuon);
        transRepo->pushHistory(log);

        // Ghi nhat ky hoat dong
        if (logger != nullptr) {
            logger->logInfo("BorrowService", "Muon sach thanh cong: " + maDocGia + " muon " + maSach + " (Ma GD: " + maGD + ", Han tra: " + hanTra + ")");
        }

        return transRepo->findById(maGD);
    } else {
        // Truong hop sach da het tren gia (soLuongHienCo == 0)
        if (autoEnqueueIfOut) {
            // Kiem tra neu doc gia da dang ky trong hang doi cua sach nay thi khong enqueue lai
            if (transRepo->isReaderInWaitQueue(maDocGia, maSach)) {
                throw BusinessRuleException("Doc gia da nam trong hang doi cho cua cuon sach nay!", "BorrowService");
            }

            // Xep vao cuoi hang doi cho (FIFO) va dong bo xuong waitlist_queue.txt
            YeuCauMuon req(maDocGia, maSach, ngayMuon);
            transRepo->enqueueWaitlist(req);

            if (logger != nullptr) {
                logger->logInfo("BorrowService", "Sach " + maSach + " het tren gia: Da them doc gia " + maDocGia + " vao hang doi cho (FIFO)");
            }
        }
        return nullptr;
    }
}

// 2. Tra sach
double BorrowService::returnBook(const string& maGD, const string& ngayTraThucTe) {
    // 1. Kiem tra dinh dang ngay tra thuc te bang TransactionValidator
    TransactionValidator::validateDate(ngayTraThucTe);

    // 2. Tim kiem giao dich theo maGD
    GiaoDich* gd = transRepo->findById(maGD);
    if (gd == nullptr) {
        throw EntityNotFoundException("Khong tim thay giao dich voi ma: " + maGD, "BorrowService");
    }

    if (gd->getTrangThai() == "DA_TRA") {
        throw BusinessRuleException("Giao dich nay da duoc tra sach truoc do!", "BorrowService");
    }

    // 3. Tinh so ngay qua han giua hanTra va ngayTraThucTe
    int soNgayTre = DateUtils::diffDays(gd->getHanTra(), ngayTraThucTe);
    double tienPhat = 0.0;
    string loaiLichSu = "TRA_SACH_DUNG_HAN";

    // 4. Neu co tre han, tinh tien phat theo Strategy Pattern va cong vao tongTienPhat cua DocGia
    if (soNgayTre > 0) {
        if (fineStrategy != nullptr) {
            tienPhat = fineStrategy->calculateFine(soNgayTre);
        }
        loaiLichSu = "TRA_SACH_TRE";

        NguoiDung* user = readerRepo->findById(gd->getMaDocGia());
        if (user != nullptr) {
            DocGia* dg = dynamic_cast<DocGia*>(user);
            if (dg != nullptr) {
                dg->congPhat(tienPhat);
                readerRepo->saveToFile();
            }
        }
    }

    // 5. Cap nhat thong tin giao dich sang DA_TRA
    gd->setNgayTraThucTe(ngayTraThucTe);
    gd->setTrangThai("DA_TRA");
    gd->setTienPhat(tienPhat);
    transRepo->saveTransactionsToFile();

    // 6. Tang so luong sach tren gia len 1
    Sach* sach = bookRepo->findById(gd->getMaSach());
    if (sach != nullptr) {
        sach->tangSoLuongHienCo();
        bookRepo->saveToFile();
    }

    // 7. Ghi vao History Stack
    string maLog = generateHistoryId();
    LichSuGiaoDich log(maLog, loaiLichSu, gd->getMaSach(), gd->getMaDocGia(), ngayTraThucTe);
    transRepo->pushHistory(log);

    if (logger != nullptr) {
        logger->logInfo("BorrowService", "Tra sach thanh cong: " + maGD + " (Tre: " + to_string(soNgayTre) + " ngay, Tien phat: " + to_string((long long)tienPhat) + " VND)");
    }

    // 8. Auto-Queue Trigger: Kiem tra xem co doc gia nao dang cho cuon sach nay trong hang doi khong
    YeuCauMuon nextReq;
    if (transRepo->dequeueFirstWaitingForBook(gd->getMaSach(), nextReq)) {
        if (logger != nullptr) {
            logger->logInfo("BorrowService", "Auto-Queue: Chuyen sach " + nextReq.maSach + " ngay lap tuc cho doc gia trong hang doi: " + nextReq.maDocGia);
        }
        // Tu dong lap phieu muon cap sach ngay lap tuc cho nguoi dang cho
        borrowBook(nextReq.maDocGia, nextReq.maSach, ngayTraThucTe, false);
    }

    return tienPhat;
}

// 3. Lay danh sach tat ca cac giao dich dang muon
void BorrowService::getActiveTransactions(DoublyLinkedList<GiaoDich>& resultList) const {
    resultList.clear();
    Node<GiaoDich>* curr = transRepo->getTransactionList().getHead();
    while (curr != nullptr) {
        if (curr->data.getTrangThai() == "DANG_MUON") {
            resultList.insertTail(curr->data);
        }
        curr = curr->next;
    }
}

// 4. Lay danh sach giao dich cua mot doc gia
void BorrowService::getTransactionsByReader(const string& maDocGia, DoublyLinkedList<GiaoDich>& resultList) const {
    resultList.clear();
    Node<GiaoDich>* curr = transRepo->getTransactionList().getHead();
    while (curr != nullptr) {
        if (curr->data.getMaDocGia() == maDocGia) {
            resultList.insertTail(curr->data);
        }
        curr = curr->next;
    }
}

// 5. Lay lich su giao dich ca nhan cua mot doc gia
void BorrowService::getHistoryByReader(const string& maDocGia, DoublyLinkedList<LichSuGiaoDich>& resultList) const {
    resultList.clear();
    stack<LichSuGiaoDich> tempStack = transRepo->getHistoryStack();
    while (!tempStack.empty()) {
        const LichSuGiaoDich& log = tempStack.top();
        if (log.maDocGia == maDocGia) {
            resultList.insertTail(log);
        }
        tempStack.pop();
    }
}

// 6. Truy cap hang doi cho
queue<YeuCauMuon>& BorrowService::getWaitQueue() {
    return transRepo->getWaitQueue();
}

const queue<YeuCauMuon>& BorrowService::getWaitQueue() const {
    return transRepo->getWaitQueue();
}

// 7. Truy cap ngan xep lich su
stack<LichSuGiaoDich>& BorrowService::getHistoryStack() {
    return transRepo->getHistoryStack();
}

const stack<LichSuGiaoDich>& BorrowService::getHistoryStack() const {
    return transRepo->getHistoryStack();
}
