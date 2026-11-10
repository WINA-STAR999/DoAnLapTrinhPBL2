#include "../../include/Repositories/ReaderRepository.h"
#include <fstream>
#include <iostream>

using namespace std;

// Ham tao: khoi tao bang bam voi kich thuoc tu ConfigManager va doc du lieu nguoi dung
ReaderRepository::ReaderRepository(const string& path)
    : userHash(ConfigManager::getInstance().getHashTableSize()) {
    if (path.empty()) {
        filePath = ConfigManager::getInstance().getReadersPath();
    } else {
        filePath = path;
    }
    loadFromFile();
}

// Ham huy: giai phong toan bo cac doi tuong NguoiDung* duoc cap phat dong
ReaderRepository::~ReaderRepository() {
    clear();
}

// Them mot nguoi dung moi vao danh sach va bang bam
void ReaderRepository::add(NguoiDung* const& user) {
    if (user == nullptr) {
        throw ValidationException("Khong the them nguoi dung rong (nullptr) vao he thong!", "ReaderRepository");
    }

    // 1. Chen con tro NguoiDung* vao cuoi danh sach lien ket doi
    userList.insertTail(user);

    // 2. Luu con tro vao bang bam de tim kiem theo maND voi thoi gian O(1)
    userHash.insert(user->getMaND(), user);

    // 3. Luu thay doi xuong file readers.txt
    saveToFile();
}

// Cap nhat thong tin nguoi dung
void ReaderRepository::update(NguoiDung* const& user) {
    if (user == nullptr) {
        throw ValidationException("Khong the cap nhat nguoi dung rong (nullptr)!", "ReaderRepository");
    }

    // Tim kiem doi tuong nguoi dung hien co trong bang bam O(1)
    NguoiDung* existing = userHash.search(user->getMaND());
    if (existing == nullptr) {
        throw EntityNotFoundException("Khong tim thay nguoi dung voi ma: " + user->getMaND(), "ReaderRepository");
    }

    // Neu doi tuong truyen vao khac voi doi tuong dang nam trong repo thi sao chep cac truong
    if (existing != user) {
        existing->setHoTen(user->getHoTen());
        existing->setSoDienThoai(user->getSoDienThoai());
        existing->setMatKhau(user->getMatKhau());

        // Kiem tra kieu cu the de cap nhat cac thuoc tinh dac thu cua tung vai tro
        if (existing->layVaiTro() == "ADMIN" && user->layVaiTro() == "ADMIN") {
            ThuThu* ttExisting = dynamic_cast<ThuThu*>(existing);
            ThuThu* ttUser = dynamic_cast<ThuThu*>(user);
            if (ttExisting != nullptr && ttUser != nullptr) {
                ttExisting->setCaTruc(ttUser->getCaTruc());
            }
        } else if (existing->layVaiTro() == "READER" && user->layVaiTro() == "READER") {
            DocGia* dgExisting = dynamic_cast<DocGia*>(existing);
            DocGia* dgUser = dynamic_cast<DocGia*>(user);
            if (dgExisting != nullptr && dgUser != nullptr) {
                dgExisting->setTongTienPhat(dgUser->getTongTienPhat());
            }
        }
    }

    // Dong bo du lieu xuong file readers.txt
    saveToFile();
}

// Xoa nguoi dung khoi he thong
bool ReaderRepository::remove(const string& maND) {
    // Kiem tra su ton tai cua nguoi dung trong bang bam
    NguoiDung* user = userHash.search(maND);
    if (user == nullptr) {
        return false;
    }

    // 1. Xoa khoi bang bam truoc
    userHash.remove(maND);

    // 2. Duyet danh sach lien ket doi de tim dung node va giai phong bo nho
    Node<NguoiDung*>* curr = userList.getHead();
    while (curr != nullptr) {
        if (curr->data != nullptr && curr->data->getMaND() == maND) {
            // Giai phong bo nho dong cua doi tuong da hinh (ThuThu hoac DocGia)
            delete curr->data;
            curr->data = nullptr;

            // Xoa node khoi danh sach lien ket doi
            userList.removeNode(curr);
            break;
        }
        curr = curr->next;
    }

    // 3. Luu file sau khi xoa
    saveToFile();
    return true;
}

// Tra cuu nguoi dung theo maND trong O(1)
NguoiDung* ReaderRepository::findById(const string& maND) {
    return userHash.search(maND);
}

// Nap danh sach nguoi dung tu file data/readers.txt
void ReaderRepository::loadFromFile() {
    clear(); // Giai phong toan bo bo nho cu truoc khi nap moi

    ifstream file(filePath);
    if (!file.is_open()) {
        throw FileIOException("Khong the mo file de doc nguoi dung: " + filePath, "ReaderRepository");
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;

        // Su dung UserFactory de tao doi tuong da hinh (ThuThu hoac DocGia)
        try {
            NguoiDung* user = UserFactory::createUserFromRecord(line);
            if (user != nullptr) {
                userList.insertTail(user);
                userHash.insert(user->getMaND(), user);
            }
        } catch (...) {
            // Bo qua cac dong khong dung dinh dang
            continue;
        }
    }
    file.close();
}

// Ghi danh sach nguoi dung xuong file data/readers.txt
void ReaderRepository::saveToFile() {
    ofstream file(filePath);
    if (!file.is_open()) {
        throw FileIOException("Khong the mo file de ghi nguoi dung: " + filePath, "ReaderRepository");
    }

    Node<NguoiDung*>* curr = userList.getHead();
    while (curr != nullptr) {
        if (curr->data != nullptr) {
            file << curr->data->serializeToFile() << "\n";
        }
        curr = curr->next;
    }
    file.close();
}

// Lay danh sach nguoi dung de duyet
DoublyLinkedList<NguoiDung*>& ReaderRepository::getUserList() {
    return userList;
}

const DoublyLinkedList<NguoiDung*>& ReaderRepository::getUserList() const {
    return userList;
}

// Lay so luong nguoi dung
int ReaderRepository::getCount() const {
    return userList.size();
}

// Kiem tra nguoi dung co ton tai hay khong
bool ReaderRepository::exists(const string& maND) const {
    return userHash.search(maND) != nullptr;
}

// Giai phong tat ca cac con tro NguoiDung* va lam rong danh sach cung bang bam
void ReaderRepository::clear() {
    Node<NguoiDung*>* curr = userList.getHead();
    while (curr != nullptr) {
        if (curr->data != nullptr) {
            delete curr->data;
            curr->data = nullptr;
        }
        curr = curr->next;
    }
    userList.clear();
    userHash.clear();
}
