/**
 * Fat.c
 * Created by Matheus Leme Da Silva
 */
#include "Fat.h"
#include "MemDefs.h"
#include "Util.h"
#include "Vga.h"

// Returns 1 if cluster is end
int FatClstIsEnd(struct FatPart p, U32 clst) {
	U32 val = 0;
	if (p.GFatType == PART_FAT12) {
		val = 0xFF8;
	} else if (p.GFatType == PART_FAT16) {
		val = 0xFFF8;
	} else if (p.GFatType == PART_FAT32) {
		val = 0xFFFFFF8;
	} else {
		return 1;
	}

	return clst >= val;
}

// Returns 1 if cluster is bad
int FatClstIsBad(struct FatPart p, U32 clst) {
	U32 val = 0;
	if (p.GFatType == PART_FAT12) {
		val = 0xFF7;
	} else if (p.GFatType == PART_FAT16) {
		val = 0xFFF7;
	} else if (p.GFatType == PART_FAT32) {
		val = 0xFFFFFF7;
	} else {
		return 1;
	}

	return clst == val;
}

// Converts cluster to LBA
U32 FatClstToLba(struct FatPart p, U32 clst) {
	return ((clst - 2) * p.Bpb.SctsPerClst) + p.GDataLba;
}

// Converts a filename to FAT name
void FilenameToFatname(char *filename, char *out) {
	if (!filename || !out)
		return;

	for (int i = 0; i < 11; i++) {
		out[i] = ' ';
	}

	int extPos = 0;
	for (int i = 0; i < 8; i++) {
	if (filename[i] == '.') {
		extPos = i;
		break;
	} else if (filename[i] == 0)
		return;

	char c = filename[i];

	if (c >= 'a' && c <= 'z')
		c -= ('a' - 'A');

	out[i] = c;
	}

	for (int i = 0; i < 3; i++) {
	char c = filename[extPos + i + 1];
	if (c == 0)
		return;
	if (c >= 'a' && c <= 'z')
		c -= ('a' - 'A');

	out[8 + i] = c;
	}
}

// Initializes a FAT partition;
// Returns non zero if have error;
int FatInit(U8 drive, U32 StartLBA, struct FatPart *out) {
	struct FatPart p;

	U8 vbr[SCT_SIZE];
	DiskRead(drive, StartLBA, 1, vbr);

	Memcpy(&p.Bpb, vbr, sizeof(struct Bpb));
	Memcpy(&p.Ebpb, &vbr[sizeof(struct Bpb)], sizeof(union Ebpb));

	if (p.Bpb.BytesPerSct != SCT_SIZE) {
		Puts("Invalid sect size\r\n");
		p.GFatType = PART_INV;
		return 1;
	}

	p.StartLBA = StartLBA;
	p.Drive = drive;

	p.GTotalScts = p.Bpb.TotalScts16 == 0 ? p.Bpb.TotalScts32 : p.Bpb.TotalScts16;
	p.GFatSz = p.Bpb.SctsPerFat == 0 ? p.Ebpb.Ebpb32.SctsPerFat32 : p.Bpb.SctsPerFat;
	p.GRootDirScts = ((p.Bpb.RootDirEnts * 32) + (SCT_SIZE - 1)) / SCT_SIZE;

	p.GFatLba = StartLBA + p.Bpb.ResScts;
	p.GDataLba = p.GFatLba + (p.Bpb.FatCount * p.GFatSz) + p.GRootDirScts;
	p.GDataScts = p.GTotalScts - (p.Bpb.ResScts + (p.Bpb.FatCount * p.GFatSz) + p.GRootDirScts);
	p.GRootLba = p.GDataLba - p.GRootDirScts;

	U32 TotalClsts = p.GDataScts / p.Bpb.SctsPerClst;

	if (TotalClsts < 4085) {
		p.GFatType = PART_FAT12;
	} else if (TotalClsts < 65525) {
		p.GFatType = PART_FAT16;
	} else {
		p.GFatType = PART_FAT32;
	}

	if (out)
		*out = p;

	return 0;
}

