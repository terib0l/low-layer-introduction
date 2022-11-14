#ifndef _EFI_H_
#define _EFI_H_

typedef unsigned long long ull;

struct EFI_INPUT_KEY {
  unsigned short ScanCode;
  unsigned short UnicodeChar;
};

struct EFI_GUID {
  unsigned int Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned char Data4[8];
};

struct EFI_SYSTEM_TABLE {
  char _buf[44];
  struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
    ull _buf;
    ull (*ReadKeyStroke)(
      struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This,
      struct EFI_INPUT_KEY *Key
    );
    void *WaitForKey;
  } *ConIn;

  char _buf2[8];
  struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    ull _buf;
    ull (*OutputString)(
      struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
      unsigned short *String
    );
    ull _buf2[4];
    ull (*ClearScreen)(
      struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This
    );
  } *ConOut;

  char _buf3[24];
  struct EFI_BOOT_SERVICES {
    ull _buf1[3];

    // Task Priority Services
    ull _buf2[2];

    // Memory Services
    ull _buf3[5];

    // Event & Timer Services
    ull _buf4[2];
    ull (*WaitForEvent)(
        ull NumberOfEvents,
        void **Event,
        ull *Index
    );
    ull _buf4_2[3];

    // Protocol Handler Services
    ull _buf5[9];

    // Image Services
    ull _buf6[5];

    // Miscellaneous Services
    ull _buf7[2];
    ull (*SetWatchdogTimer)(
        ull Timeout,
        ull WatchdogCode,
        ull DataSize,
        unsigned short *WatchdogData
    );

    // DriverSupport Services
    ull _buf8[2];

    // Open and Close Protocol Services
    ull _buf9[3];

    // Library Services
    ull _buf10[2];
    ull (*LocateProtocol)(
        struct EFI_GUID *Protocol,
        void *Registration,
        void **Interface
    );
    ull _buf10_2[2];

    // 32-bit CRC Services
    ull _buf11;

    // Miscellaneous Services
    ull _buf12[3];
  } *BootServices;
};

struct EFI_GRAPHICS_OUTPUT_PROTOCOL {
  ull _buf[3];
  struct EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE {
    unsigned int MaxMode;
    unsigned int Mode;
    struct EFI_GRAPHICS_OUTPUT_MODE_INFORMATION {
      unsigned int Version;
      unsigned int HorizontalResolution;
      unsigned int VerticalResolution;
      enum EFI_GRAPHICS_PIXEL_FORMAT {
        PixelRedGreenBlueReserved8BitPerColor,
        PixelBlueGreenRedReserved8BitPerColor,
        PixelBitMask,
        PixelBltOnly,
        PixelFormatMax
      } PixelFormat;
    } *Info;
    ull SizeOfInfo;
    ull FrameBufferBase;
  } *Mode;
};

struct EFI_GRAPHICS_OUTPUT_BLT_PIXEL {
  unsigned char Blue;
  unsigned char Green;
  unsigned char Red;
  unsigned char Reserved;
};

extern struct EFI_SYSTEM_TABLE *ST;
extern struct EFI_GRAPHICS_OUTPUT_PROTOCOL *GOP;

void efi_init(struct EFI_SYSTEM_TABLE *SystemTable);

#endif
