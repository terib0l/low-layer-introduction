typedef unsigned long long ull;

struct EFI_SYSTEM_TABLE {
  char _buf[60];
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
  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
  SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello UEFI!\n");

  while (1);
}
