#ifndef READER_REPOSITORY_H
#define READER_REPOSITORY_H

#include "../Interfaces/IRepository.h"
#include "../DataStructures/DoublyLinkedList.hpp"
#include "../DataStructures/HashTable.hpp"
#include "../Models/NguoiDung.h"
#include "../Models/ThuThu.h"
#include "../Models/DocGia.h"
#include "../Factories/UserFactory.h"
#include "../Exceptions/LibraryException.h"
#include "../Utils/ConfigManager.h"
#include <string>

using namespace std;

// Lop ReaderRepository quan ly luu tru va truy xuat nguoi dung (ThuThu va DocGia)
// Quan ly bo nho da hinh thong qua DoublyLinkedList<NguoiDung*> va HashTable<string, NguoiDung*>
class ReaderRepository : public IRepository<NguoiDung*, string, NguoiDung*> {
private:
    DoublyLinkedList<NguoiDung*> userList;     // Danh sach lien ket doi quan ly cac con tro da hinh NguoiDung*
    HashTable<string, NguoiDung*> userHash;    // Bang bam tra cuu nhanh nguoi dung theo maND trong O(1)
    string filePath;                           // Duong dan den file data/readers.txt

public:
    // Ham tao: khoi tao bang bam va nap danh sach nguoi dung tu file
    ReaderRepository(const string& path = "");

    // Ham huy: giai phong toan bo bo nho dong cac con tro NguoiDung*
    virtual ~ReaderRepository();

    // Khong cho phep sao chep nong de tranh loi con tro treo
    ReaderRepository(const ReaderRepository&) = delete;
    ReaderRepository& operator=(const ReaderRepository&) = delete;

    // Them nguoi dung moi vao danh sach va bang bam, sau do luu xuong file
    void add(NguoiDung* const& user) override;

    // Cap nhat thong tin nguoi dung da ton tai va dong bo xuong file
    void update(NguoiDung* const& user) override;

    // Xoa nguoi dung khoi he thong, giai phong bo nho va luu xuong file
    bool remove(const string& maND) override;

    // Tra cuu nguoi dung theo maND trong O(1) bang bang bam (tra ve nullptr neu khong ton tai)
    NguoiDung* findById(const string& maND) override;

    // Nap toan bo danh sach nguoi dung tu file data/readers.txt dung UserFactory
    void loadFromFile() override;

    // Ghi toan bo danh sach nguoi dung hien co trong RAM xuong file data/readers.txt
    void saveToFile() override;

    // Lay tham chieu den danh sach lien ket doi de duyet nguoi dung
    DoublyLinkedList<NguoiDung*>& getUserList();
    const DoublyLinkedList<NguoiDung*>& getUserList() const;

    // Lay tong so luong nguoi dung hien co
    int getCount() const;

    // Kiem tra nguoi dung co ton tai hay khong theo maND
    bool exists(const string& maND) const;

    // Giai phong toan bo bo nho dong cua cac doi tuong NguoiDung* va lam rong repo
    void clear();
};

#endif // READER_REPOSITORY_H
