#ifndef DATA_VALIDATORS_H
#define DATA_VALIDATORS_H

#include "../Models/Sach.h"
#include "../Models/NguoiDung.h"
#include "../Exceptions/LibraryException.h"
#include "../Utils/ConfigManager.h"
#include "../Utils/DateUtils.h"
#include <string>

using namespace std;

// 1. Validator kiem tra tinh hop le cua du lieu Sach
class BookValidator {
public:
    static void validate(const Sach& s) {
        // Kiem tra ma sach: bat dau bang "BK" va theo sau la 3 den 5 chu so (tong do dai 5 den 7 ky tu)
        string ma = s.getMaSach();
        if (ma.length() < 5 || ma.length() > 7 || ma[0] != 'B' || ma[1] != 'K') {
            throw ValidationException("Ma sach phai bat dau bang 'BK' theo sau la 3 den 5 chu so (VD: BK001)!", "BookValidator");
        }
        for (int i = 2; i < (int)ma.length(); i++) {
            if (ma[i] < '0' || ma[i] > '9') {
                throw ValidationException("Ma sach phai bat dau bang 'BK' theo sau la 3 den 5 chu so (VD: BK001)!", "BookValidator");
            }
        }

        // Kiem tra ten sach khong duoc rong va khong chua dau '|'
        string ten = s.getTenSach();
        if (ten.empty()) {
            throw ValidationException("Ten sach khong duoc de trong!", "BookValidator");
        }
        if (ten.find('|') != string::npos) {
            throw ValidationException("Ten sach khong duoc chua ky tu '|'!", "BookValidator");
        }

        // Kiem tra tac gia khong duoc rong va khong chua dau '|'
        string tg = s.getTacGia();
        if (tg.empty()) {
            throw ValidationException("Tac gia khong duoc de trong!", "BookValidator");
        }
        if (tg.find('|') != string::npos) {
            throw ValidationException("Tac gia khong duoc chua ky tu '|'!", "BookValidator");
        }

        // Kiem tra the loai khong duoc rong va khong chua dau '|'
        string tl = s.getTheLoai();
        if (tl.empty()) {
            throw ValidationException("The loai sach khong duoc de trong!", "BookValidator");
        }
        if (tl.find('|') != string::npos) {
            throw ValidationException("The loai sach khong duoc chua ky tu '|'!", "BookValidator");
        }

        // Kiem tra nam xuat ban nam trong khoang cau hinh [minYear, maxYear]
        int minYear = ConfigManager::getInstance().getMinYear();
        int maxYear = ConfigManager::getInstance().getMaxYear();
        if (s.getNamXB() < minYear || s.getNamXB() > maxYear) {
            throw ValidationException("Nam xuat ban phai nam trong khoang tu " + to_string(minYear) + " den " + to_string(maxYear) + "!", "BookValidator");
        }

        // Kiem tra so luong
        if (s.getTongSoLuong() < 1) {
            throw ValidationException("Tong so luong sach phai lon hon hoac bang 1!", "BookValidator");
        }
        if (s.getSoLuongHienCo() < 0 || s.getSoLuongHienCo() > s.getTongSoLuong()) {
            throw ValidationException("So luong sach hien co phai tu 0 den tong so luong!", "BookValidator");
        }

        // Kiem tra vi tri ke sach: dung 5 ky tu theo dinh dang [A-Z][1-9]-T[1-9] (VD: A1-T2)
        string vt = s.getViTri();
        if (vt.length() != 5) {
            throw ValidationException("Vi tri sach phai dung dinh dang A1-T2 (VD: A1-T2, B3-T1)!", "BookValidator");
        }
        if (vt[0] < 'A' || vt[0] > 'Z' ||
            vt[1] < '1' || vt[1] > '9' ||
            vt[2] != '-' ||
            vt[3] != 'T' ||
            vt[4] < '1' || vt[4] > '9') {
            throw ValidationException("Vi tri sach phai dung dinh dang A1-T2 (VD: A1-T2, B3-T1)!", "BookValidator");
        }
    }
};

// 2. Validator kiem tra tinh hop le cua du lieu NguoiDung (ThuThu va DocGia)
class ReaderValidator {
public:
    static void validate(const NguoiDung& u) {
        // Kiem tra ma nguoi dung: bat dau bang "RD" hoac "AD" theo sau boi 3 den 5 chu so
        string ma = u.getMaND();
        if (ma.length() < 5 || ma.length() > 7) {
            throw ValidationException("Ma nguoi dung phai co do dai tu 5 den 7 ky tu!", "ReaderValidator");
        }

        string tienTo = ma.substr(0, 2);
        if (tienTo != "RD" && tienTo != "AD") {
            throw ValidationException("Ma nguoi dung phai bat dau bang 'RD' (Doc gia) hoac 'AD' (Thu thu)!", "ReaderValidator");
        }

        for (int i = 2; i < (int)ma.length(); i++) {
            if (ma[i] < '0' || ma[i] > '9') {
                throw ValidationException("Ma nguoi dung phai chua cac chu so sau tien to RD/AD!", "ReaderValidator");
            }
        }

        // Kiem tra so dien thoai: dung 10 chu so, bat dau bang '0'
        string sdt = u.getSoDienThoai();
        if (sdt.length() != 10 || sdt[0] != '0') {
            throw ValidationException("So dien thoai phai gom dung 10 chu so va bat dau bang so 0!", "ReaderValidator");
        }
        for (int i = 0; i < 10; i++) {
            if (sdt[i] < '0' || sdt[i] > '9') {
                throw ValidationException("So dien thoai chi duoc chua cac chu so!", "ReaderValidator");
            }
        }

        // Kiem tra ho ten va mat khau
        string ten = u.getHoTen();
        if (ten.empty()) {
            throw ValidationException("Ho ten nguoi dung khong duoc de trong!", "ReaderValidator");
        }
        if (ten.find('|') != string::npos) {
            throw ValidationException("Ho ten khong duoc chua ky tu '|'!", "ReaderValidator");
        }

        string mk = u.getMatKhau();
        if (mk.empty()) {
            throw ValidationException("Mat khau khong duoc de trong!", "ReaderValidator");
        }
        if (mk.find('|') != string::npos) {
            throw ValidationException("Mat khau khong duoc chua ky tu '|'!", "ReaderValidator");
        }
    }
};

// 3. Validator kiem tra tinh hop le cua thoi gian trong giao dich
class TransactionValidator {
public:
    static void validateDate(const string& dateStr) {
        if (!DateUtils::isValidDate(dateStr)) {
            throw ValidationException("Dinh dang ngay khong hop le hoac sai ngay thuc te (yeu cau DD/MM/YYYY): " + dateStr, "TransactionValidator");
        }
    }
};

#endif // DATA_VALIDATORS_H
