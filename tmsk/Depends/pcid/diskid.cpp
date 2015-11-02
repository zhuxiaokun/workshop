#include "diskid.h"
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>

// DoIDENTIFY
// FUNCTION: Send an IDENTIFY command to the drive
// bDriveNum = 0-3
// bIDCmd = IDE_ATA_IDENTIFY or IDE_ATAPI_IDENTIFY
BOOL DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
				 PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
				 PDWORD lpcbBytesReturned)
{
	// Set up data structures for IDENTIFY command.
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
	pSCIP -> irDriveRegs.bFeaturesReg = 0;
	pSCIP -> irDriveRegs.bSectorCountReg = 1;
	//pSCIP -> irDriveRegs.bSectorNumberReg = 1;
	pSCIP -> irDriveRegs.bCylLowReg = 0;
	pSCIP -> irDriveRegs.bCylHighReg = 0;

	// Compute the drive number.
	pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

	// The command can either be IDE identify or ATAPI identify.
	pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
	pSCIP -> bDriveNumber = bDriveNum;
	pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;

	return ( DeviceIoControl (hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
		(LPVOID) pSCIP,
		sizeof(SENDCMDINPARAMS) - 1,
		(LPVOID) pSCOP,
		sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
		lpcbBytesReturned, NULL) );
}

bool IsFixedIdeDrive(DWORD diskdata[256])
{
	if(diskdata == NULL)
		return false;

	return ((diskdata [0] & 0x0040) != 0);
}

bool FormatIdeInformation(char OutputString[256], DWORD diskdata[256], int DriveInfo)
{
	int firstIndex = 0;
	int lastIndex = 0;
	__int64 sectors = 0;
	__int64 bytes = 0;

	switch(DriveInfo)
	{
		case DriveType:
			if (diskdata [0] & 0x0080)
				strcpy(OutputString, "Removable");
			else if (diskdata [0] & 0x0040)
				strcpy(OutputString, "Fixed");
			else
				strcpy(OutputString, "Unknown");
			return true;
		case DriveSerialNumber:
			firstIndex = 10;
			lastIndex = 19;
			break;
		case DriveModelNumber:
			firstIndex = 27;
			lastIndex = 46;
			break;
		case DriveControllerRevisionNumber:
			firstIndex = 23;
			lastIndex = 26;
			break;
		case DriveControllerBufferSize:
			ultoa(diskdata [21] * 512, OutputString, 10);
			return true;
		case DriveSize:
			if (diskdata [83] & 0x400) 
				sectors =	diskdata [103] * 65536I64 * 65536I64 * 65536I64 + 
							diskdata [102] * 65536I64 * 65536I64 + 
							diskdata [101] * 65536I64 + 
							diskdata [100];
			else
				sectors = diskdata [61] * 65536 + diskdata [60];
			bytes = sectors * 512;
			_i64toa(bytes, OutputString, 10);			
		default:
			return false;
	}

	int index = 0;
	int position = 0;

	//  each integer has two characters stored in it backwards
	for (index = firstIndex; index <= lastIndex; index++)
	{
		//  get high byte for 1st character
		OutputString [position] = (char) (diskdata [index] / 256);
		position++;

		//  get low byte for 2nd character
		OutputString [position] = (char) (diskdata [index] % 256);
		position++;
	}

	//  end the string 
	OutputString [position] = '\0';

	//  cut off the trailing blanks
	for (index = position - 1; index > 0 && ' ' == OutputString [index]; index--)
		OutputString [index] = '\0';

	return true;
}

bool ReadIdePhysicalDriveInNT(int drive, DWORD diskdata[256])
{
	if(drive >= MAX_IDE_DRIVES)
		return false;

	BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
	bool done = false;

	HANDLE hPhysicalDriveIOCTL = 0;

	//  Try to get a handle to PhysicalDrive IOCTL, report failure
	//  and exit if can't.
	char driveName [256];
	sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);

	//  Windows NT, Windows 2000, must have admin rights
	hPhysicalDriveIOCTL = CreateFile (driveName,
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE , NULL,
		OPEN_EXISTING, 0, NULL);

	if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
	{
		GETVERSIONOUTPARAMS VersionParams;
		DWORD               cbBytesReturned = 0;

		// Get the version, etc of PhysicalDrive IOCTL
		memset ((void*) &VersionParams, 0, sizeof(VersionParams));

		if (DeviceIoControl (hPhysicalDriveIOCTL, DFP_GET_VERSION,
				NULL, 
				0,
				&VersionParams,
				sizeof(VersionParams),
				&cbBytesReturned, NULL) )
		{
			// If there is a IDE device at number "i" issue commands
			// to the device
			if (VersionParams.bIDEDeviceMap > 0)
			{
				BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
				SENDCMDINPARAMS  scip;

				// Now, get the ID sector for all IDE devices in the system.
				// If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
				// otherwise use the IDE_ATA_IDENTIFY command
				bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;

				memset (&scip, 0, sizeof(scip));
				memset (IdOutCmd, 0, sizeof(IdOutCmd));

				if ( DoIDENTIFY (hPhysicalDriveIOCTL, 
					&scip, 
					(PSENDCMDOUTPARAMS)&IdOutCmd, 
					(BYTE) bIDCmd,
					(BYTE) drive,
					&cbBytesReturned))
				{
					int ijk = 0;
					USHORT *pIdSector = (USHORT *)
						((PSENDCMDOUTPARAMS) IdOutCmd) -> bBuffer;

					for (ijk = 0; ijk < 256; ijk++)
						diskdata [ijk] = pIdSector [ijk];

					done = true;
				}
			}
		}

		CloseHandle (hPhysicalDriveIOCTL);
	}

	return done;
}

