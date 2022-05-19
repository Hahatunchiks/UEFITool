#include "JSONParser.h"

const char LEFT_BRACKET = '{';
const char RIGHT_BRACKET = '}';
const int GUIDLength = 16;
const std::string CAPSULE_HEADER = "\"CapsuleHeader\":{";

static std::uint32_t calculateSize(std::uint8_t *size_) {
    std::bitset<8> sizeLeft(size_[0]);
    std::bitset<8> sizeMid(size_[1]);
    std::bitset<8> sizeRight(size_[2]);
    std::bitset<24> size(0);
    for (int i = 0; i < 8; i++) {
        size[i] = sizeLeft[i];
        size[i + 8] = sizeMid[i];
        size[i + 16] = sizeRight[i];
    }

    return size.to_ulong();
}

static void writeField(const std::string &filedName, const std::uint32_t field, std::ofstream &output, int offset,
                       bool noComma = false) {

    if (!noComma) {
        output << std::string(offset, '\t') << filedName << ":" << "\"" << field << "\",\n";
    } else {
        output << std::string(offset, '\t') << filedName << ":" << "\"" << field << "\"\n";
    }
}

static void writeGUID(const std::uint8_t Name[], std::ofstream &output, int offset) {
    output << std::string(offset+1, '\t') << "\"GUID\"" << ":\"";
    for (int i = 0; i < GUIDLength; i++) {
        output << std::hex << (int) Name[i];
    }
    output << "\",\n";
}

static void writeSection(const EFI_COMMON_SECTION_HEADER header, std::ofstream &output, int offset) {
    output << std::string(offset, '\t') << "\"FileSection\":" << LEFT_BRACKET << "\n";
    writeField("\"Size(hex)\"", calculateSize((uint8_t *) header.size), output, offset + 1);
    writeField("\"Type(hex)\"", header.type, output, offset + 1, true);
    output << std::string(offset, '\t') << RIGHT_BRACKET << ",\n";
}

static void writeFile(const BIOS_FILE &file, std::ofstream &output, int offset) {
    output << std::string(offset, '\t') << "\"FileHeader\":{\n";
    writeGUID(file.header.Name, output, offset);
   // output << "\":" << LEFT_BRACKET << "\n";
    writeField("\"Size(hex)\"", calculateSize((uint8_t *) file.header.Size), output, offset + 1);
    writeField("\"State(hex)\"", file.header.State, output, offset + 1);
    writeField("\"DataChecksum(hex)\"", file.header.DataChecksum, output, offset + 1);
    writeField("\"HeaderChecksum(hex)\"", file.header.HeaderChecksum, output, offset + 1);
    writeField("\"Type(hex)\"", file.header.Type, output, offset + 1);
    for (const auto &section: file.sectionHeaders) {
        writeSection(section, output, offset + 1);
    }
    output << std::string(offset, '\t') << RIGHT_BRACKET << ",\n";

}

static void writeVolume(const EFI_VOLUME &volume, std::ofstream &output, int offset) {
    output << std::string(offset, '\t') << "\"VolumeHeader\":{\n";
    writeGUID(volume.volumeHeader.fileSystemGuid, output, offset);
  //  output << "\":" << LEFT_BRACKET << "\n";
    writeField("\"Signature(hex)\"", volume.volumeHeader.signature, output, offset + 1);
    writeField("\"FirmwareLength(hex)\"", volume.volumeHeader.fvLength, output, offset + 1);
    writeField("\"HeaderLength (hex)\"", volume.volumeHeader.headerLength, output, offset + 1);
    writeField("\"Checksum(hex)\"", volume.volumeHeader.checksum, output, offset + 1);
    writeField("\"AmountOfFiles(hex)\"", volume.files.size(), output, offset + 1, true);
    output << std::string(offset, '\t') << RIGHT_BRACKET << ",\n";
    for (const auto &file: volume.files) {
        writeFile(file, output, offset);

    }
}

void JSONParser::write(UEFIImage &img, const std::string &fileName) {
    std::ofstream fOutput(fileName);

    if (fOutput.is_open()) {
        int deep = 1;
        fOutput << LEFT_BRACKET << '\n';
        if (img.isCapsuled()) {
            fOutput << "\t" << "\"CapsuleHeader\":{\n";
            auto capsuleHeader = img.getCapsuleHeader();
            writeGUID(capsuleHeader.GUID, fOutput, deep);
          //  fOutput << "\":" << LEFT_BRACKET << "\n";
            writeField("\"ImageSize(hex)\"", capsuleHeader.capsuleImageSize, fOutput, deep+1);
            writeField("\"HeaderSize(hex)\"", capsuleHeader.headerSie, fOutput, deep+1);
            writeField("\"OEMStart(hex)\"", capsuleHeader.OEMHeaderOffset, fOutput, deep+1);
            writeField("\"ImageStart(hex)\"", capsuleHeader.FWImageOffset, fOutput, deep+1, true);
            fOutput << "\t" << RIGHT_BRACKET << ",\n";
        }

        auto descriptorRegion = img.getFlashDescriptorRegion();
        fOutput << std::string(deep, '\t') << "\"" << descriptorRegion.name << "\":{\n";
        deep++;
        writeField("\"regionAddress(hex)\"", descriptorRegion.address, fOutput, deep);
        writeField("\"ComponentSectionBaseAddress (hex)\"", descriptorRegion.componentSectionBase, fOutput, deep);
        writeField("\"RegionSectionBaseAddress (hex)\"", descriptorRegion.regionSectionBase, fOutput, deep);
        writeField("\"MasterSectionBaseAddress(hex)\"", descriptorRegion.masterSectionBase, fOutput, deep, true);
        deep--;
        fOutput << std::string(deep, '\t') << RIGHT_BRACKET << ",\n";

        auto biosRegion = img.getBiosRegion();
        fOutput << std::string(deep, '\t') << "\"" << biosRegion.name << "\":{\n";
        deep++;
        writeField("\"RegionAddress(hex)\"", biosRegion.address, fOutput, deep);
        writeField("\"RegionSize(hex)\"", biosRegion.size, fOutput, deep);
        for (const auto &volume: biosRegion.volumes) {
            writeVolume(volume, fOutput, deep);
        }
        fOutput << "\t" << RIGHT_BRACKET << '\n';


        fOutput << RIGHT_BRACKET;

    } else {
        std::cerr << "Couldn't open/create file" << std::endl;
        exit(-1);
    }
}