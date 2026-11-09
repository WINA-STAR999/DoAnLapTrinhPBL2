#ifndef NGUOI_DUNG_H
#define NGUOI_DUNG_H

#include <string>

using namespace std;

// Lop co so truu tuong NguoiDung dai dien cho tat ca nguoi dung trong he thong
class NguoiDung {
protected:
    string maND;        // Ma dinh danh nguoi dung (vi du: AD001, RD001)
    string hoTen;       // Ho va ten day du
    string soDienThoai; // So dien thoai (10 chu so bat dau bang 0)
    string matKhau;     // Mat khau dang nhap

public:
    // Ham tao khoi tao cac thong tin chung cua nguoi dung
    NguoiDung(const string& ma = "", const string& ten = "", const string& sdt = "", const string& mk = "");

    // Ham huy ao de dam bao giai phong dung khi xoa thong qua con tro NguoiDung*
    virtual ~NguoiDung() = default;

    // Phuong thuc thuan ao tra ve vai tro: "ADMIN" hoac "READER"
    virtual string layVaiTro() const = 0;

    // Phuong thuc thuan ao xuat chuoi du lieu ngan cach boi dau '|' de luu xuong file readers.txt
    virtual string serializeToFile() const = 0;

    // Kiem tra mat khau dang nhap co khop hay khong
    bool kiemTraMatKhau(const string& mk) const;

    // Cac ham getter va setter cho cac thuoc tinh
    string getMaND() const;
    void setMaND(const string& ma);

    string getHoTen() const;
    void setHoTen(const string& ten);

    string getSoDienThoai() const;
    void setSoDienThoai(const string& sdt);

    string getMatKhau() const;
    void setMatKhau(const string& mk);
};

#endif // NGUOI_DUNG_H
