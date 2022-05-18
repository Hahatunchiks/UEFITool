#include "UEFIParser.h"
#include <bitset>

static const std::uint32_t signature = 0x4856465F;

typedef struct {
    std::uint8_t resetVector[16];
    std::uint32_t signature;
    std::uint8_t descriptorMAP[16];
} EFI_FLASH_DESCRIPTOR;


typedef struct {
    std::uint8_t GUID[16];
    std::uint32_t headerSie;
    std::uint32_t flags;
    std::uint32_t capsuleImageSize;
    std::uint16_t FWImageOffset;
    std::uint16_t OEMHeaderOffset;
} EFI_CAPSULE_HEADER;

static void CalculateFlashSectionsBases(const std::uint8_t offset, std::uint32_t &dest) {
    std::bitset<8> offsetAddress(offset);
    std::bitset<32> address(0);
    for (int i = 0; i < 8; i++) {
        address[i + 4] = offsetAddress[i];
    }
    dest = address.to_ulong();

}

static void setFlashRegionSections(const EFI_FLASH_DESCRIPTOR &descriptor, FlashDescriptorRegion &region,
                                   const std::uint32_t offset) {
    CalculateFlashSectionsBases(descriptor.descriptorMAP[0], region.componentSectionBase);
    region.componentSectionBase += offset;
    CalculateFlashSectionsBases(descriptor.descriptorMAP[2], region.regionSectionBase);
    region.regionSectionBase += offset;
    CalculateFlashSectionsBases(descriptor.descriptorMAP[4], region.masterSectionBase);
    region.masterSectionBase += offset;
}


static void CalculateRegionBase(std::uint32_t offset, Region &reg, std::uint32_t FWOffset) {
    std::bitset<32> regionValues(offset);
    std::bitset<32> regionBase(0);
    std::bitset<32> regionSize(0);

    for (int i = 0; i <= 12; i++) {
        regionBase[i + 12] = regionValues[i];
    }
    reg.address = regionBase.to_ulong() + FWOffset;

    for (int i = 16; i <= 28; i++) {
        regionSize[i - 4] = regionValues[i];
    }
    reg.size = regionSize.to_ulong() + FWOffset;
}


static std::uint32_t findNextVolumeFV(std::ifstream &img) {
    std::uint32_t offset = 0;
    std::uint32_t word = -1;
    while (true) {
        img.read((char *) (&word), sizeof(word));
        if (img.eof() || img.fail()) {
            break;
        }
        if (word == signature) {
            return offset;
        }
        offset += sizeof(word);
    }
    return 0;
}

static void processBiosFV(std::ifstream &image, BiosRegion &biosRegion, bool isFirst = false) {
    EFI_FIRMWARE_VOLUME_HEADER FVHeader0;
    EFI_FV_BLOCK_MAP map;

    if (!isFirst) {
        image.seekg(-44, std::ios_base::cur);
    }
    image.read((char *) (&FVHeader0), sizeof(FVHeader0));

    while (true) {
        image.read((char *) &map, sizeof(map));
        if (map.numBlocks == 0 && map.length == 0) break;
    }

    BIOS_FILE file;
    std::vector<BIOS_FILE> files;
    std::uint32_t readedBytes = 0;
    while (readedBytes < FVHeader0.fvLength - FVHeader0.headerLength) {
        image.read((char *) (&file.header), sizeof(file.header));
        if (file.header.State == 0xFF) {
            break;
        }

        std::bitset<8> sizeLeft(file.header.Size[0]);
        std::bitset<8> sizeMid(file.header.Size[1]);
        std::bitset<8> sizeRight(file.header.Size[2]);
        std::bitset<24> size(0);
        for (int i = 0; i < 8; i++) {
            size[i] = sizeLeft[i];
            size[i + 8] = sizeMid[i];
            size[i + 16] = sizeRight[i];
        }
        files.push_back(file);
        readedBytes += size.to_ulong();
        image.seekg(size.to_ulong() - sizeof(file.header), std::ios_base::cur);
        //FFS alignment
        if (image.tellg() % 16 <= 8) {
            image.seekg(((std::uint32_t) (8 - (image.tellg())) % 8), std::ios_base::cur);
            readedBytes += image.tellg() % 8;
        } else {
            image.seekg((std::uint32_t) ((16 - image.tellg())) % 8, std::ios_base::cur);
            readedBytes += (16 - image.tellg()) % 8;
        }
    }

    EFI_VOLUME volume = {.volumeHeader = FVHeader0, .files = files};
    biosRegion.volumes.push_back(volume);
}

void UEFIParser::parse() const {
    std::ifstream image(fileName,
                        std::ios_base::in | std::ios_base::binary);
    if (image.is_open()) {
        long offset = 0;
        EFI_FLASH_DESCRIPTOR descriptor;
        EFI_CAPSULE_HEADER capsuleHeader;
        FlashDescriptorRegion flashDescriptorRegion;

        std::uint8_t firstLine[16];
        image.read((char *) (&firstLine), sizeof(firstLine));
        bool isHasNotCapsuleHeader = true;
        for (unsigned char &i: firstLine) {
            if (i != 0xFF) {
                isHasNotCapsuleHeader = false;
            }
        }
        image.seekg(0, std::ios_base::beg);

        if (!isHasNotCapsuleHeader) {
            image.read((char *) (&capsuleHeader), sizeof(capsuleHeader));
            offset += capsuleHeader.FWImageOffset;
            flashDescriptorRegion.address = 0;
            image.seekg(offset);
        } else {
            flashDescriptorRegion.address = capsuleHeader.FWImageOffset;
        }
        image.read((char *) (&descriptor), sizeof(descriptor));

        //Flash Descriptor Region init
        setFlashRegionSections(descriptor, flashDescriptorRegion, offset);
        offset = flashDescriptorRegion.regionSectionBase;
        image.seekg(offset, std::ios_base::beg);
        image.read((char *) (&flashDescriptorRegion.regionAddresses), sizeof(flashDescriptorRegion.regionAddresses));
        img->setFlashDescriptorRegion(flashDescriptorRegion);



        // Bios Region init
        BiosRegion biosRegion;
        CalculateRegionBase(flashDescriptorRegion.regionAddresses[1], biosRegion, flashDescriptorRegion.address);
        offset = biosRegion.address;
        image.seekg(offset, std::ios_base::beg);

        //Bios FV process
        processBiosFV(image, biosRegion, true);
        std::uint32_t nextFvBeg = findNextVolumeFV(image);
        for (int i = 0; nextFvBeg != 0; i++) {
            processBiosFV(image, biosRegion);
            nextFvBeg = findNextVolumeFV(image);
        }
        img->setBiosRegion(biosRegion);
    } else {
        std::cerr << "No such file\n";
        exit(-1);
    }

    image.close();
}