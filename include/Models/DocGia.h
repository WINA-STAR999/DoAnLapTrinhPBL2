#ifndef DOC_GIA_H
#define DOC_GIA_H

#include "NguoiDung.h"
#include <string>

using namespace std;

// Lop DocGia dai dien cho ban doc muon sach trong thu vien (vai tro READER), ke thua tu NguoiDung
class DocGia : public NguoiDung {
private:
    double tongTienPhat; // Tong tien phat tich luy do tra sach tre han

public:
    // Ham tao lop DocGia
    DocGia(const string& ma = "", const string& ten = "", const string& sdt = "", const string& mk = "", double tienPhat = 0.0);

    // Tra ve chuoi dai dien vai tro READER
    string layVaiTro() const override;

    // Dinh dang luu file readers.txt: maND|hoTen|soDienThoai|matKhau|READER|tongTienPhat
    string serializeToFile() const override;

    // Lay tong so tien phat hien tai
    double getTongTienPhat() const;

    // Thiet lap tong tien phat
    void setTongTienPhat(double amount);

    // Cong them tien phat khi tra sach tre han
    void congPhat(double amount);

    // Thanh toan tien phat
    void thanhToanPhat(double amount);
};

#endif // DOC_GIA_H
