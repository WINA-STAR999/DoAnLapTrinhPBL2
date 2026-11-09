#include "../../include/Models/GiaoDich.h"

using namespace std;

// === 1. GiaoDich ===
GiaoDich::GiaoDich(const string& mgd, const string& mdg, const string& ms,
                   const string& nm, const string& ht, const string& nttt,
                   const string& tt, double tp)
    : maGD(mgd), maDocGia(mdg), maSach(ms),
      ngayMuon(nm), hanTra(ht), ngayTraThucTe(nttt),
      trangThai(tt), tienPhat(tp) {}

string GiaoDich::serializeToFile() const {
    return maGD + "|" + maDocGia + "|" + maSach + "|" + 
           ngayMuon + "|" + hanTra + "|" + ngayTraThucTe + "|" + 
           trangThai + "|" + to_string((long long)tienPhat);
}

string GiaoDich::getMaGD() const { return maGD; }
void GiaoDich::setMaGD(const string& mgd) { maGD = mgd; }

string GiaoDich::getMaDocGia() const { return maDocGia; }
void GiaoDich::setMaDocGia(const string& mdg) { maDocGia = mdg; }

string GiaoDich::getMaSach() const { return maSach; }
void GiaoDich::setMaSach(const string& ms) { maSach = ms; }

string GiaoDich::getNgayMuon() const { return ngayMuon; }
void GiaoDich::setNgayMuon(const string& nm) { ngayMuon = nm; }

string GiaoDich::getHanTra() const { return hanTra; }
void GiaoDich::setHanTra(const string& ht) { hanTra = ht; }

string GiaoDich::getNgayTraThucTe() const { return ngayTraThucTe; }
void GiaoDich::setNgayTraThucTe(const string& nttt) { ngayTraThucTe = nttt; }

string GiaoDich::getTrangThai() const { return trangThai; }
void GiaoDich::setTrangThai(const string& tt) { trangThai = tt; }

double GiaoDich::getTienPhat() const { return tienPhat; }
void GiaoDich::setTienPhat(double tp) { tienPhat = tp; }

// === 2. YeuCauMuon ===
YeuCauMuon::YeuCauMuon(const string& mdg, const string& ms, const string& ndk)
    : maDocGia(mdg), maSach(ms), ngayDangKy(ndk) {}

string YeuCauMuon::serializeToFile() const {
    return maDocGia + "|" + maSach + "|" + ngayDangKy;
}

// === 3. ActionLog ===
ActionLog::ActionLog(ActionType t, const Sach& oldS, const Sach& newS)
    : type(t), oldState(oldS), newState(newS) {}

// === 4. LichSuGiaoDich ===
LichSuGiaoDich::LichSuGiaoDich(const string& ml, const string& lgd, const string& ms,
                               const string& mdg, const string& nth)
    : maLog(ml), loaiGiaoDich(lgd), maSach(ms), maDocGia(mdg), ngayThucHien(nth) {}

string LichSuGiaoDich::serializeToFile() const {
    return maLog + "|" + loaiGiaoDich + "|" + maSach + "|" + maDocGia + "|" + ngayThucHien;
}
