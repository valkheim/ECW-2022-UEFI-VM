#include "my.h"
#include <Library/DevicePathLib.h> // FileDevicePath()

EFI_STATUS GetFileIo(EFI_FILE_PROTOCOL **Root)
{
	EFI_STATUS  Status = 0;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;
    Status = g_BS->LocateProtocol(
            &gEfiSimpleFileSystemProtocolGuid,
            NULL,
            (VOID**)&SimpleFileSystem
    );
    if (Status != EFI_SUCCESS) {
        return Status;
    }

    Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, Root);
    return Status;
}

EFI_STATUS WriteFile(EFI_FILE_PROTOCOL *Root, IN CHAR16 *FileName, IN VOID *Buffer, IN UINTN *BufferSize)
{
	EFI_STATUS  Status = 0;

    EFI_FILE_PROTOCOL *File = NULL;
    Status = Root->Open(
        Root,
        &File,
        FileName,
        EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
        0
    );
    if (Status != EFI_SUCCESS || !File)
    {
        Error(L"Cannot Open file");
        return Status;
    }
    
    Status = File->Write(File, BufferSize, Buffer);
    if (Status != EFI_SUCCESS)
    {
        Error(L"EFI_FILE_PROTOCOL Write file");
    }

    Status = File->Close(File);
    if (Status != EFI_SUCCESS)
    {
        Error(L"EFI_FILE_PROTOCOL Close file");
    }
    

    return Status;
}

EFI_STATUS FindFile(IN CHAR16 *ImagePath, OUT EFI_DEVICE_PATH **DevicePath)
{
	EFI_FILE_IO_INTERFACE *ioDevice;
	EFI_FILE_HANDLE handleRoots, bootFile;
	EFI_HANDLE* handleArray;
	UINTN nbHandles, i;
	EFI_STATUS efistatus;

	*DevicePath = (EFI_DEVICE_PATH *)NULL;
	efistatus = g_BS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &nbHandles, &handleArray);
	if (EFI_ERROR( efistatus ))
		return efistatus;

	for (i = 0; i < nbHandles; i++)
	{
		efistatus = g_BS->HandleProtocol( handleArray[i], &gEfiSimpleFileSystemProtocolGuid, (VOID**)&ioDevice );
		if (EFI_ERROR( efistatus ))
			continue;

		efistatus = ioDevice->OpenVolume( ioDevice, &handleRoots );
		if (EFI_ERROR( efistatus ))
			continue;

		efistatus = handleRoots->Open( handleRoots, &bootFile, ImagePath, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY );
		if (!EFI_ERROR( efistatus ))
		{
			handleRoots->Close( bootFile );
			*DevicePath = FileDevicePath(handleArray[i], ImagePath);
			break;
		}
	}

	return efistatus;
}
