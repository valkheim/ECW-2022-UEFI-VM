#pragma once

#include <Uefi.h>
#include <Protocol/SimpleFileSystem.h>

extern EFI_SYSTEM_TABLE  *g_ST;
extern EFI_BOOT_SERVICES *g_BS;

// Logging
VOID Info(IN CHAR16 *String);
VOID Error(IN CHAR16 *String);

// Files IO
EFI_STATUS GetFileIo(EFI_FILE_PROTOCOL **Root);
EFI_STATUS WriteFile(EFI_FILE_PROTOCOL *Root, IN CHAR16 *FileName, IN VOID *Buffer, IN UINTN *BufferSize);
EFI_STATUS FindFile(IN CHAR16 *ImagePath, OUT EFI_DEVICE_PATH **DevicePath);

// Images
EFI_STATUS ImageLoad(IN EFI_HANDLE ParentHandle, IN EFI_DEVICE_PATH* DevicePath, OUT EFI_HANDLE* ImageHandle);
EFI_STATUS ImageStart(IN EFI_HANDLE ImageHandle);