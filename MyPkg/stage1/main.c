#include "my.h"
#include <Library/BaseLib.h> // StrLen()

EFI_SYSTEM_TABLE  *g_ST;
EFI_BOOT_SERVICES *g_BS;

/*
EFI_STATUS prompt()
{
    g_ST->ConOut->OutputString(g_ST->ConOut, L"Hello World\r\n");

    //SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key);
    //SystemTable->ConOut->OutputString(SystemTable->ConOut, &key.UnicodeChar);

    EFI_INPUT_KEY Key;
    UINTN KeyEvent = 0;
    while (Key.ScanCode != SCAN_ESC)
    {
        g_BS->WaitForEvent(1, &g_ST->ConIn->WaitForKey, &KeyEvent);
        g_ST->ConIn->ReadKeyStroke(g_ST->ConIn, &Key);
        g_ST->ConIn->Reset(g_ST->ConIn, FALSE);
        g_ST->ConOut->OutputString(g_ST->ConOut, &Key.UnicodeChar);
    }

    return EFI_SUCCESS;
}
*/

#include "stage2.in"

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE  *SystemTable)
{
   // Setup global variables
   g_ST = SystemTable;
   g_BS = g_ST->BootServices;

    // Clear screen
    g_ST->ConOut->ClearScreen(g_ST->ConOut);

    Info(L"Hello hackers!");

    EFI_STATUS Status = 0;
    EFI_FILE_PROTOCOL *Root;
    Status = GetFileIo(&Root);
    if (EFI_ERROR(Status))
    {
        Error(L"Cannot open volume");
        return Status;
    }

    CHAR16 *FileName = (CHAR16 *)L"\\seukrai";
    unsigned char *Buffer = stage2_efi;
    UINTN BufferSize = stage2_efi_len;
    Status = WriteFile(Root, FileName, Buffer, &BufferSize);
    if (EFI_ERROR(Status))
    {
        Error(L"Cannot write to file");
        return Status;
    }

    EFI_DEVICE_PATH* DevicePath = NULL;
    Status = FindFile(FileName, &DevicePath);
    if (EFI_ERROR(Status))
    {
        Error(L"Cannot find file");
        return Status;
    }

    EFI_HANDLE NextImageHandle = NULL;
    Status = ImageLoad(ImageHandle, DevicePath, &NextImageHandle);
    if (EFI_ERROR(Status))
    {
        Error(L"Cannot load image");
        return Status;
    }

    Status = ImageStart(NextImageHandle);
    if (EFI_ERROR(Status))
    {
        Error(L"Cannot start image");
        return Status;
    }

    return EFI_SUCCESS;
}