#ifndef SACH_H
#define SACH_H

#include <string>

using namespace std;

// Lop Sach dai dien cho thuc the sach trong kho thu vien
class Sach {
private:
    string maSach;          // Ma dinh danh sach (BKxxx)
    string tenSach;         // Ten tua de sach
    string tacGia;          // Ten tac gia
    string theLoai;         // The loai sach (Giao Trinh, Tham Khao, Cong Nghe...)
    int namXB;              // Nam xuat ban
    int tongSoLuong;        // Tong so luong sach nhap vao thu vien
    int soLuongHienCo;      // So luong sach hien con tren gia (co the cho muon)
    string viTri;           // Vi tri ke sach theo dinh dang A1-T2 -> Kệ A1 - Tầng 2

public:
    // Ham tao mac dinh va ham tao day du 8 tham so
    Sach(const string& ma = "", const string& ten = "", const string& tg = "", const string& tl = "", 
         int nam = 0, int tongSL = 0, int slHienCo = 0, const string& vt = "");

    // Xuat chuoi dinh dang luu file books.txt voi 8 truong ngan cach boi '|'
    string serializeToFile() const;

    // Cac ham getter lay thong tin
    string getMaSach() const;
    string getTenSach() const;
    string getTacGia() const;
    string getTheLoai() const;
    int getNamXB() const;
    int getTongSoLuong() const;
    int getSoLuongHienCo() const;
    string getViTri() const;

    // Cac ham setter cap nhat thong tin
    void setMaSach(const string& ma);
    void setTenSach(const string& ten);
    void setTacGia(const string& tg);
    void setTheLoai(const string& tl);
    void setNamXB(int nam);
    void setTongSoLuong(int tongSL);
    void setSoLuongHienCo(int slHienCo);
    void setViTri(const string& vt);

    // Cac ham ho tro nghiep vu muon tra
    bool giamSoLuongHienCo();
    void tangSoLuongHienCo();
};

#endif // SACH_H