#define SENDIDLENGTH  sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE

bool ReadIdePhysicalDriveInNTByScsi(int drive, DWORD diskdata[256])
{
	if(drive >= MAX_IDE_DRIVES*2)
		return false;

	bool done = false;
	int controller = drive / 2;
	drive = drive % 2;

	HANDLE hScsiDriveIOCTL = 0;
	char   driveName [256];

	//  Try to get a handle to PhysicalDrive IOCTL, report failure
	//  and exit if can't.
	sprintf (driveName, "\\\\.\\Scsi%d:", controller);

	//  Windows NT, Windows 2000, any rights should do
	hScsiDriveIOCTL = CreateFile (driveName,
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, 0, NULL);

	if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
	{
		char buffer [sizeof (SRB_IO_CONTROL) + SENDIDLENGTH];
		SRB_IO_CONTROL *p = (SRB_IO_CONTROL *) buffer;
		SENDCMDINPARAMS *pin =
			(SENDCMDINPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
		DWORD dummy;

		memset (buffer, 0, sizeof (buffer));
		p -> HeaderLength = sizeof (SRB_IO_CONTROL);
		p -> Timeout = 10000;
		p -> Length = SENDIDLENGTH;
		p -> ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
		strncpy ((char *) p -> Signature, "SCSIDISK", 8);

		pin -> irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
		pin -> bDriveNumber = drive;

		if (DeviceIoControl (hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
			buffer,
			sizeof (SRB_IO_CONTROL) +
			sizeof (SENDCMDINPARAMS) - 1,
			buffer,
			sizeof (SRB_IO_CONTROL) + SENDIDLENGTH,
			&dummy, NULL))
		{
			SENDCMDOUTPARAMS *pOut =
				(SENDCMDOUTPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
			IDSECTOR *pId = (IDSECTOR *) (pOut -> bBuffer);
			if (pId -> sModelNumber [0])
			{
				int ijk = 0;
				USHORT *pIdSector = (USHORT *) pId;

				for (ijk = 0; ijk < 256; ijk++)
					diskdata [ijk] = pIdSector [ijk];

				done = true;
			}
		}
		CloseHandle (hScsiDriveIOCTL);
	}

	return done;
}

//Drive information in 98

static WORD g_DriveBuffer98[256];
volatile static bool g_bReadDrive98 = false;
static DWORD g_OldInterruptAddress;
static DWORDLONG g_IDTR;
static WORD g_IdeBasePort;
static BYTE g_IdeSelectCmd;

#define ATA_GDIcmd		0xEC    //GDI command for ATA
#define ATAPI_GDIcmd	0xA1    //GDI command for ATAPI

//选择Master/Slave
inline void SelectDevice(WORD BasePort, BYTE SelectCmd)
{
	_asm
	{
		mov dx, BasePort
		add dx, 6
		mov al, SelectCmd
		out dx, al
	}

	return;
}

//发送读驱动器参数命令
inline void SendGDICmd(WORD BasePort, BYTE GDICmd)
{
	_asm
	{
		mov dx, BasePort
		add dx, 7											
		mov al, GDICmd
		out dx, al
	}

	return;
}

//取错误码
inline void GetError(WORD BasePort, BYTE& byError)
{
	BYTE byTemp;

	_asm
	{
		mov  dx, BasePort
		add  dx, 7											
		in   al, dx
		mov  byTemp, al
	}

	byError = byTemp;

	return;
}

static bool WaitHardDiskIdle(BYTE& byAL)
{   
	BYTE byTemp;
	WORD IdePort;
	
	IdePort = g_IdeBasePort + 7;
	DWORD LoopCount = 100000;

BUSYWAIT:
	if(LoopCount-- == 0)
		return false;	//超时

	_asm
	{
		mov  dx, IdePort		
		in   al, dx
		test al, 0x80
		jnz BUSYWAIT
		mov  byTemp, al
	}
	
	byAL = byTemp;

	return true;
}   

//Interrupt proc
void _declspec(naked)InterruptProcess(void)
{
	BYTE byAL;

	//保存寄存器值
	_asm
	{
		push   eax
		push   ebx
		push   ecx
		push   edx
		push   esi
	}

	//检查IDE是否存在
	_asm
	{
		mov dx, g_IdeBasePort
		add dx, 7
		in  al, dx
		mov byAL, al
	}

	if(byAL == 0xFF || byAL == 0x7F)	
		goto FINISH;			//IDE not present

	if(!WaitHardDiskIdle(byAL))			
		goto FINISH;			//等待超时		
	
	//选择Master/Slave
	SelectDevice(g_IdeBasePort, g_IdeSelectCmd);

	if(!WaitHardDiskIdle(byAL))			
		goto FINISH;			//等待超时	

	if((byAL & 0x40) == 0)	//Disk not exist
		goto FINISH;

	//ATA Command
	if(!WaitHardDiskIdle(byAL))			
		goto FINISH;			//等待超时	

	//选择Master/Slave
	SelectDevice(g_IdeBasePort, g_IdeSelectCmd);
	SendGDICmd(g_IdeBasePort, ATA_GDIcmd);

	if(!WaitHardDiskIdle(byAL))			
		goto FINISH;			//等待超时	

	GetError(g_IdeBasePort, byAL);
	
	if((byAL & 0x1) == 0)	//no error
		goto READINFO;

	//ATAPI Command
	if(!WaitHardDiskIdle(byAL))			
		goto FINISH;			//等待超时	

	//选择Master/Slave
	SelectDevice(g_IdeBasePort, g_IdeSelectCmd);
	SendGDICmd(g_IdeBasePort, ATAPI_GDIcmd);

	if(!WaitHardDiskIdle(byAL))			
		goto FINISH;			//等待超时	

	GetError(g_IdeBasePort, byAL);

	if((byAL & 0x1) == 0)	//no error
		goto READINFO;

	//读取信息失败，结束
	goto FINISH;

READINFO:
	//读取硬盘控制器的全部信息
	_asm
	{
		LEA edi, g_DriveBuffer98
		mov ecx, 256
		mov dx, g_IdeBasePort
		CLD

		REP INSW
	}

	g_bReadDrive98 = true;

FINISH:
	_asm
	{
		pop esi
		pop edx
		pop ecx
		pop ebx
		pop eax
		iretd
	} 
}

bool ReadIdePhysicalDriveIn98(int drive, DWORD diskdata[256])
{
	g_bReadDrive98 = false;
	int num = 0;

	switch(drive)
	{
		case 0:
			g_IdeBasePort = 0x1f0;
			g_IdeSelectCmd = 0xa0;
			break;
		case 1:
			g_IdeBasePort = 0x1f0;
			g_IdeSelectCmd = 0xb0;
			break;
		case 2:
			g_IdeBasePort = 0x170;
			g_IdeSelectCmd = 0xa0;
			break;
		case 3:
			g_IdeBasePort = 0x170;
			g_IdeSelectCmd = 0xb0;
			break;
		case 4:
			g_IdeBasePort = 0x1e8;
			g_IdeSelectCmd = 0xa0;
			break;
		case 5:
			g_IdeBasePort = 0x1e8;
			g_IdeSelectCmd = 0xb0;
			break;
		case 6:
			g_IdeBasePort = 0x168;
			g_IdeSelectCmd = 0xa0;
			break;
		case 7:
			g_IdeBasePort = 0x168;
			g_IdeSelectCmd = 0xb0;
			break;
		default:			//98下只支持4个IDE通道，每通道2个硬盘(Master/Salve)
			return false;
	}

	_asm
	{
		push eax                 
		//获取修改的中断的中断描述符（中断门）地址
		sidt g_IDTR 
		mov  eax, dword ptr [g_IDTR+02h]                 
		add  eax, 3*08h+04h
		cli
		//保存原先的中断入口地址
		push ecx
		mov  ecx, dword ptr [eax]
		mov  cx, word ptr [eax-04h]
		mov  dword ptr g_OldInterruptAddress, ecx
		pop  ecx
		//设置修改的中断入口地址为新的中断处理程序入口地址
		push ebx
		lea  ebx, InterruptProcess 
		mov  word ptr [eax-04h], bx
		shr  ebx, 10h
		mov  word ptr [eax+02h], bx
		pop  ebx
		//执行中断，转到Ring 0（类似CIH病毒原理）
		int  3h
		//恢复原先的中断入口地址
		push ecx
		mov  ecx, dword ptr g_OldInterruptAddress
		mov  word ptr [eax-04h], cx
		shr  ecx, 10h
		mov  word ptr [eax+02h], cx
		pop  ecx
		sti
		pop  eax
	}

	if(g_bReadDrive98)
		for(int i = 0; i < 256; i++)
			diskdata[i] = (DWORD)(g_DriveBuffer98[i]);
	
	return g_bReadDrive98;
} 