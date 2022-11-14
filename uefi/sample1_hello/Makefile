all: fs/EFI/BOOT/BOOTX64.EFI

CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wall \
				 -Wextra \
				 -e efi_main \
				 -nostdinc \
				 -nostdlib \
				 -fno-builtin \
				 -Wl,--subsystem,10
# コンパイル時の警告を全て表示
# -Wallで指定されなかった警告を表示
# エントリポイントの関数を指定
# ヘッダファイルのための標準のシステムディレクトリを検索しない
# リンク時に標準のシステムライブラリとスタートアップファイルを使用しない
# ビルトイン関数を利用しない
# 作成するファイルがUEFIアプリケーションであることを指定
TARGET = fs/EFI/BOOT/BOOTX64.EFI
SRCS = main.c

$(TARGET): $(SRCS)
	mkdir -p fs/EFI/BOOT
	$(CC) $(CFLAGS) -o $@ $<

run: fs/EFI/BOOT/BOOTX64.EFI
	qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -hda fat:rw:fs
#	qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -hda fat:fs

clean:
	rm -f $(TARGET)