// Returns next cluster
U32 FatNextClst(struct FatPart p, U32 clst) {
	if (p.GFatType == PART_INV || clst < 2)
		return 0;

	U8 buf[SCT_SIZE * 2];
	U32 fatOffset;

	if (p.GFatType == PART_FAT12)
		fatOffset = (clst + (clst / 2));
	else if (p.GFatType == PART_FAT16)
		fatOffset = (clst * 2);
	else if (p.GFatType == PART_FAT32)
		fatOffset = (clst * 4);

	U32 fatSector = p.GFatLba + (fatOffset / SCT_SIZE);
	U32 entryOffset = fatOffset % SCT_SIZE;

	U32 val = 0;
	if (p.GFatType == PART_FAT12) {
		DiskRead(p.Drive, fatSector, 2, buf);
		val = *(U32 *)&buf[entryOffset];
		val = (clst & 1) ? val >> 4 : val & 0xFFF;
		val &= 0xFFF;
	} else if (p.GFatType == PART_FAT16) {
		DiskRead(p.Drive, fatSector, 1, buf);
		val = *(U32 *)&buf[entryOffset];
		val &= 0xFFFF;
	} else if (p.GFatType == PART_FAT32) {
		DiskRead(p.Drive, fatSector, 1, buf);
		val = *(U32 *)&buf[entryOffset];
		val &= 0xFFFFFFF;
	}

	return val;
}

// Find Dir entry in a directory;
// Returns non zero if have error;
// If clst is zero and FAT is 12/16, reads in root directory;
// TODO: LFN Support
int FatFindInDir(struct FatPart p, U32 clst, struct FatDirEntry *out,
				 char *filename) {
	if (!filename)
		return 1;

	U32 curClst = clst;

	char fatname[11];
	FilenameToFatname(filename, fatname);

	while (!FatClstIsEnd(p, curClst)) {
		U32 scts = curClst == 0 ? p.GRootDirScts : p.Bpb.SctsPerClst;
		for (U32 i = 0; i < scts; i++) {
			U8 sct[SCT_SIZE];
			U32 lba = curClst == 0 ? p.GRootLba + i : FatClstToLba(p, curClst) + i;
			DiskRead(p.Drive, lba, 1, sct);

			struct FatDirEntry *ents = (struct FatDirEntry *)sct;

			for (int j = 0; j < 16; j++) {
				if (ents[j].Name[0] == 0)
					return 1;

				if (Memcmp(ents[j].Name, fatname, 11) == 0) {
					if (out)
						*out = ents[j];
					return 0;
				}
			}
		}

		if (clst == 0)
			return 1;

		curClst = FatNextClst(p, curClst);
	}

	return 1;
}

// Find a entry with complete PATH;
// Returns non zero if have error;
int FatFind(struct FatPart p, const char *path, struct FatDirEntry *out) {
	if (!path)
		return 1;

	struct FatDirEntry e;
	U16 curClst = p.GFatType == PART_FAT32 ? p.Ebpb.Ebpb32.RootDirClst : 0;
	U32 part = 0;
	while (1) {
		char filename[11];
		if (GetPathPart(path, filename, part++, 11) != 0)
			break;

		if (FatFindInDir(p, curClst, &e, filename) != 0)
			return 1;

		if (e.Attr & ARCHIVE)
			break;

		curClst = e.ClstLo;
		if (p.GFatType == PART_FAT32) {
			curClst |= (e.ClstHi << 16);
			curClst &= 0xFFFFFFF;
		}
	}

	if (out)
		*out = e;

	return 0;
}

// Read N bytes at OFFSET in E;
// Returns total of bytes readed
U32 FatRead(struct FatPart p, struct FatDirEntry e, U32 off, U32 n, void *d) {
	if (e.ClstLo < 2 || !d)
		return 0;

	U32 clstSize = p.Bpb.SctsPerClst * SCT_SIZE;
	U32 clst = e.ClstLo;
	clst = e.ClstLo;
	if (p.GFatType == PART_FAT32) {
		clst |= (e.ClstHi << 16);
		clst &= 0xFFFFFFF;
	}

	U32 skip = off / clstSize;
	U32 offInClst = off % clstSize;

	// Skip clsts
	for (U32 i = 0; i < skip; i++) {
		clst = FatNextClst(p, clst);
		if (FatClstIsEnd(p, clst))
			return 0;
	}

	U8 *dest = (U8 *)d;

	U32 remaining = n;
	U32 curOff = 0;
	U32 totalRead = 0;
	while (!FatClstIsEnd(p, clst) && remaining > 0) {
		U32 clstLba = FatClstToLba(p, clst);
		U32 localOff = offInClst;
		U8 *buf = (U8 *)CLST_BUFFER;

		DiskRead(p.Drive, clstLba, p.Bpb.SctsPerClst, buf);

		U32 maxInClst = clstSize - localOff;
		U32 toRead = remaining < maxInClst ? remaining : maxInClst;
		for (U32 i = 0; i < toRead; i++)
			dest[curOff + i] = buf[localOff + i];

		remaining -= toRead;
		totalRead += toRead;
		curOff += toRead;
		localOff = 0;
		clst = FatNextClst(p, clst);
	}

	return totalRead;
}
