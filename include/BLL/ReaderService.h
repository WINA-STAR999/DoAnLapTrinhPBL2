#ifndef READER_SERVICE_H
#define READER_SERVICE_H

#include "../Repositories/ReaderRepository.h"
#include "../Repositories/TransactionRepository.h"
#include "../Interfaces/ILogger.h"
#include "../Models/NguoiDung.h"
#include "../Models/ThuThu.h"
#include "../Models/DocGia.h"
#include "../Validators/DataValidators.h"
#include "../Exceptions/LibraryException.h"
#include <string>

using namespace std;

// Lop ReaderService xu ly logic nghiep vu ve quan ly tai khoan va nguoi dung (ThuThu va DocGia)
// Cung cap cac chuc nang: xac thuc dang nhap O(1), CRUD doc gia, quan ly tien phat, doi mat khau va cap nhat ho so
class ReaderService {
private:
    ReaderRepository* readerRepo;           // Kho du lieu nguoi dung (Constructor Injection)
    TransactionRepository* transRepo;       // Kho du lieu giao dich de kiem tra rang buoc muon/tra
    ILogger* logger;                        // Dich vu ghi nhat ky he thong (DIP)

public:
    // Ham tao nhan cac phu thuoc qua Constructor Injection
    ReaderService(ReaderRepository* rRepo, TransactionRepository* tRepo, ILogger* log);

    // Ham huy mac dinh
    virtual ~ReaderService() = default;

    // Cam sao chep service
    ReaderService(const ReaderService&) = delete;
    ReaderService& operator=(const ReaderService&) = delete;

    // 1. Xac thuc dang nhap trong O(1) qua HashTable va tra ve con tro da hinh NguoiDung*
    NguoiDung* authenticate(const string& maND, const string& password);

    // 2. Them nguoi dung moi (ThuThu hoac DocGia) co kiem tra hop le va chong trung ma
    void addReader(NguoiDung* user);

    // 3. Cap nhat thong tin nguoi dung da co
    void updateReader(NguoiDung* user);

    // 4. Xoa nguoi dung (kiem tra rang buoc: khong cho xoa neu con sach dang muon hoac con no tien phat)
    void deleteReader(const string& maND);

    // 5. Thay doi mat khau nguoi dung
    void changePassword(const string& maND, const string& oldPass, const string& newPass);

    // 6. Cap nhat ho so ca nhan (ho ten, so dien thoai)
    void updateProfile(const string& maND, const string& newName, const string& newPhone);

    // 7. Thanh toan / xoa tien phat cho doc gia
    void clearFine(const string& maDocGia, double amount);

    // 8. Lay so tien phat hien tai cua doc gia
    double getFineAmount(const string& maDocGia) const;

    // 9. Tra cuu nguoi dung theo ma trong O(1)
    NguoiDung* getReaderById(const string& maND) const;

    // 10. Lay danh sach toan bo nguoi dung trong he thong
    DoublyLinkedList<NguoiDung*>& getAllUsers();
    const DoublyLinkedList<NguoiDung*>& getAllUsers() const;
};

#endif // READER_SERVICE_H
