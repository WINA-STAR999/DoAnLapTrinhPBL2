cmake_minimum_required(VERSION 3.10)
project(LibraryManagement_PBL2 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Them cac co canh bao bien dich
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Thu muc chua cac file header
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Danh sach cac file ma nguon .cpp
set(SOURCES
    src/Utils/ConfigManager.cpp
    src/Utils/FileLogger.cpp
    src/Utils/DateUtils.cpp
    src/Models/NguoiDung.cpp
    src/Models/ThuThu.cpp
    src/Models/DocGia.cpp
    src/Models/Sach.cpp
    src/Models/GiaoDich.cpp
    src/Repositories/BookRepository.cpp
    src/Repositories/ReaderRepository.cpp
    src/Repositories/TransactionRepository.cpp
    src/BLL/BookService.cpp
    src/BLL/ReaderService.cpp
    src/BLL/BorrowService.cpp
    src/UI/AppInterface.cpp
    src/main.cpp
)

# Tao file thuc thi
add_executable(LibraryManagement_PBL2 ${SOURCES})
