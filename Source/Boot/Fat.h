/**
 * Fat.h
 * Created by Matheus Leme Da Silva
 */
#ifndef FAT_H
#define FAT_H
#include "Types.h"

struct Bpb {
	U8 Jump[3];
	U8 OemName[8];
	U16 BytesPerSct;
	U8 SctsPerClst;
	U16 ResScts;
	U8 FatCount;
	U16 RootDirEnts;
	U16 TotalScts16;
	U8 MediaDesc;
	U16 SctsPerFat;
	U16 SctsPerTrack;
	U16 Heads;
	U32 HiddenScts;
	U32 TotalScts32;
} __attribute__((packed));

struct Ebpb16 {
	U8 DriveNum;
	U8 Res;
	U8 Sig;
	U32 VolId;
	U8 VolLabel[11];
	U8 SysId[8];
} __attribute__((packed));

struct Ebpb32 {
	U32 SctsPerFat32;
	U16 Flags;
	U16 FatVersion;
	U32 RootDirClst;
	U16 FsInfo;
	U16 BkpSct;
	U8 Res[12];
	U8 DriveNum;
	U8 NtFlags;
	U8 Sig;
	U32 VolId;
	U8 VolLabel[11];
	U8 SysId[8];
} __attribute__((packed));

union Ebpb {
	struct Ebpb16 Ebpb16;
	struct Ebpb32 Ebpb32;
};

struct FatDirEntry {
	U8 Name[11];
	U8 Attr;
	U8 Res;
	U8 Res1;
	U16 CtdTime;
	U16 CtdDate;
	U16 AcsDate;
	U16 ClstHi;
	U16 ModTime;
	U16 ModDate;
	U16 ClstLo;
	U32 FileSz;
} __attribute__((packed));

#define READ_ONLY 0x01
#define HIDDEN 0x02
#define SYSTEM 0x04
#define VOLUME_ID 0x08
#define DIRECTORY 0x10
#define ARCHIVE 0x20

struct FatPart {
	struct Bpb Bpb;
	union Ebpb Ebpb;

	U32 StartLBA;

	U32 GTotalScts;
	U32 GFatSz;
	U32 GRootDirScts;
	U32 GDataScts;

	U32 GDataLba;
	U32 GFatLba;
	U32 GRootLba;

	U8 GFatType;
	U8 Drive;
};

#define PART_INV 0
#define PART_FAT12 1
#define PART_FAT16 2
#define PART_FAT32 3

int FatInit(U8 drive, U32 StartLBA, struct FatPart *out);
int FatFind(struct FatPart p, const char *path, struct FatDirEntry *out);
U32 FatRead(struct FatPart p, struct FatDirEntry e, U32 off, U32 n, void *d);

void FilenameToFatname(char *filename, char *out);

#endif // FAT_H
