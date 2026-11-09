#ifndef THU_THU_H
#define THU_THU_H

#include "NguoiDung.h"
#include <string>

using namespace std;

// Lop ThuThu dai dien cho quan tri vien thu vien (vai tro ADMIN), ke thua tu NguoiDung
class ThuThu : public NguoiDung {
private:
    string caTruc; // Ca lam viec cua thu thu (vi du: Ca Sang, Ca Chieu, Ca Toi)

public:
    // Ham tao lop ThuThu
    ThuThu(const string& ma = "", const string& ten = "", const string& sdt = "", const string& mk = "", const string& ca = "Ca Sang");

    // Tra ve chuoi dai dien vai tro ADMIN
    string layVaiTro() const override;

    // Dinh dang luu file readers.txt: maND|hoTen|soDienThoai|matKhau|ADMIN|caTruc
    string serializeToFile() const override;

    // Getter va Setter cho ca truc
    string getCaTruc() const;
    void setCaTruc(const string& ca);
};

#endif // THU_THU_H
