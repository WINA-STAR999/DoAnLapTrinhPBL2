#include "../../include/Repositories/TransactionRepository.h"
#include <fstream>
#include <iostream>

using namespace std;

// Ham tao khoi tao duong dan va tu dong nap ca 3 tap tin vao bo nho RAM
TransactionRepository::TransactionRepository(const string& transPath, const string& queuePath, const string& histPath) {
    if (transPath.empty()) {
        transactionsPath = ConfigManager::getInstance().getTransactionsPath();
    } else {
        transactionsPath = transPath;
    }

    if (queuePath.empty()) {
        waitlistQueuePath = ConfigManager::getInstance().getWaitlistQueuePath();
    } else {
        waitlistQueuePath = queuePath;
    }

    if (histPath.empty()) {
        historyStackPath = ConfigManager::getInstance().getHistoryStackPath();
    } else {
        historyStackPath = histPath;
    }

    loadFromFile();
}

// === 1. CAC THAO TAC VOI GIAO DICH (DoublyLinkedList<GiaoDich>) ===

// Them giao dich moi vao danh sach va dong bo xuong file
void TransactionRepository::add(const GiaoDich& gd) {
    addTransaction(gd);
}

void TransactionRepository::addTransaction(const GiaoDich& gd) {
    transactionList.insertTail(gd);
    saveTransactionsToFile();
}

// Cap nhat giao dich da co
void TransactionRepository::update(const GiaoDich& gd) {
    updateTransaction(gd);
}

void TransactionRepository::updateTransaction(const GiaoDich& gd) {
    Node<GiaoDich>* curr = transactionList.getHead();
    while (curr != nullptr) {
        if (curr->data.getMaGD() == gd.getMaGD()) {
            curr->data = gd;
            saveTransactionsToFile();
            return;
        }
        curr = curr->next;
    }

    throw EntityNotFoundException("Khong tim thay giao dich voi ma: " + gd.getMaGD(), "TransactionRepository");
}

// Xoa giao dich theo maGD
bool TransactionRepository::remove(const string& maGD) {
    return removeTransaction(maGD);
}

bool TransactionRepository::removeTransaction(const string& maGD) {
    Node<GiaoDich>* curr = transactionList.getHead();
    while (curr != nullptr) {
        if (curr->data.getMaGD() == maGD) {
            transactionList.removeNode(curr);
            saveTransactionsToFile();
            return true;
        }
        curr = curr->next;
    }
    return false;
}

// Tim kiem giao dich theo maGD
GiaoDich* TransactionRepository::findById(const string& maGD) {
    Node<GiaoDich>* curr = transactionList.getHead();
    while (curr != nullptr) {
        if (curr->data.getMaGD() == maGD) {
            return &(curr->data);
        }
        curr = curr->next;
    }
    return nullptr;
}

// Tim giao dich dang muon giua mot doc gia va mot cuon sach
GiaoDich* TransactionRepository::findActiveTransaction(const string& maDocGia, const string& maSach) {
    Node<GiaoDich>* curr = transactionList.getHead();
    while (curr != nullptr) {
        if (curr->data.getMaDocGia() == maDocGia && 
            curr->data.getMaSach() == maSach && 
            curr->data.getTrangThai() == "DANG_MUON") {
            return &(curr->data);
        }
        curr = curr->next;
    }
    return nullptr;
}

// Lay danh sach giao dich de duyet
DoublyLinkedList<GiaoDich>& TransactionRepository::getTransactionList() {
    return transactionList;
}

const DoublyLinkedList<GiaoDich>& TransactionRepository::getTransactionList() const {
    return transactionList;
}

// Lay so luong giao dich
int TransactionRepository::getTransactionCount() const {
    return transactionList.size();
}

