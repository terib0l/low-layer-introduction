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
};

void efi_main(
  void *ImageHandle __attribute__ ((unused)),
  struct EFI_SYSTEM_TABLE *SystemTable
){
  struct EFI_INPUT_KEY key;
  unsigned short str[3];

  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

  while(1) {
    if(!SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key)) {
      if(key.UnicodeChar != L'\r'){
        str[0] = key.UnicodeChar;
        str[1] = L'\0';
      } else {
        str[0] = L'\r';
        str[1] = L'\n';
        str[2] = L'\0';
      }
      SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
    }
  }
}
