#ifndef TRANSACTION_REPOSITORY_H
#define TRANSACTION_REPOSITORY_H

#include "../DataStructures/DoublyLinkedList.hpp"
#include "../Models/GiaoDich.h"
#include "../Exceptions/LibraryException.h"
#include "../Utils/ConfigManager.h"
#include <string>
#include <queue>
#include <stack>

using namespace std;

// Lop TransactionRepository quan ly luu tru va dong bo ba nguon du lieu giao dich:
// 1. Danh sach muon tra: DoublyLinkedList<GiaoDich> (data/transactions.txt)
// 2. Hang doi yeu cau cho sach: std::queue<YeuCauMuon> (data/waitlist_queue.txt - FIFO)
// 3. Ngan xep nhat ky he thong: std::stack<LichSuGiaoDich> (data/history_stack.txt - LIFO)
class TransactionRepository {
private:
    DoublyLinkedList<GiaoDich> transactionList; // Danh sach lien ket doi luu cac giao dich muon/tra
    queue<YeuCauMuon> waitQueue;                // Hang doi FIFO luu danh sach ban doc dang cho muon sach
    stack<LichSuGiaoDich> historyStack;         // Ngan xep LIFO luu vet lich su cac thao tac trong he thong

    string transactionsPath;                    // Duong dan file data/transactions.txt
    string waitlistQueuePath;                   // Duong dan file data/waitlist_queue.txt
    string historyStackPath;                    // Duong dan file data/history_stack.txt

public:
    // Ham tao: khoi tao duong dan va tu dong nap toan bo 3 file du lieu vao RAM
    TransactionRepository(const string& transPath = "", const string& queuePath = "", const string& histPath = "");

    // Ham huy mac dinh
    virtual ~TransactionRepository() = default;

    // === CAC THAO TAC VOI GIAO DICH MUON / TRA (DoublyLinkedList<GiaoDich>) ===

    // Them giao dich moi vao danh sach va luu xuong transactions.txt
    void add(const GiaoDich& gd);
    void addTransaction(const GiaoDich& gd);

    // Cap nhat thong tin giao dich da co (vi du: danh dau da tra, ghi nhan tien phat) va luu xuong file
    void update(const GiaoDich& gd);
    void updateTransaction(const GiaoDich& gd);

    // Xoa giao dich theo ma giao dich
    bool remove(const string& maGD);
    bool removeTransaction(const string& maGD);

    // Tim kiem giao dich theo ma giao dich (tra ve nullptr neu khong tim thay)
    GiaoDich* findById(const string& maGD);

    // Tim giao dich dang muon (chua tra) cua mot doc gia doi voi mot cuon sach
    GiaoDich* findActiveTransaction(const string& maDocGia, const string& maSach);

    // Lay tham chieu den danh sach giao dich de duyet
    DoublyLinkedList<GiaoDich>& getTransactionList();
    const DoublyLinkedList<GiaoDich>& getTransactionList() const;

    // Lay so luong giao dich hien co
    int getTransactionCount() const;

    // Kiem tra mot cuon sach co dang duoc ai do muon hay khong
    bool isBookBeingBorrowed(const string& maSach) const;

    // Kiem tra mot doc gia co dang muon sach chua tra hay khong
    bool hasActiveBorrowing(const string& maDocGia) const;

    // === CAC THAO TAC VOI HANG DOI CHO (std::queue<YeuCauMuon>) ===

    // Dang ky mot yeu cau muon vao cuoi hang doi cho va luu xuong waitlist_queue.txt
    void enqueueWaitlist(const YeuCauMuon& req);

    // Trich xuat yeu cau cho dau tien theo thu tu FIFO khop voi maSach
    // Giu nguyen thu tu FIFO cua tat ca cac yeu cau khac va dong bo xuong waitlist_queue.txt
    bool dequeueFirstWaitingForBook(const string& maSach, YeuCauMuon& outRequest);

    // Lay tham chieu den hang doi cho
    queue<YeuCauMuon>& getWaitQueue();
    const queue<YeuCauMuon>& getWaitQueue() const;

    // Lay so luong yeu cau dang cho trong hang doi
    int getWaitQueueSize() const;

    // Kiem tra xem mot doc gia da co trong hang doi cho cua mot cuon sach hay chua
    bool isReaderInWaitQueue(const string& maDocGia, const string& maSach) const;

    // === CAC THAO TAC VOI NGAN XEP LICH SU (std::stack<LichSuGiaoDich>) ===

    // Day mot nhat ky hoat dong vao dinh ngan xep va luu xuong history_stack.txt
    void pushHistory(const LichSuGiaoDich& log);

    // Lay tham chieu den ngan xep lich su
    stack<LichSuGiaoDich>& getHistoryStack();
    const stack<LichSuGiaoDich>& getHistoryStack() const;

    // Lay so luong ban ghi lich su trong ngan xep
    int getHistoryStackSize() const;

    // === CAC THAO TAC FILE I/O ===

    // Nap dong thoi ca 3 file du lieu
    void loadFromFile();

    // Ghi dong thoi ca 3 file du lieu
    void saveToFile();

    // Doc va ghi rieng tung file
    void loadTransactionsFromFile();
    void saveTransactionsToFile();

    void loadWaitQueueFromFile();
    void saveWaitQueueToFile();

    void loadHistoryStackFromFile();
    void saveHistoryStackToFile();

    // Xoa sach du lieu trong ca 3 cau truc du lieu tren RAM
    void clear();
};

#endif // TRANSACTION_REPOSITORY_H
