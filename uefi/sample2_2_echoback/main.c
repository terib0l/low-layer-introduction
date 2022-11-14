typedef unsigned long long ull;

struct EFI_INPUT_KEY {
  unsigned short ScanCode;
  unsigned short UnicodeChar;
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
    ull _buf7[3];

    // DriverSupport Services
    ull _buf8[2];

    // Open and Close Protocol Services
    ull _buf9[3];

    // Library Services
    ull _buf10[5];

    // 32-bit CRC Services
    ull _buf11;

    // Miscellaneous Services
    ull _buf12[3];
  } *BootServices;
};

void efi_main(
  void *ImageHandle __attribute__ ((unused)),
  struct EFI_SYSTEM_TABLE *SystemTable
){
  struct EFI_INPUT_KEY key;
  ull waitidx;
  unsigned short str[3];

  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

  while(1) {
    // Wait for inputed key
    SystemTable->BootServices->WaitForEvent(1, &(SystemTable->ConIn->WaitForKey), &waitidx);

    // Get inputed key
    SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key);
    if(key.UnicodeChar != L'\r'){
      str[0] = key.UnicodeChar;
      str[1] = L'\0';
    } else {
      str[0] = L'\r';
      str[1] = L'\n';
      str[2] = L'\0';
    }

    // Output inputed key
    SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
  }
}
