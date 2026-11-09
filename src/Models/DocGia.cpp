#include "../../include/Models/DocGia.h"

using namespace std;

// Khoi tao doi tuong DocGia voi so tien phat ban dau
DocGia::DocGia(const string& ma, const string& ten, const string& sdt, const string& mk, double tienPhat)
    : NguoiDung(ma, ten, sdt, mk), tongTienPhat(tienPhat) {}

// Tra ve vai tro doc gia
string DocGia::layVaiTro() const {
    return "READER";
}

// Xuat chuoi luu file: maND|hoTen|soDienThoai|matKhau|READER|tongTienPhat
string DocGia::serializeToFile() const {
    // Chuyen so thuc tien phat ve dang nguyen hoac khong chu so thap phan thua
    return maND + "|" + hoTen + "|" + soDienThoai + "|" + matKhau + "|READER|" + to_string((long long)tongTienPhat);
}

double DocGia::getTongTienPhat() const {
    return tongTienPhat;
}

void DocGia::setTongTienPhat(double amount) {
    tongTienPhat = amount;
}

void DocGia::congPhat(double amount) {
    if (amount > 0) {
        tongTienPhat += amount;
    }
}

void DocGia::thanhToanPhat(double amount) {
    if (amount > 0) {
        tongTienPhat -= amount;
        if (tongTienPhat < 0) {
            tongTienPhat = 0;
        }
    }
}
