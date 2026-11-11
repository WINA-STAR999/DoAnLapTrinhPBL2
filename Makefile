CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
TARGET = LibraryManagement.exe

SRCS = src/Utils/ConfigManager.cpp \
       src/Utils/FileLogger.cpp \
       src/Utils/DateUtils.cpp \
       src/Models/NguoiDung.cpp \
       src/Models/ThuThu.cpp \
       src/Models/DocGia.cpp \
       src/Models/Sach.cpp \
       src/Models/GiaoDich.cpp \
       src/Repositories/BookRepository.cpp \
       src/Repositories/ReaderRepository.cpp \
       src/Repositories/TransactionRepository.cpp \
       src/BLL/BookService.cpp \
       src/BLL/ReaderService.cpp \
       src/BLL/BorrowService.cpp \
       src/UI/AppInterface.cpp \
       src/main.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q /F $(subst /,\,$(OBJS)) $(TARGET) 2>nul || rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
