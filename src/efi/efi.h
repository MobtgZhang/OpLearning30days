/**
 * 自定义 UEFI 类型定义（不依赖 GNU-EFI）
 * 基于 UEFI Specification 2.10
 */
#ifndef EFI_H
#define EFI_H

/* 基础类型 */
typedef unsigned long long  UINTN;
typedef unsigned char      UINT8;
typedef unsigned short     UINT16;
typedef unsigned int       UINT32;
typedef unsigned long long UINT64;
typedef signed int         INT32;
typedef UINT64             EFI_PHYSICAL_ADDRESS;
typedef UINT64             EFI_VIRTUAL_ADDRESS;
typedef void               *EFI_HANDLE;
typedef void               *EFI_EVENT;

/* 状态码 */
typedef UINTN EFI_STATUS;
#define EFI_SUCCESS              0
#define EFI_LOAD_ERROR           (1ULL << (sizeof(EFI_STATUS)*8 - 1))
#define EFI_INVALID_PARAMETER    (EFI_LOAD_ERROR | 2)
#define EFI_UNSUPPORTED          (EFI_LOAD_ERROR | 3)
#define EFI_NOT_READY            (EFI_LOAD_ERROR | 6)
#define EFI_DEVICE_ERROR         (EFI_LOAD_ERROR | 7)
#define EFI_NOT_FOUND            (EFI_LOAD_ERROR | 14)

/* 表头 */
typedef struct {
	UINT64  Signature;
	UINT32  Revision;
	UINT32  HeaderSize;
	UINT32  CRC32;
	UINT32  Reserved;
} EFI_TABLE_HEADER;

/* 简单文本输出 */
typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_TEXT_RESET)(
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, unsigned char ExtendedVerification);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_TEXT_STRING)(
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, UINT16 *String);

struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
	void            *Reset;
	EFI_TEXT_STRING OutputString;
	void            *TestString;
	void            *QueryMode;
	void            *SetMode;
	void            *SetAttribute;
	void            *ClearScreen;
	void            *SetCursorPosition;
	void            *EnableCursor;
	void            *Mode;
};

/* 引导服务函数类型（调用时强转使用） */
typedef struct EFI_BOOT_SERVICES EFI_BOOT_SERVICES;
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_LOCATE_PROTOCOL)(
	void *Protocol, void *Registration, void **Interface);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_HANDLE_PROTOCOL)(
	EFI_HANDLE Handle, void *Protocol, void **Interface);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_EXIT_BOOT_SERVICES)(
	EFI_HANDLE ImageHandle, UINTN MapKey);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_ALLOCATE_POOL)(
	UINTN PoolType, UINTN Size, void **Buffer);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_FREE_POOL)(void *Buffer);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_CREATE_EVENT)(
	UINT32 Type, UINTN NotifyTpl, void *NotifyFunction, void *NotifyContext,
	EFI_EVENT *Event);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_WAIT_FOR_EVENT)(
	UINTN NumberOfEvents, EFI_EVENT *Event, UINTN *Index);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_CLOSE_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_STALL)(UINTN Microseconds);
typedef void* EFI_RUNTIME_SERVICES;

/* 定时器事件（WaitForEvent 与指针 WaitForInput 并用，避免只轮询 Stall 时 GetState 永远 NOT_READY） */
#define EVT_TIMER       0x80000000u
#define TPL_APPLICATION 4
typedef enum {
	TimerCancel,
	TimerPeriodic,
	TimerRelative
} EFI_TIMER_DELAY;
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_SET_TIMER)(
	EFI_EVENT Event, EFI_TIMER_DELAY Type, UINT64 TriggerTime);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_CHECK_EVENT)(EFI_EVENT Event);
typedef enum {
	AllHandles = 0,
	ByRegisterNotify = 1,
	ByProtocol = 2
} EFI_LOCATE_SEARCH_TYPE;
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_LOCATE_HANDLE_BUFFER)(
	EFI_LOCATE_SEARCH_TYPE SearchType, void *Protocol, void *SearchKey,
	UINTN *NoHandles, EFI_HANDLE **Buffer);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_CONNECT_CONTROLLER)(
	EFI_HANDLE ControllerHandle, EFI_HANDLE DriverImageHandle,
	void *RemainingDevicePath, unsigned char Recursive);

