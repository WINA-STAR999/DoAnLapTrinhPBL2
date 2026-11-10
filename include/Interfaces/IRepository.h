#ifndef I_REPOSITORY_H
#define I_REPOSITORY_H

#include <string>

using namespace std;

// Giao dien mau Repository tong quat (Generic Repository Pattern)
// T: Kieu thuc the du lieu duoc quan ly (Sach, NguoiDung*, GiaoDich)
// K: Kieu du lieu khoa chinh (mac dinh la string)
// Ret: Kieu tra ve cua phuong thuc tim kiem findById (mac dinh la T*)
template <typename T, typename K = string, typename Ret = T*>
class IRepository {
public:
    virtual ~IRepository() = default;

    // Them mot ban ghi vao he thong
    virtual void add(const T& item) = 0;

    // Cap nhat thong tin mot ban ghi da ton tai
    virtual void update(const T& item) = 0;

    // Xoa ban ghi khoi he thong dua vao khoa chinh
    virtual bool remove(const K& id) = 0;

    // Tim kiem va tra ve doi tuong theo khoa chinh
    virtual Ret findById(const K& id) = 0;

    // Doc toan bo du lieu tu file vao bo nho RAM
    virtual void loadFromFile() = 0;

    // Luu toan bo du lieu tu bo nho RAM xuong file
    virtual void saveToFile() = 0;
};

#endif // I_REPOSITORY_H
