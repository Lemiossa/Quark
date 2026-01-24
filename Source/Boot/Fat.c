/**
 * Fat.c
 * Created by Matheus Leme Da Silva
 */
#include "Fat.h"
#include "Util.h"
#include "Vga.h"

// Returns 1 if cluster is end
int FatClstIsEnd(struct FatPart p, U16 clst) {
  U16 val = 0;
  if (p.GFatType == PART_FAT12) {
    val = 0xFF8;
  } else if (p.GFatType == PART_FAT16) {
    val = 0xFFF8;
  } else {
    return 1;
  }

  return clst >= val;
}

// Returns 1 if cluster is bad
int FatClstIsBad(struct FatPart p, U16 clst) {
  U16 val = 0;
  if (p.GFatType == PART_FAT12) {
    val = 0xFF7;
  } else if (p.GFatType == PART_FAT16) {
    val = 0xFFF7;
  } else {
    return 1;
  }

  return clst == val;
}

// Converts cluster to LBA
U32 FatClstToLba(struct FatPart p, U16 clst) {
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

// Initializes a FAT partition
// Returns non zero if have error
int FatInit(U8 drive, U32 StartLBA) {
  struct FatPart p;

  U8 vbr[SCT_SIZE];
  DiskRead(drive, StartLBA, vbr);

  Memcpy(&p.Bpb, vbr, sizeof(struct Bpb));
  Memcpy(&p.Ebpb, &vbr[sizeof(struct Bpb)], sizeof(struct Ebpb));

  if (p.Bpb.BytesPerSct != SCT_SIZE) {
    p.GFatType = PART_INV;
    return 1;
  }

  p.StartLBA = StartLBA;
  p.Drive = drive;

  p.GTotalScts = p.Bpb.TotalScts16 == 0 ? p.Bpb.TotalScts32 : p.Bpb.TotalScts16;
  p.GFatSz = p.Bpb.SctsPerFat;
  p.GRootDirScts = ((p.Bpb.RootDirEnts * 32) + (SCT_SIZE - 1)) / SCT_SIZE;
  p.GFatLba = StartLBA + p.Bpb.ResScts;
  p.GDataLba = p.GFatLba + (p.Bpb.FatCount * p.GFatSz) + p.GRootDirScts;
  p.GDataScts = p.GTotalScts -
                (p.Bpb.ResScts + (p.Bpb.FatCount * p.GFatSz) + p.GRootDirScts);
  p.GRootLba = p.GDataLba - p.GRootDirScts;

  U32 TotalClsts = p.GDataScts / p.Bpb.SctsPerClst;

  if (TotalClsts < 4085) {
    p.GFatType = PART_FAT12;
  } else if (TotalClsts < 65525) {
    p.GFatType = PART_FAT16;
  }

  return 0;
}

// Returns next cluster
U16 FatNextClst(struct FatPart part, U16 clst) {
  if (part.GFatType == PART_INV || clst < 2)
    return 0xFFF8;

  U8 buf[SCT_SIZE * 2];
  U32 fatOffset = part.GFatType == PART_FAT12   ? (clst + (clst / 2))
                  : part.GFatType == PART_FAT16 ? (clst * 2)
                                                : 0;
  U32 fatSector = part.GFatLba + (fatOffset / SCT_SIZE);
  U32 entryOffset = fatOffset % SCT_SIZE;

  U16 val = 0;
  if (part.GFatType == PART_FAT12) {
    DiskRead(part.Drive, fatSector, buf);
    DiskRead(part.Drive, fatSector + 1, &buf[512]);
    val = *(U16 *)&buf[entryOffset];
    val = (clst & 1) ? val >> 4 : val & 0xFFF;
  } else if (part.GFatType == PART_FAT16) {
    DiskRead(part.Drive, fatSector, buf);
    val = *(U16 *)&buf[entryOffset];
  }

  return val;
}

// Find Dir entry in a directory
// Returns non zero if have error
// If clst is zero and FAT is 12/16, reads in root directory
int FatFindInDir(struct FatPart p, U16 clst, struct FatDirEntry *out,
                 char *filename) {
  if (!filename)
    return 1;
  U16 curClst = clst;

  char fatname[11];
  FilenameToFatname(filename, fatname);

  while (!FatClstIsEnd(p, curClst)) {
    U32 scts = curClst == 0 ? p.GRootDirScts : p.Bpb.SctsPerClst;
    for (U32 i = 0; i < scts; i++) {
      U8 sct[SCT_SIZE];
      U32 lba = curClst == 0 ? p.GRootLba + i : FatClstToLba(p, curClst) + i;
      DiskRead(p.Drive, lba, sct);

      for (int j = 0; j < SCT_SIZE; j += sizeof(struct FatDirEntry)) {
        struct FatDirEntry *entry = (struct FatDirEntry *)&sct[j];

        if (entry->Name[0] == 0)
          return 1;

        if (Memcmp(entry->Name, fatname, 11) == 0) {
          if (out)
            *out = *entry;
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
