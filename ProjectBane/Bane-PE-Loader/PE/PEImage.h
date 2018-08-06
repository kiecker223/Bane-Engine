#pragma once

#include "Common.h"
#include <memory>
#include <string>
#include <optional>
#include <algorithm>

typedef struct DosHeader
{
	uint8  magic[2];
	uint16 cblp;
	uint16 cp;
	uint16 crlc;
	uint16 cparhdr;
	uint16 minalloc;
	uint16 maxalloc;
	uint16 ss;
	uint16 sp;
	uint16 csum;
	uint16 ip;
	uint16 cs;
	uint16 lfarlc;
	uint16 ovno;
	uint16 res[4];
	uint16 oemid;
	uint16 oeminfo;
	uint16 res2[10];
	uint16 lfanew;
} DosHeader;

typedef struct  PEHeader
{
	uint8  Signature[4];
	uint16 Machine;
	uint16 NumberofSections;
	uint32 TimeDateStamp;
	uint32 PointerToSymbolTable;
	uint32 NumberOfSymbols;
	uint16 SizeOfOptionalHeader;
	uint16 Characteristics;
} PEHeader;

typedef struct ImageDataDirectory
{
	uint32 VirtualAddress;
	uint32 Size;
} ImageDataDirectory;

typedef struct ImageOptionalHeader
{
	uint16 Magic;
	uint8  MajorLinkerVersion;
	uint8  MinorLinkerVersion;
	uint32 SizeOfCode;
	uint32 SizeOfInitializedData;
	uint32 SizeOfUninitializedData;
	uint32 AddressOfEntryPoint;
	uint32 BaseOfCode;
	uint32 BaseOfData;
	uint64 ImageBase;
	uint32 SectionAlignment;
	uint32 FileAlignment;
	uint16 MajorOperatingSystemVersion;
	uint16 MinorOperatingSystemVersion;
	uint16 MajorImageVersion;
	uint16 MinorImageVersion;
	uint16 MajorSubsystemVersion;
	uint16 MinorSubsystemVersion;
	uint32 Win32VersionValue;
	uint32 SizeOfImage;
	uint32 SizeOfHeaders;
	uint32 Checksum;
	uint16 Subsystem;
	uint16 DllCharacteristics;
	uint64 SizeofStackReserve;
	uint64 SizeofStackCommit;
	uint64 SizeofHeapReserve;
	uint64 SizeofHeapCommit;
	uint32 LoaderFlags;
	uint32 NumberOfRvaAndSizes;
	ImageDataDirectory directories[0x10];
} ImageOptionalHeader;

typedef struct ImageSectionHeader
{
	uint8  Name[8];
	uint32 VirtualSize;
	uint32 VirtualAddress;
	uint32 SizeOfRawData;
	uint32 PointerToRawData;
	uint32 PointerToRelocations;
	uint32 PointerToLineNumbers;
	uint16 NumberOfRelocations;
	uint16 NumberOfLineNumbers;
	uint32 Characteristics;
} ImageSectionHeader;
typedef std::shared_ptr<ImageSectionHeader> pImageSectionHeader;

typedef struct ImageImportDescriptor
{
	uint32 OriginalFirstThunk;
	uint32 TimeDateStamp;
	uint32 ForwarderChain;
	uint32 Name;
	uint32 FirstThunk;
} ImageImportDescriptor;
typedef std::shared_ptr<ImageImportDescriptor> pImageImportDescriptor;

typedef struct ImportLookupTable
{
	uint64	AddressOfData;
	uint16	Hint;
	std::string		Name;
} ImportLookupTable;
typedef std::shared_ptr<ImportLookupTable> pImportLookupTable;

typedef struct ImageExportDirectory
{
	uint32 Characteristics;
	uint32 TimeDateStamp;
	uint16 MajorVersion;
	uint16 MinorVersion;
	uint32 Name;
	uint32 Base;
	uint32 NumberOfFunctions;
	uint32 NumberOfNames;
	uint32 AddressOfFunctions;
	uint32 AddressOfNames;
	uint32 AddressOfNameOrdinals;
	std::string		NameStr; // Non-standard!
} ImageExportDirectory;
typedef std::shared_ptr<ImageExportDirectory> peExportImageDirectory;


#if defined WIN32 && !defined DECLSPEC
#ifdef BANE_EXPORT
#define DECLSPEC    __declspec(dllexport)
#else
#define DECLSPEC    __declspec(dllimport)
#endif
#elif !defined WIN32 && !defined DECLSPEC
#define DECLSPEC
#endif

typedef std::shared_ptr<FILE> FileHandle;


class PEImage
{
public:
	PEImage(const std::string& path);
	virtual ~PEImage() {}
	static std::shared_ptr<PEImage> create(const std::string& path);
	uint64 GetFilesize() const;
private:
	bool ParseDosHeader();
	bool ParsePEHeader();
	bool ParseImageOptionalHeader();
	bool ParseSectionTables();
	bool ParseDirectories();
	bool ParseExports();
	bool ReadDirectory(int directory);

	std::string					path;
	FileHandle					fileHandle;
	uint64						fileSize;
	std::optional<DosHeader>	dosHeader;
	std::optional<PEHeader>		peHeader;
	std::optional<ImageOptionalHeader> imageOptionalHeader;


};

