#include "../../include/Models/ThuThu.h"

using namespace std;

// Khoi tao doi tuong ThuThu
ThuThu::ThuThu(const string& ma, const string& ten, const string& sdt, const string& mk, const string& ca)
    : NguoiDung(ma, ten, sdt, mk), caTruc(ca) {}

// Tra ve vai tro quan tri vien
string ThuThu::layVaiTro() const {
    return "ADMIN";
}

// Xuat chuoi luu file: maND|hoTen|soDienThoai|matKhau|ADMIN|caTruc
string ThuThu::serializeToFile() const {
    return maND + "|" + hoTen + "|" + soDienThoai + "|" + matKhau + "|ADMIN|" + caTruc;
}

string ThuThu::getCaTruc() const {
    return caTruc;
}

void ThuThu::setCaTruc(const string& ca) {
    caTruc = ca;
}