/* 成员顺序必须与 UEFI Spec 2.10 一致 */
struct EFI_BOOT_SERVICES {
	EFI_TABLE_HEADER Hdr;
	void *RaiseTPL;
	void *RestoreTPL;
	void *AllocatePages;
	void *FreePages;
	void *GetMemoryMap;
	void *AllocatePool;
	void *FreePool;
	void *CreateEvent;
	void *SetTimer;
	void *WaitForEvent;
	void *SignalEvent;
	void *CloseEvent;
	void *CheckEvent;
	void *InstallProtocolInterface;
	void *ReinstallProtocolInterface;
	void *UninstallProtocolInterface;
	void *HandleProtocol;
	void *Reserved;
	void *RegisterProtocolNotify;
	void *LocateHandle;
	void *LocateDevicePath;
	void *InstallConfigurationTable;
	void *LoadImage;
	void *StartImage;
	void *Exit;
	void *UnloadImage;
	void *ExitBootServices;
	void *GetNextMonotonicCount;
	void *Stall;
	void *SetWatchdogTimer;
	void *ConnectController;
	void *DisconnectController;
	void *OpenProtocol;
	void *CloseProtocol;
	void *OpenProtocolInformation;
	void *ProtocolsPerHandle;
	void *LocateHandleBuffer;
	void *LocateProtocol;
	void *InstallMultipleProtocolInterfaces;
	void *UninstallMultipleProtocolInterfaces;
	void *CalculateCrc32;
	void *CopyMem;
	void *SetMem;
	void *CreateEventEx;
};

/* 系统表 */
typedef struct {
	EFI_TABLE_HEADER                  Hdr;
	UINT16                            *FirmwareVendor;
	UINT32                            FirmwareRevision;
	EFI_HANDLE                        ConsoleInHandle;
	void                              *ConIn;
	EFI_HANDLE                        ConsoleOutHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut;
	EFI_HANDLE                        StandardErrorHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *StdErr;
	EFI_RUNTIME_SERVICES              *RuntimeServices;
	EFI_BOOT_SERVICES                 *BootServices;
	UINTN                             NumberOfTableEntries;
	void                              *ConfigurationTable;
} EFI_SYSTEM_TABLE;

/* GOP 相关 */
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
	{ 0x9042a9de, 0x23dc, 0x4a38, { 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a } }

typedef struct {
	UINT32 RedMask;
	UINT32 GreenMask;
	UINT32 BlueMask;
	UINT32 ReservedMask;
} EFI_PIXEL_BITMASK;

typedef enum {
	PixelRedGreenBlueReserved8BitPerColor,
	PixelBlueGreenRedReserved8BitPerColor,
	PixelBitMask,
	PixelBltOnly,
	PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
	UINT32                     Version;
	UINT32                     HorizontalResolution;
	UINT32                     VerticalResolution;
	EFI_GRAPHICS_PIXEL_FORMAT  PixelFormat;
	EFI_PIXEL_BITMASK          PixelInformation;
	UINT32                     PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
	UINT32                                 MaxMode;
	UINT32                                 Mode;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *Info;
	UINTN                                  SizeOfInfo;
	EFI_PHYSICAL_ADDRESS                   FrameBufferBase;
	UINTN                                  FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_GRAPHICS_OUTPUT_QUERY_MODE)(
	EFI_GRAPHICS_OUTPUT_PROTOCOL *This, UINT32 ModeNumber, UINTN *SizeOfInfo,
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_GRAPHICS_OUTPUT_SET_MODE)(
	EFI_GRAPHICS_OUTPUT_PROTOCOL *This, UINT32 ModeNumber);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_GRAPHICS_OUTPUT_BLT)(
	EFI_GRAPHICS_OUTPUT_PROTOCOL *This, void *BltBuffer, UINT32 BltOperation,
	UINTN SourceX, UINTN SourceY, UINTN DestinationX, UINTN DestinationY,
	UINTN Width, UINTN Height, UINTN Delta);

