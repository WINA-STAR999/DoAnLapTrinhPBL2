#ifndef BOOK_REPOSITORY_H
#define BOOK_REPOSITORY_H

#include "../Interfaces/IRepository.h"
#include "../Interfaces/ISortStrategy.h"
#include "../DataStructures/DoublyLinkedList.hpp"
#include "../DataStructures/HashTable.hpp"
#include "../Models/Sach.h"
#include "../Exceptions/LibraryException.h"
#include "../Utils/ConfigManager.h"
#include <string>

using namespace std;

// Lop BookRepository quan ly luu tru va truy xuat du lieu sach trong thu vien
// Ket hop giua DoublyLinkedList<Sach> (luu tru thuc the) va HashTable<string, Sach*> (chi muc O(1))
class BookRepository : public IRepository<Sach, string> {
private:
    DoublyLinkedList<Sach> bookList;     // Danh sach lien ket doi so huu cac doi tuong Sach trong RAM
    HashTable<string, Sach*> bookHash;   // Bang bam luu con tro Sach* tro truc tiep vao Node ben trong bookList
    string filePath;                     // Duong dan file data/books.txt

public:
    // Ham tao: khoi tao repo voi kich thuoc bang bam va tu dong nap du lieu tu file
    BookRepository(const string& path = "");

    // Ham huy mac dinh
    virtual ~BookRepository() = default;

    // Them sach moi vao danh sach, lay con tro Sach* luu vao bang bam va dong bo xuong file
    void add(const Sach& s) override;

    // Cap nhat thong tin sach tai cho (in-place) tren con tro lay tu bang bam va dong bo xuong file
    void update(const Sach& s) override;

    // Xoa sach khoi bang bam va danh sach lien ket doi, sau do dong bo xuong file
    bool remove(const string& maSach) override;

    // Tra cuu nhanh sach theo ma sach trong O(1) nho bang bam (tra ve nullptr neu khong tim thay)
    Sach* findById(const string& maSach) override;

    // Nap toan bo danh sach sach tu file data/books.txt vao RAM
    void loadFromFile() override;

    // Ghi toan bo danh sach sach hien co trong RAM xuong file data/books.txt
    void saveToFile() override;

    // Lay tham chieu den danh sach lien ket doi de duyet qua tung cuon sach
    DoublyLinkedList<Sach>& getBookList();
    const DoublyLinkedList<Sach>& getBookList() const;

    // Sap xep danh sach sach tai cho theo chien luoc ISortStrategy (Merge Sort)
    // Thuat toan Merge Sort noi lai con tro next/prev ma khong doi dia chi data nen HashTable van hop le
    void sort(const ISortStrategy<Sach>& strategy);

    // Lay tong so dau sach hien co trong danh sach
    int getCount() const;

    // Kiem tra su ton tai cua ma sach trong bang bam O(1)
    bool exists(const string& maSach) const;

    // Xoa toan bo du lieu trong bo nho RAM
    void clear();
};

#endif // BOOK_REPOSITORY_H
