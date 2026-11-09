#ifndef USER_FACTORY_H
#define USER_FACTORY_H

#include "../Models/NguoiDung.h"
#include "../Models/ThuThu.h"
#include "../Models/DocGia.h"
#include "../Exceptions/LibraryException.h"
#include <string>
#include <sstream>

using namespace std;

// Lop UserFactory ap dung mau thiet ke Factory Method de tao doi tuong da hinh NguoiDung*
class UserFactory {
public:
    // Khoi tao NguoiDung* (ThuThu* hoac DocGia*) tu dong du lieu doc trong file readers.txt
    static NguoiDung* createUserFromRecord(const string& line) {
        if (line.empty()) {
            return nullptr;
        }

        // Tach chuoi theo dau '|' bang vong lap co ban
        string truong[6];
        int soLuongTruong = 0;
        string tuTam = "";

        for (int i = 0; i < (int)line.length(); i++) {
            if (line[i] == '|') {
                if (soLuongTruong < 6) {
                    truong[soLuongTruong++] = tuTam;
                    tuTam = "";
                }
            } else {
                tuTam += line[i];
            }
        }
        if (soLuongTruong < 6) {
            truong[soLuongTruong++] = tuTam;
        }

        if (soLuongTruong < 6) {
            throw ValidationException("Dong du lieu nguoi dung khong du 6 truong: " + line, "UserFactory");
        }

        string maND = truong[0];
        string hoTen = truong[1];
        string soDT = truong[2];
        string matKhau = truong[3];
        string vaiTro = truong[4];
        string thamSoThem = truong[5];

        if (vaiTro == "ADMIN") {
            // Tao doi tuong ThuThu quan tri vien
            return new ThuThu(maND, hoTen, soDT, matKhau, thamSoThem);
        } else if (vaiTro == "READER") {
            // Tao doi tuong DocGia
            double tienPhat = 0.0;
            try {
                if (!thamSoThem.empty()) {
                    tienPhat = stod(thamSoThem);
                }
            } catch (...) {
                tienPhat = 0.0;
            }
            return new DocGia(maND, hoTen, soDT, matKhau, tienPhat);
        } else {
            throw ValidationException("Vai tro nguoi dung khong hop le (yeu cau ADMIN hoac READER): " + vaiTro, "UserFactory");
        }
    }

    // Khoi tao NguoiDung* theo tham so truc tiep tu giao dien nguoi dung
    static NguoiDung* createUser(const string& role, const string& maND, const string& hoTen,
                                 const string& soDT, const string& matKhau, const string& extraParam) {
        if (role == "ADMIN") {
            string ca = extraParam.empty() ? "Ca Sang" : extraParam;
            return new ThuThu(maND, hoTen, soDT, matKhau, ca);
        } else if (role == "READER") {
            double tienPhat = 0.0;
            try {
                if (!extraParam.empty()) {
                    tienPhat = stod(extraParam);
                }
            } catch (...) {
                tienPhat = 0.0;
            }
            return new DocGia(maND, hoTen, soDT, matKhau, tienPhat);
        } else {
            throw ValidationException("Vai tro khong hop le khi tao nguoi dung: " + role, "UserFactory");
        }
    }
};

#endif // USER_FACTORY_H
