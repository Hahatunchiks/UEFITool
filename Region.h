#ifndef UEFI_IMAGE_PARSER_REGION_H
#define UEFI_IMAGE_PARSER_REGION_H
#include <iostream>
#include <vector>
#include <bitset>
#include <map>
#include <unordered_map>

struct Region {
    std::string name;
    std::uint32_t address{};
    std::uint32_t size{};
};

struct FlashDescriptorRegion : public Region {
    FlashDescriptorRegion() {
        name = "FlashDescriptorRegion";
    }

    std::uint32_t componentSectionBase{};
    std::uint32_t regionSectionBase{};
    std::uint32_t regionAddresses[5]{};
    std::uint32_t masterSectionBase{};
};


typedef struct {
    std::uint32_t numBlocks;
    std::uint32_t length;
} EFI_FV_BLOCK_MAP;

typedef struct {
    std::uint8_t zeroVector[16];
    std::uint8_t fileSystemGuid[16];
    std::uint64_t fvLength;
    std::uint32_t signature;
    std::uint32_t attributes;
    std::uint16_t headerLength;
    std::uint16_t checksum;
    std::uint16_t extHeaderOffset;
    std::uint8_t reserved[1];
    std::uint8_t revision;

} EFI_FIRMWARE_VOLUME_HEADER;

typedef struct {
    std::uint8_t Name[16];
    std::uint8_t HeaderChecksum;
    std::uint8_t DataChecksum;
    std::uint8_t Type;
    std::uint8_t Attributes;
    std::uint8_t Size[3];
    std::uint8_t State;
} EFI_FFS_FILE_HEADER;

typedef struct {
    std::uint8_t size[3];
    std::uint8_t type;
} EFI_COMMON_SECTION_HEADER;

typedef struct {
    EFI_FFS_FILE_HEADER header;
    std::vector<EFI_COMMON_SECTION_HEADER> sectionHeaders;
} BIOS_FILE;

typedef struct {
    EFI_FIRMWARE_VOLUME_HEADER volumeHeader;
    std::vector<BIOS_FILE> files;
} EFI_VOLUME;


struct BiosRegion : public Region {
    BiosRegion()  {
        name = "Bios";
    }
    std::vector<EFI_VOLUME> volumes;
};

#endif //UEFI_IMAGE_PARSER_REGION_H