#include "../../include/Models/Sach.h"

using namespace std;

// Khoi tao day du thong tin mot cuon sach
Sach::Sach(const string& ma, const string& ten, const string& tg, const string& tl, 
           int nam, int tongSL, int slHienCo, const string& vt)
    : maSach(ma), tenSach(ten), tacGia(tg), theLoai(tl),
      namXB(nam), tongSoLuong(tongSL), soLuongHienCo(slHienCo), viTri(vt) {}

// Xuat chuoi 8 truong ngan cach boi '|' de luu xuong file data/books.txt
string Sach::serializeToFile() const {
    return maSach + "|" + tenSach + "|" + tacGia + "|" + theLoai + "|" + 
           to_string(namXB) + "|" + to_string(tongSoLuong) + "|" + 
           to_string(soLuongHienCo) + "|" + viTri;
}

// Cac ham getter
string Sach::getMaSach() const {
    return maSach;
}

string Sach::getTenSach() const {
    return tenSach;
}

string Sach::getTacGia() const {
    return tacGia;
}

string Sach::getTheLoai() const {
    return theLoai;
}

int Sach::getNamXB() const {
    return namXB;
}

int Sach::getTongSoLuong() const {
    return tongSoLuong;
}

int Sach::getSoLuongHienCo() const {
    return soLuongHienCo;
}

string Sach::getViTri() const {
    return viTri;
}

// Cac ham setter
void Sach::setMaSach(const string& ma) {
    maSach = ma;
}

void Sach::setTenSach(const string& ten) {
    tenSach = ten;
}

void Sach::setTacGia(const string& tg) {
    tacGia = tg;
}

void Sach::setTheLoai(const string& tl) {
    theLoai = tl;
}

void Sach::setNamXB(int nam) {
    namXB = nam;
}

void Sach::setTongSoLuong(int tongSL) {
    tongSoLuong = tongSL;
}

void Sach::setSoLuongHienCo(int slHienCo) {
    soLuongHienCo = slHienCo;
}

void Sach::setViTri(const string& vt) {
    viTri = vt;
}

// Giam so luong sach tren gia khi co doc gia muon
bool Sach::giamSoLuongHienCo() {
    if (soLuongHienCo > 0) {
        soLuongHienCo--;
        return true;
    }
    return false;
}

// Tang so luong sach tren gia khi doc gia tra sach
void Sach::tangSoLuongHienCo() {
    if (soLuongHienCo < tongSoLuong) {
        soLuongHienCo++;
    }
}
