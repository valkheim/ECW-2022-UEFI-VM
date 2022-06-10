#include "my.h"
#include <Library/BaseLib.h> // StrLen()
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Decompress.h>
#include <IndustryStandard/Pci.h>

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

    //Info(L"Hello hackers!");

    EFI_STATUS Status = 0;

    unsigned char *Buffer = rom;
    UINTN BufferSize = rom_len;

    EFI_DECOMPRESS_PROTOCOL *Decompress = NULL;
    EFI_GUID gEfiDecompressProtocolGuid = { 0xD8117CFE, 0x94A6, 0x11D4, { 0x9A, 0x3A, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D }};
    Status = g_BS->LocateProtocol(&gEfiDecompressProtocolGuid, NULL, (VOID **)&Decompress);
    if (EFI_ERROR(Status))
    {
        //Error(L"Cannot locate efi decompression guid");
        return Status;
    }

    UINT32 DestinationSize;
    UINT32 ScratchSize;
    Status = Decompress->GetInfo(Decompress, Buffer, (UINT32)BufferSize, &DestinationSize, &ScratchSize);
    if (EFI_ERROR(Status))
    {
        //Error(L"Cannot get info");
        return Status;
    }

    VOID *DecompressedImageBuffer = NULL;
    Status = g_BS->AllocatePool(EfiBootServicesData, DestinationSize, &DecompressedImageBuffer);
    if (DecompressedImageBuffer == NULL)
    {
        //Error(L"Cannot allocate decompressed image buffer");
        return Status;
    }

    UINT8 *Scratch = NULL;
    Status = g_BS->AllocatePool(EfiBootServicesData, ScratchSize, (VOID**)&Scratch);
    if (Scratch == NULL)
    {
        //Error(L"Cannot allocate Scratch");
        return Status;
    }

    Status = Decompress->Decompress(
        Decompress,
        Buffer,
        BufferSize,
        DecompressedImageBuffer,
        DestinationSize,
        Scratch,
        ScratchSize
    );
    if (EFI_ERROR(Status))
    {
        //Error(L"Cannot decompress");
        return Status;
    }

    Buffer = DecompressedImageBuffer;
    BufferSize = DestinationSize;

    for (int i = 0 ; i < BufferSize ; ++i)
    {
        Buffer[i] ^= 0x71;
    }

    EFI_HANDLE NextImageHandle = NULL;
	Status = g_BS->LoadImage(TRUE, ImageHandle, NULL, Buffer, BufferSize, &NextImageHandle);
	if (EFI_ERROR(Status))
	{
        //Error(L"Cannot LoadImage");
        return Status;
	}

    Status = g_BS->StartImage(NextImageHandle, (UINTN *)NULL, (CHAR16 **)NULL);
    if (EFI_ERROR(Status))
    {
        //Error(L"Cannot start image");
        return Status;
    }
    

    return EFI_SUCCESS;
}