// Kiem tra cuon sach co dang co nguoi muon hay khong
bool TransactionRepository::isBookBeingBorrowed(const string& maSach) const {
    Node<GiaoDich>* curr = transactionList.getHead();
    while (curr != nullptr) {
        if (curr->data.getMaSach() == maSach && curr->data.getTrangThai() == "DANG_MUON") {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

// Kiem tra doc gia co giao dich dang muon chua tra hay khong
bool TransactionRepository::hasActiveBorrowing(const string& maDocGia) const {
    Node<GiaoDich>* curr = transactionList.getHead();
    while (curr != nullptr) {
        if (curr->data.getMaDocGia() == maDocGia && curr->data.getTrangThai() == "DANG_MUON") {
            return true;
        }
        curr = curr->next;
    }
    return false;
}

// === 2. CAC THAO TAC VOI HANG DOI CHO (std::queue<YeuCauMuon>) ===

// Dang ky yeu cau muon moi vao cuoi hang doi cho
void TransactionRepository::enqueueWaitlist(const YeuCauMuon& req) {
    waitQueue.push(req);
    saveWaitQueueToFile();
}

// Trich xuat yeu cau cho dau tien theo thu tu FIFO co maSach trung khop
bool TransactionRepository::dequeueFirstWaitingForBook(const string& maSach, YeuCauMuon& outRequest) {
    bool timThay = false;
    queue<YeuCauMuon> hangDoiTam;

    // Duyet qua toan bo hang doi bang cach lan luot rut tung phan tu ra
    while (!waitQueue.empty()) {
        YeuCauMuon req = waitQueue.front();
        waitQueue.pop();

        // Neu chua tim thay va ma sach trung khop thi trich xuat phan tu nay ra
        if (!timThay && req.maSach == maSach) {
            outRequest = req;
            timThay = true;
            // Khong day req vao hangDoiTam vi yeu cau nay da duoc lay ra de phuc vu
        } else {
            // Cac yeu cau khac duoc giu nguyen va day vao hang doi tam
            hangDoiTam.push(req);
        }
    }

    // Gan lai toan bo cac phan tu con lai vao waitQueue goc de bao toan thu tu FIFO
    waitQueue = hangDoiTam;

    // Neu co lay duoc yeu cau thi dong bo lai file waitlist_queue.txt
    if (timThay) {
        saveWaitQueueToFile();
    }

    return timThay;
}

// Lay tham chieu den hang doi cho
queue<YeuCauMuon>& TransactionRepository::getWaitQueue() {
    return waitQueue;
}

const queue<YeuCauMuon>& TransactionRepository::getWaitQueue() const {
    return waitQueue;
}

// Lay so luong yeu cau dang cho
int TransactionRepository::getWaitQueueSize() const {
    return (int)waitQueue.size();
}

// Kiem tra xem doc gia da dang ky cho cuon sach nay hay chua
bool TransactionRepository::isReaderInWaitQueue(const string& maDocGia, const string& maSach) const {
    queue<YeuCauMuon> tempQueue = waitQueue;
    while (!tempQueue.empty()) {
        const YeuCauMuon& req = tempQueue.front();
        if (req.maDocGia == maDocGia && req.maSach == maSach) {
            return true;
        }
        tempQueue.pop();
    }
    return false;
}

// === 3. CAC THAO TAC VOI NGAN XEP LICH SU (std::stack<LichSuGiaoDich>) ===

// Day mot nhat ky hoat dong vao dinh ngan xep va luu xuong file
void TransactionRepository::pushHistory(const LichSuGiaoDich& log) {
    historyStack.push(log);
    saveHistoryStackToFile();
}

// Lay tham chieu den ngan xep lich su
stack<LichSuGiaoDich>& TransactionRepository::getHistoryStack() {
    return historyStack;
}

const stack<LichSuGiaoDich>& TransactionRepository::getHistoryStack() const {
    return historyStack;
}

// Lay so luong ban ghi lich su
int TransactionRepository::getHistoryStackSize() const {
    return (int)historyStack.size();
}

// === 4. CAC THAO TAC FILE I/O ===

// Nap dong thoi ca 3 file du lieu
void TransactionRepository::loadFromFile() {
    loadTransactionsFromFile();
    loadWaitQueueFromFile();
    loadHistoryStackFromFile();
}

// Ghi dong thoi ca 3 file du lieu
void TransactionRepository::saveToFile() {
    saveTransactionsToFile();
    saveWaitQueueToFile();
    saveHistoryStackToFile();
}

// Doc file transactions.txt
void TransactionRepository::loadTransactionsFromFile() {
    transactionList.clear();

    ifstream file(transactionsPath);
    if (!file.is_open()) {
        throw FileIOException("Khong the mo file de doc giao dich: " + transactionsPath, "TransactionRepository");
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;

        string truong[8];
        int soTruong = 0;
        string tuTam = "";

        for (int i = 0; i < (int)line.length(); i++) {
            if (line[i] == '|') {
                if (soTruong < 8) {
                    truong[soTruong++] = tuTam;
                    tuTam = "";
                }
            } else {
                tuTam += line[i];
            }
        }
        if (soTruong < 8) {
            truong[soTruong++] = tuTam;
        }

        if (soTruong == 8) {
            double tienPhat = 0.0;
            try {
                tienPhat = stod(truong[7]);
            } catch (...) {
                tienPhat = 0.0;
            }

            GiaoDich gd(truong[0], truong[1], truong[2], truong[3], truong[4], truong[5], truong[6], tienPhat);
            transactionList.insertTail(gd);
        }
    }
    file.close();
}

// Ghi file transactions.txt
void TransactionRepository::saveTransactionsToFile() {
    ofstream file(transactionsPath);
    if (!file.is_open()) {
        throw FileIOException("Khong the mo file de ghi giao dich: " + transactionsPath, "TransactionRepository");
    }

    Node<GiaoDich>* curr = transactionList.getHead();
    while (curr != nullptr) {
        file << curr->data.serializeToFile() << "\n";
        curr = curr->next;
    }
    file.close();
}

// Doc file waitlist_queue.txt
void TransactionRepository::loadWaitQueueFromFile() {
    while (!waitQueue.empty()) {
        waitQueue.pop();
    }

    ifstream file(waitlistQueuePath);
    if (!file.is_open()) {
        throw FileIOException("Khong the mo file de doc hang doi: " + waitlistQueuePath, "TransactionRepository");
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;

        string truong[3];
        int soTruong = 0;
        string tuTam = "";

        for (int i = 0; i < (int)line.length(); i++) {
            if (line[i] == '|') {
                if (soTruong < 3) {
                    truong[soTruong++] = tuTam;
                    tuTam = "";
                }
            } else {
                tuTam += line[i];
            }
        }
        if (soTruong < 3) {
            truong[soTruong++] = tuTam;
        }

        if (soTruong == 3) {
            YeuCauMuon req(truong[0], truong[1], truong[2]);
            waitQueue.push(req);
        }
    }
    file.close();
}

// Ghi file waitlist_queue.txt
void TransactionRepository::saveWaitQueueToFile() {
    ofstream file(waitlistQueuePath);
    if (!file.is_open()) {
        throw FileIOException("Khong the mo file de ghi hang doi: " + waitlistQueuePath, "TransactionRepository");
    }

    // Sao chep hang doi de ghi tung phan tu ma khong lam rong waitQueue goc
    queue<YeuCauMuon> tempQueue = waitQueue;
    while (!tempQueue.empty()) {
        file << tempQueue.front().serializeToFile() << "\n";
        tempQueue.pop();
    }
    file.close();
}

// Doc file history_stack.txt
void TransactionRepository::loadHistoryStackFromFile() {
    while (!historyStack.empty()) {
        historyStack.pop();
    }

    ifstream file(historyStackPath);
    if (!file.is_open()) {
        throw FileIOException("Khong the mo file de doc lich su: " + historyStackPath, "TransactionRepository");
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;

        string truong[5];
        int soTruong = 0;
        string tuTam = "";

        for (int i = 0; i < (int)line.length(); i++) {
            if (line[i] == '|') {
                if (soTruong < 5) {
                    truong[soTruong++] = tuTam;
                    tuTam = "";
                }
            } else {
                tuTam += line[i];
            }
        }
        if (soTruong < 5) {
            truong[soTruong++] = tuTam;
        }

        if (soTruong == 5) {
            LichSuGiaoDich log(truong[0], truong[1], truong[2], truong[3], truong[4]);
            historyStack.push(log);
        }
    }
    file.close();
}

// Ghi file history_stack.txt
void TransactionRepository::saveHistoryStackToFile() {
    ofstream file(historyStackPath);
    if (!file.is_open()) {
        throw FileIOException("Khong the mo file de ghi lich su: " + historyStackPath, "TransactionRepository");
    }

    // Ngan xep luu phan tu moi nhat tren dinh (top)
    // De ghi xuong file theo dung thu tu thoi gian cu nhat den moi nhat, ta dao nguoc ngan xep vao mot ngan xep tam
    stack<LichSuGiaoDich> tempCopy = historyStack;
    stack<LichSuGiaoDich> reverseStack;

    while (!tempCopy.empty()) {
        reverseStack.push(tempCopy.top());
        tempCopy.pop();
    }

    // Ghi lan luot tu phan tu cu nhat den moi nhat vao file
    while (!reverseStack.empty()) {
        file << reverseStack.top().serializeToFile() << "\n";
        reverseStack.pop();
    }
    file.close();
}

// Xoa toan bo du lieu trong bo nho RAM
void TransactionRepository::clear() {
    transactionList.clear();
    while (!waitQueue.empty()) {
        waitQueue.pop();
    }
    while (!historyStack.empty()) {
        historyStack.pop();
    }
}
