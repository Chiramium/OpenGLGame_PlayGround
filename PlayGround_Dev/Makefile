#
# Makefile
#

# 最終目的のファイル
TARGET = Game.exe

# ソースファイルの一覧
SRCS = Game.c
ICO = icon.ico

# リソーススクリプトファイルの一覧
RC = ${ICO:.ico=.rc}

# オブジェクトファイルの一覧
OBJS = ${SRCS:.c=.o}
ICOOBJ = ${RC:.rc=.o}

# ヘッダファイルの一覧
HEADERS =

# コンパイラ・リンカの指定
CC = gcc
CCFLAGS = -Wall -I/usr/include/opengl
LD = gcc
LDFLAGS =
LIBS = -lglpng -lglut32 -lglu32 -lopengl32 -lm
ECHO = echo
WINDRES = windres

# OBJSからTARGETを作る方法
$(TARGET) : $(OBJS) $(ICOOBJ)
	$(LD) $(OBJS) $(ICOOBJ) $(LDFLAGS) -o $(TARGET) $(LIBS)

$(ICOOBJ) : $(RC)
	$(WINDRES) -i $(RC) -o $(ICOOBJ)

$(RC) : $(ICO)
	$(ECHO) GLUT_ICON ICON $(ICO) > $(RC)

# *.c から *.o を作る方法
.c.o :
	$(CC) $(CCFLAGS) -c $<

# *.oは HEADERS と MakeFile に依存
$(OBJS) : $(HEADERS) Makefile
$(ICOOBJ) : $(RC) Makefile
$(RC) : $(ICO) Makefile

# make cleanとしたときに実行されるコマンド
clean :
	rm -f $(TARGET) $(OBJS) $(ICOOBJ) $(RC) core *-