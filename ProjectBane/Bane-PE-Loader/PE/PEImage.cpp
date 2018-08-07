#include "PEImage.h"
#include "Win32Helpers.h"
#include <cstring>

PEImage::PEImage(const std::string& inPath)
	: path(inPath)
{
	FILE* f = fopen(path.c_str(), "rb");
	if (f == nullptr)
	{
		printf("We have failed big time!!!!");
		return;
	}

	fileHandle = std::shared_ptr<FILE>(f, fclose);
	fseek(fileHandle.get(), 0, SEEK_END);
	fileSize = ftell(fileHandle.get());
	fseek(fileHandle.get(), 0, SEEK_SET);

	if (!ParseDosHeader()) {
		printf("ParseDosHeader");
		return;
	}

	if (!ParsePEHeader()) {
		printf("ParsePEHeader");
		return;
	}

	if (!ParseImageOptionalHeader()) {
		printf("ParseImageOptionalHeader");
		return;
	}

	if (!ParseSectionTables())
	{
		printf("ParseSectionTables");
		return;
	}
}



std::shared_ptr<PEImage> PEImage::create(const std::string& path) {
	return std::make_shared<PEImage>(path);
}

uint64 PEImage::GetFilesize() const {
	return fileSize;
}

bool PEImage::ParseDosHeader()
{
	if (fileHandle == nullptr) {
		return false;
	}

	DosHeader dos;
	memset(&dos, 0, sizeof(dos));
	if (sizeof(dos) > GetFilesize())
	{
		return false;
	}

	if (sizeof(dos) != fread(&dos, 1, sizeof(dos), fileHandle.get()))
	{
		return false;
	}
	if (dos.magic[0] != 'M' || dos.magic[1] != 'Z')
	{
		return false;
	}
	dosHeader = std::make_optional<DosHeader>(dos);
	return true;
}

bool PEImage::ParsePEHeader()
{
	if (!dosHeader || fileHandle == nullptr) {
		return false;
	}

	PEHeader peh;
	memset(&peh, 0, sizeof(peh));

	if (fseek(fileHandle.get(), dosHeader->lfanew, SEEK_SET))
	{
		return false;
	}
	if (sizeof(peh) != fread(&peh, 1, sizeof(peh), fileHandle.get()))
	{

		return false;
	}
	if (peh.Signature[0] != 'P' || peh.Signature[1] != 'E' || peh.Signature[2] != '\x00' || peh.Signature[3] != '\x00')
	{

		return false;
	}
	peHeader = std::make_optional<PEHeader>(peh);

	return true;
}

bool PEImage::ParseImageOptionalHeader()
{
	if (!peHeader || fileHandle == nullptr) {
		return false;
	}

	ImageOptionalHeader ioh;
	memset(&ioh, 0, sizeof(ioh));

	if (peHeader->SizeOfOptionalHeader == 0)
	{

		return true;
	}

	if (fseek(fileHandle.get(), dosHeader->lfanew + sizeof(PEHeader), SEEK_SET))
	{
		return false;
	}

	if (0x18 != fread(&ioh, 1, 0x18, fileHandle.get()))
	{

		return false;
	}

	if (ioh.Magic != IMAGE_OPTIONAL_HEADER_MAGIC.at("PE32") && ioh.Magic != IMAGE_OPTIONAL_HEADER_MAGIC.at("PE32+"))
	{

		return false;
	}
	else if (ioh.Magic == IMAGE_OPTIONAL_HEADER_MAGIC.at("PE32"))
	{
		if (4 != fread(&ioh.BaseOfData, 1, 4, fileHandle.get()) || 4 != fread(&ioh.ImageBase, 1, 4, fileHandle.get()))
		{
			return false;
		}
	}
	else
	{

		if (8 != fread(&ioh.ImageBase, 1, 8, fileHandle.get()))
		{
			return false;
		}
	}

	if (0x28 != fread(&ioh.SectionAlignment, 1, 0x28, fileHandle.get()))
	{
		return false;
	}

	if (ioh.FileAlignment == 0 || ioh.SectionAlignment == 0)
	{
		return false;
	}

	if (ioh.Magic == IMAGE_OPTIONAL_HEADER_MAGIC.at("PE32+"))
	{
		if (40 != fread(&ioh.SizeofStackReserve, 1, 40, fileHandle.get()))
		{
			return false;
		}
	}
	else
	{
		fread(&ioh.SizeofStackReserve, 1, 4, fileHandle.get());
		fread(&ioh.SizeofStackCommit, 1, 4, fileHandle.get());
		fread(&ioh.SizeofHeapReserve, 1, 4, fileHandle.get());
		fread(&ioh.SizeofHeapCommit, 1, 4, fileHandle.get());
		fread(&ioh.LoaderFlags, 1, 4, fileHandle.get());
		fread(&ioh.NumberOfRvaAndSizes, 1, 4, fileHandle.get());
		if (feof(fileHandle.get()) || ferror(fileHandle.get()))
		{

			return false;
		}

		//if (ioh.NumberOfRvaAndSizes > 0x10) {

		//}

		for (unsigned int i = 0; i < std::min(ioh.NumberOfRvaAndSizes, static_cast<uint32>(0x10)); ++i)
		{
			if (8 != fread(&ioh.directories[i], 1, 8, fileHandle.get()))
			{

				return false;
			}
		}

		imageOptionalHeader = std::make_optional<ImageOptionalHeader>(ioh);
	}

	return true;
}

bool PEImage::ParseSectionTables()
{
	if (!peHeader || !dosHeader || fileHandle == nullptr) {
		return false;
	}

	if (fseek(fileHandle.get(), dosHeader->lfanew + sizeof(PEHeader) + peHeader->SizeOfOptionalHeader, SEEK_SET))
	{
		return false;
	}

	for (int i = 0; i < peHeader->NumberofSections; ++i)
	{
		ImageSectionHeader sec;
		memset(&sec, 0, sizeof(ImageSectionHeader));
		if (sizeof(ImageSectionHeader) != fread(&sec, 1, sizeof(ImageSectionHeader), fileHandle.get()))
		{

			return false;
		}

		printf("Section HeaderName %s", sec.Name);
	}

	return true;
}

bool PEImage::ParseDirectories()
{
	if (fileHandle == nullptr)
		return false;

	return ParseExports();
}

bool PEImage::ParseExports()
{
	if (!imageOptionalHeader || fileHandle == nullptr) {
		return false;
	}

	ImageExportDirectory ied;
	unsigned int ied_size = 9 * sizeof(uint32) + 2 * sizeof(uint16);
	memset(&ied, 0, ied_size);

	if (!ReadDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT)) {
		return true; // No exports
	}

	return true;
}

bool PEImage::ReadDirectory(int directory)
{
	if (fileHandle == nullptr) {
		return false;
	}

	if (directory > 0x10)
	{
		return false;
	}

	if (!imageOptionalHeader)
	{
		return false;
	}

	if (imageOptionalHeader->directories[directory].VirtualAddress == 0 && imageOptionalHeader->directories[directory].Size == 0) {
		return false;
	}
	else if (imageOptionalHeader->directories[directory].Size == 0)
	{
	}
	else if (imageOptionalHeader->directories[directory].VirtualAddress == 0)
	{
		return false;
	}

	//unsigned int offset = rva_to_offset(imageOptionalHeader->directories[directory].VirtualAddress);

	//if (!offset || fseek(fileHandle.get(), offset, SEEK_SET))
	//{
	//	return false;
	//}

	return true;
}
