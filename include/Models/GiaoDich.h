#ifndef GIAO_DICH_H
#define GIAO_DICH_H

#include "Sach.h"
#include <string>

using namespace std;

// 1. Lop GiaoDich dai dien cho mot luot muon hoac tra sach
class GiaoDich {
private:
    string maGD;            // Ma giao dich (vi du: GD001)
    string maDocGia;        // Ma doc gia muon sach (RDxxx)
    string maSach;          // Ma sach duoc muon (BKxxx)
    string ngayMuon;        // Ngay muon sach (DD/MM/YYYY)
    string hanTra;          // Han tra sach du kien (DD/MM/YYYY)
    string ngayTraThucTe;   // Ngay tra sach thuc te (mac dinh "CHUA_TRA")
    string trangThai;       // Trang thai: "DANG_MUON" hoac "DA_TRA"
    double tienPhat;        // So tien phat neu tra tre han

public:
    GiaoDich(const string& mgd = "", const string& mdg = "", const string& ms = "",
             const string& nm = "", const string& ht = "", const string& nttt = "CHUA_TRA",
             const string& tt = "DANG_MUON", double tp = 0.0);

    // Xuat chuoi dinh dang luu file transactions.txt: maGD|maDocGia|maSach|ngayMuon|hanTra|ngayTraThucTe|trangThai|tienPhat
    string serializeToFile() const;

    // Getter va Setter
    string getMaGD() const;
    void setMaGD(const string& mgd);

    string getMaDocGia() const;
    void setMaDocGia(const string& mdg);

    string getMaSach() const;
    void setMaSach(const string& ms);

    string getNgayMuon() const;
    void setNgayMuon(const string& nm);

    string getHanTra() const;
    void setHanTra(const string& ht);

    string getNgayTraThucTe() const;
    void setNgayTraThucTe(const string& nttt);

    string getTrangThai() const;
    void setTrangThai(const string& tt);

    double getTienPhat() const;
    void setTienPhat(double tp);
};

// 2. Cau truc YeuCauMuon dai dien cho mot yeu cau trong hang doi cho (Waitlist FIFO Queue)
struct YeuCauMuon {
    string maDocGia;    // Ma doc gia dang cho
    string maSach;      // Ma sach dang cho muon
    string ngayDangKy;  // Ngay dang ky vao hang doi (DD/MM/YYYY)

    YeuCauMuon(const string& mdg = "", const string& ms = "", const string& ndk = "");

    // Xuat chuoi dinh dang luu file waitlist_queue.txt: maDocGia|maSach|ngayDangKy
    string serializeToFile() const;
};

// 3. Kieu liet ke cac hanh dong de ho tro Undo Stack cho Sach
enum ActionType {
    ADD_BOOK,       // Hanh dong them sach moi
    UPDATE_BOOK,    // Hanh dong sua thong tin sach
    DELETE_BOOK     // Hanh dong xoa sach
};

// Cau truc luu thong tin thao tac phuc vu Undo Stack cua BookService
struct ActionLog {
    ActionType type;    // Loai thao tac da thuc hien
    Sach oldState;      // Trang thai cu cua sach truoc khi thao tac
    Sach newState;      // Trang thai moi cua sach sau khi thao tac

    ActionLog(ActionType t = ADD_BOOK, const Sach& oldS = Sach(), const Sach& newS = Sach());
};

// 4. Cau truc LichSuGiaoDich luu log trong ngan xep lich su toan he thong (History Stack - LIFO)
struct LichSuGiaoDich {
    string maLog;           // Ma nhat ky (vi du: LG101)
    string loaiGiaoDich;    // Loai giao dich (MUON_SACH, TRA_SACH_DUNG_HAN, TRA_SACH_TRE, THEM_SACH, SUA_SACH, XOA_SACH, UNDO_SACH)
    string maSach;          // Ma sach lien quan
    string maDocGia;        // Ma doc gia lien quan
    string ngayThucHien;    // Ngay thuc hien thao tac (DD/MM/YYYY)

    LichSuGiaoDich(const string& ml = "", const string& lgd = "", const string& ms = "",
                   const string& mdg = "", const string& nth = "");

    // Xuat chuoi dinh dang luu file history_stack.txt: maLog|loaiGiaoDich|maSach|maDocGia|ngayThucHien
    string serializeToFile() const;
};

#endif // GIAO_DICH_H
