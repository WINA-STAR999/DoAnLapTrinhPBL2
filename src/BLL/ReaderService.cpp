#include "../../include/BLL/ReaderService.h"
#include <iostream>

using namespace std;

// Ham khoi tao nhan cac phu thuoc qua Dependency Injection
ReaderService::ReaderService(ReaderRepository* rRepo, TransactionRepository* tRepo, ILogger* log)
    : readerRepo(rRepo), transRepo(tRepo), logger(log) {}

// 1. Xac thuc dang nhap trong O(1) nho bang bam HashTable
NguoiDung* ReaderService::authenticate(const string& maND, const string& password) {
    // Tra cuu tai khoan trong bang bam O(1)
    NguoiDung* user = readerRepo->findById(maND);
    if (user == nullptr) {
        throw EntityNotFoundException("Khong tim thay tai khoan voi ma: " + maND, "ReaderService");
    }

    // Kiem tra khop mat khau
    if (!user->kiemTraMatKhau(password)) {
        throw BusinessRuleException("Mat khau dang nhap khong chinh xac!", "ReaderService");
    }

    // Ghi log dang nhap thanh cong
    if (logger != nullptr) {
        logger->logInfo("ReaderService", "Xac thuc dang nhap thanh cong: " + maND + " (" + user->layVaiTro() + ")");
    }

    return user;
}

// 2. Them nguoi dung moi vao he thong
void ReaderService::addReader(NguoiDung* user) {
    if (user == nullptr) {
        throw ValidationException("Doi tuong nguoi dung khong duoc rong (nullptr)!", "ReaderService");
    }

    // Kiem tra hop le du lieu bang ReaderValidator
    ReaderValidator::validate(*user);

    // Kiem tra trung ma nguoi dung O(1)
    if (readerRepo->findById(user->getMaND()) != nullptr) {
        throw BusinessRuleException("Ma nguoi dung da ton tai trong he thong: " + user->getMaND(), "ReaderService");
    }

    // Them vao repository
    readerRepo->add(user);

    // Ghi nhat ky
    if (logger != nullptr) {
        logger->logInfo("ReaderService", "Them nguoi dung moi thanh cong: " + user->getMaND() + " - " + user->getHoTen());
    }
}

// 3. Cap nhat thong tin nguoi dung
void ReaderService::updateReader(NguoiDung* user) {
    if (user == nullptr) {
        throw ValidationException("Doi tuong nguoi dung khong duoc rong (nullptr)!", "ReaderService");
    }

    // Kiem tra hop le du lieu
    ReaderValidator::validate(*user);

    // Kiem tra ton tai
    if (readerRepo->findById(user->getMaND()) == nullptr) {
        throw EntityNotFoundException("Khong tim thay nguoi dung can cap nhat: " + user->getMaND(), "ReaderService");
    }

    // Cap nhat trong repository
    readerRepo->update(user);

    // Ghi nhat ky
    if (logger != nullptr) {
        logger->logInfo("ReaderService", "Cap nhat nguoi dung thanh cong: " + user->getMaND());
    }
}

// 4. Xoa nguoi dung khoi he thong
void ReaderService::deleteReader(const string& maND) {
    NguoiDung* user = readerRepo->findById(maND);
    if (user == nullptr) {
        throw EntityNotFoundException("Khong tim thay nguoi dung can xoa: " + maND, "ReaderService");
    }

    // Rang buoc nghiep vu 1: Khong cho phep xoa doc gia dang co sach muon chua tra
    if (transRepo != nullptr && transRepo->hasActiveBorrowing(maND)) {
        throw BusinessRuleException("Khong the xoa doc gia dang co giao dich muon sach chua tra!", "ReaderService");
    }

    // Rang buoc nghiep vu 2: Khong cho phep xoa doc gia con no tien phat
    DocGia* dg = dynamic_cast<DocGia*>(user);
    if (dg != nullptr && dg->getTongTienPhat() > 0) {
        throw BusinessRuleException("Khong the xoa doc gia con no tien phat chua thanh toan!", "ReaderService");
    }

    // Xoa khoi repository
    readerRepo->remove(maND);

    // Ghi log
    if (logger != nullptr) {
        logger->logInfo("ReaderService", "Xoa nguoi dung thanh cong: " + maND);
    }
}

