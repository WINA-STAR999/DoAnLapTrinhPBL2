#include "../../include/Models/NguoiDung.h"

using namespace std;

// Khoi tao thong tin nguoi dung co so
NguoiDung::NguoiDung(const string& ma, const string& ten, const string& sdt, const string& mk)
    : maND(ma), hoTen(ten), soDienThoai(sdt), matKhau(mk) {}

// Kiem tra khop mat khau
bool NguoiDung::kiemTraMatKhau(const string& mk) const {
    return matKhau == mk;
}

// Cac getter va setter
string NguoiDung::getMaND() const {
    return maND;
}

void NguoiDung::setMaND(const string& ma) {
    maND = ma;
}

string NguoiDung::getHoTen() const {
    return hoTen;
}

void NguoiDung::setHoTen(const string& ten) {
    hoTen = ten;
}

string NguoiDung::getSoDienThoai() const {
    return soDienThoai;
}

void NguoiDung::setSoDienThoai(const string& sdt) {
    soDienThoai = sdt;
}

string NguoiDung::getMatKhau() const {
    return matKhau;
}

void NguoiDung::setMatKhau(const string& mk) {
    matKhau = mk;
}
