#include "../../include/Repositories/BookRepository.h"
#include <fstream>
#include <iostream>

using namespace std;

// Ham tao: khoi tao bang bam voi kich thuoc tu ConfigManager va doc du lieu san co
BookRepository::BookRepository(const string& path)
    : bookHash(ConfigManager::getInstance().getHashTableSize()) {
    if (path.empty()) {
        filePath = ConfigManager::getInstance().getBooksPath();
    } else {
        filePath = path;
    }
    loadFromFile();
}

// Them sach moi vao kho
void BookRepository::add(const Sach& s) {
    // 1. Chen sach vao cuoi danh sach lien ket doi de luu tru thuc the
    // Phuong thuc insertTail tra ve con tro Sach* tro truc tiep vao du lieu ben trong Node
    Sach* ptr = bookList.insertTail(s);

    // 2. Day ngay dia chi Sach* nay vao HashTable de phuc vu tim kiem O(1)
    bookHash.insert(s.getMaSach(), ptr);

    // 3. Dong bo du lieu moi xuong file books.txt
    saveToFile();
}

// Cap nhat thong tin sach da co trong kho
void BookRepository::update(const Sach& s) {
    // Tim dia chi cua cuon sach can cap nhat trong bang bam O(1)
    Sach* ptr = bookHash.search(s.getMaSach());
    if (ptr == nullptr) {
        throw EntityNotFoundException("Khong tim thay sach voi ma: " + s.getMaSach(), "BookRepository");
    }

    // Cap nhat truc tiep cac gia tri vao doi tuong tai dia chi ptr (in-place)
    // Nho vay, dia chi ptr van giu nguyen va anh xa trong HashTable van hoan toan chinh xac
    *ptr = s;

    // Dong bo thay doi xuong file
    saveToFile();
}

// Xoa mot cuon sach khoi kho dua tren ma sach
bool BookRepository::remove(const string& maSach) {
    // Kiem tra su ton tai cua sach trong bang bam
    Sach* ptr = bookHash.search(maSach);
    if (ptr == nullptr) {
        return false;
    }

    // 1. Xoa khoi bang bam de vo hieu hoa tra cuu O(1)
    bookHash.remove(maSach);

    // 2. Duyet danh sach lien ket doi de tim va xoa node tuong ung
    Node<Sach>* curr = bookList.getHead();
    while (curr != nullptr) {
        if (curr->data.getMaSach() == maSach) {
            bookList.removeNode(curr);
            break;
        }
        curr = curr->next;
    }

    // 3. Luu lai danh sach sau khi da xoa xuong tap tin
    saveToFile();
    return true;
}

// Tra cuu sach theo ma sach bang bang bam O(1)
Sach* BookRepository::findById(const string& maSach) {
    return bookHash.search(maSach);
}

// Doc toan bo danh sach sach tu file data/books.txt
void BookRepository::loadFromFile() {
    clear(); // Don dep bo nho truoc khi nap du lieu moi

    ifstream file(filePath);
    if (!file.is_open()) {
        throw FileIOException("Khong the mo file de doc: " + filePath, "BookRepository");
    }

    string line;
    while (getline(file, line)) {
        // Bo qua cac dong trong hoac dong chua ky tu xuong dong cua Windows
        if (line.empty()) continue;
        if (line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;

        // Tach cac truong du lieu bang ky tu '|'
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

        // Kiem tra du 8 truong theo dung quy dinh
        if (soTruong == 8) {
            string ma = truong[0];
            string ten = truong[1];
            string tg = truong[2];
            string tl = truong[3];
            int nam = 0;
            int tongSL = 0;
            int slHienCo = 0;

            try {
                nam = stoi(truong[4]);
                tongSL = stoi(truong[5]);
                slHienCo = stoi(truong[6]);
            } catch (...) {
                // Bo qua dong neu co loi chuyen doi so
                continue;
            }

            string vt = truong[7];

            // Tao doi tuong sach moi
            Sach s(ma, ten, tg, tl, nam, tongSL, slHienCo, vt);

            // Chen sach vao cuoi danh sach lien ket doi va lay dia chi Sach* cua node do
            Sach* ptr = bookList.insertTail(s);

            // Day ngay dia chi Sach* vao HashTable de phuc vu tra cuu O(1)
            bookHash.insert(ma, ptr);
        }
    }
    file.close();
}

// Ghi toan bo danh sach sach tu RAM xuong file
void BookRepository::saveToFile() {
    ofstream file(filePath);
    if (!file.is_open()) {
        throw FileIOException("Khong the mo file de ghi: " + filePath, "BookRepository");
    }

    // Duyet danh sach lien ket doi tu dau den cuoi de ghi tung ban ghi
    Node<Sach>* curr = bookList.getHead();
    while (curr != nullptr) {
        file << curr->data.serializeToFile() << "\n";
        curr = curr->next;
    }
    file.close();
}

// Lay danh sach lien ket doi de duyet
DoublyLinkedList<Sach>& BookRepository::getBookList() {
    return bookList;
}

const DoublyLinkedList<Sach>& BookRepository::getBookList() const {
    return bookList;
}

// Sap xep danh sach sach theo chien luoc ISortStrategy (Merge Sort)
void BookRepository::sort(const ISortStrategy<Sach>& strategy) {
    // Merge Sort tai cho chi noi lai cac con tro next/prev cua cac Node trong DoublyLinkedList
    // Dia chi bo nho cua cac doi tuong Sach van giu nguyen nen cac con tro Sach* trong HashTable van dung
    bookList.mergeSort(strategy);
}

// Lay so luong dau sach
int BookRepository::getCount() const {
    return bookList.size();
}

// Kiem tra su ton tai cua sach theo ma
bool BookRepository::exists(const string& maSach) const {
    return bookHash.search(maSach) != nullptr;
}

// Xoa toan bo du lieu trong bo nho RAM
void BookRepository::clear() {
    bookHash.clear();
    bookList.clear();
}