// 5. Thay doi mat khau
void ReaderService::changePassword(const string& maND, const string& oldPass, const string& newPass) {
    NguoiDung* user = readerRepo->findById(maND);
    if (user == nullptr) {
        throw EntityNotFoundException("Khong tim thay tai khoan voi ma: " + maND, "ReaderService");
    }

    // Kiem tra mat khau cu
    if (!user->kiemTraMatKhau(oldPass)) {
        throw BusinessRuleException("Mat khau cu khong chinh xac!", "ReaderService");
    }

    // Kiem tra mat khau moi
    if (newPass.empty() || newPass.find('|') != string::npos) {
        throw ValidationException("Mat khau moi khong duoc de trong va khong chua ky tu '|'!", "ReaderService");
    }

    // Dat mat khau moi va dong bo xuong file
    user->setMatKhau(newPass);
    readerRepo->saveToFile();

    // Ghi log
    if (logger != nullptr) {
        logger->logInfo("ReaderService", "Doi mat khau thanh cong cho ma: " + maND);
    }
}

// 6. Cap nhat ho so ca nhan (ho ten, so dien thoai)
void ReaderService::updateProfile(const string& maND, const string& newName, const string& newPhone) {
    NguoiDung* user = readerRepo->findById(maND);
    if (user == nullptr) {
        throw EntityNotFoundException("Khong tim thay nguoi dung voi ma: " + maND, "ReaderService");
    }

    // Kiem tra hop le ho ten
    if (newName.empty() || newName.find('|') != string::npos) {
        throw ValidationException("Ho ten khong duoc de trong va khong chua ky tu '|'!", "ReaderService");
    }

    // Kiem tra hop le so dien thoai
    if (newPhone.length() != 10 || newPhone[0] != '0') {
        throw ValidationException("So dien thoai phai gom dung 10 chu so va bat dau bang so 0!", "ReaderService");
    }
    for (int i = 0; i < 10; i++) {
        if (newPhone[i] < '0' || newPhone[i] > '9') {
            throw ValidationException("So dien thoai chi duoc chua cac chu so!", "ReaderService");
        }
    }

    // Cap nhat va dong bo xuong file
    user->setHoTen(newName);
    user->setSoDienThoai(newPhone);
    readerRepo->saveToFile();

    // Ghi log
    if (logger != nullptr) {
        logger->logInfo("ReaderService", "Cap nhat ho so ca nhan thanh cong cho ma: " + maND);
    }
}

// 7. Thanh toan / xoa tien phat cho doc gia
void ReaderService::clearFine(const string& maDocGia, double amount) {
    NguoiDung* user = readerRepo->findById(maDocGia);
    if (user == nullptr) {
        throw EntityNotFoundException("Khong tim thay doc gia voi ma: " + maDocGia, "ReaderService");
    }

    DocGia* dg = dynamic_cast<DocGia*>(user);
    if (dg == nullptr) {
        throw BusinessRuleException("Nguoi dung nay khong phai la doc gia!", "ReaderService");
    }

    // Tru tien phat da thanh toan
    dg->thanhToanPhat(amount);
    readerRepo->saveToFile();

    // Ghi log
    if (logger != nullptr) {
        logger->logInfo("ReaderService", "Thanh toan tien phat: " + to_string((long long)amount) + " VND cho doc gia " + maDocGia);
    }
}

// 8. Lay so tien phat hien tai cua doc gia
double ReaderService::getFineAmount(const string& maDocGia) const {
    NguoiDung* user = readerRepo->findById(maDocGia);
    if (user == nullptr) {
        throw EntityNotFoundException("Khong tim thay doc gia voi ma: " + maDocGia, "ReaderService");
    }

    DocGia* dg = dynamic_cast<DocGia*>(user);
    if (dg == nullptr) {
        return 0.0;
    }

    return dg->getTongTienPhat();
}

// 9. Tra cuu nguoi dung theo ma trong O(1)
NguoiDung* ReaderService::getReaderById(const string& maND) const {
    return readerRepo->findById(maND);
}

// 10. Lay toan bo danh sach nguoi dung
DoublyLinkedList<NguoiDung*>& ReaderService::getAllUsers() {
    return readerRepo->getUserList();
}

const DoublyLinkedList<NguoiDung*>& ReaderService::getAllUsers() const {
    return readerRepo->getUserList();
}