struct EFI_GRAPHICS_OUTPUT_PROTOCOL {
	EFI_GRAPHICS_OUTPUT_QUERY_MODE QueryMode;
	EFI_GRAPHICS_OUTPUT_SET_MODE   SetMode;
	EFI_GRAPHICS_OUTPUT_BLT        Blt;
	EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
};

/* Simple Pointer (鼠标) */
#define EFI_SIMPLE_POINTER_PROTOCOL_GUID \
	{ 0x31878c87, 0x0b75, 0x11d5, { 0x9a, 0x4f, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d } }

typedef struct {
	INT32 RelativeMovementX;
	INT32 RelativeMovementY;
	INT32 RelativeMovementZ;
	UINT8 LeftButton;
	UINT8 RightButton;
} EFI_SIMPLE_POINTER_STATE;

typedef struct {
	UINT64 ResolutionX;
	UINT64 ResolutionY;
	UINT64 ResolutionZ;
	UINT8 LeftButton;
	UINT8 RightButton;
} EFI_SIMPLE_POINTER_MODE;

typedef struct EFI_SIMPLE_POINTER_PROTOCOL EFI_SIMPLE_POINTER_PROTOCOL;
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_SIMPLE_POINTER_RESET)(
	EFI_SIMPLE_POINTER_PROTOCOL *This, unsigned char ExtendedVerification);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_SIMPLE_POINTER_GET_STATE)(
	EFI_SIMPLE_POINTER_PROTOCOL *This, EFI_SIMPLE_POINTER_STATE *State);

struct EFI_SIMPLE_POINTER_PROTOCOL {
	EFI_SIMPLE_POINTER_RESET      Reset;
	EFI_SIMPLE_POINTER_GET_STATE  GetState;
	EFI_EVENT                     WaitForInput;
	EFI_SIMPLE_POINTER_MODE       *Mode;
};

#define EFI_ABSOLUTE_POINTER_PROTOCOL_GUID \
	{ 0x8D59D32B, 0xC655, 0x4AE9, \
	  { 0x9B, 0x15, 0xF2, 0x59, 0x04, 0x99, 0x2A, 0x43 } }

typedef struct {
	UINT64 AbsoluteMinX, AbsoluteMinY, AbsoluteMinZ;
	UINT64 AbsoluteMaxX, AbsoluteMaxY, AbsoluteMaxZ;
	UINT32 Attributes;
} EFI_ABSOLUTE_POINTER_MODE;

typedef struct {
	UINT64 CurrentX, CurrentY, CurrentZ;
	UINT32 ActiveButtons;
} EFI_ABSOLUTE_POINTER_STATE;

typedef struct EFI_ABSOLUTE_POINTER_PROTOCOL EFI_ABSOLUTE_POINTER_PROTOCOL;
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_ABSOLUTE_POINTER_RESET_ABS)(
	EFI_ABSOLUTE_POINTER_PROTOCOL *This, unsigned char ExtendedVerification);
typedef EFI_STATUS (__attribute__((ms_abi)) *EFI_ABSOLUTE_POINTER_GET_STATE_ABS)(
	EFI_ABSOLUTE_POINTER_PROTOCOL *This, EFI_ABSOLUTE_POINTER_STATE *State);

struct EFI_ABSOLUTE_POINTER_PROTOCOL {
	EFI_ABSOLUTE_POINTER_RESET_ABS     Reset;
	EFI_ABSOLUTE_POINTER_GET_STATE_ABS GetState;
	EFI_EVENT                          WaitForInput;
	EFI_ABSOLUTE_POINTER_MODE          *Mode;
};

/* GUID 结构 (16 字节) */
typedef struct {
	UINT32 Data1;
	UINT16 Data2;
	UINT16 Data3;
	UINT8  Data4[8];
} EFI_GUID;

#endif /* EFI_H */
