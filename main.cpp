#include "UEFIParser.h"

ulong calculateSize(std::uint8_t *size_) {
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
int main(int argc, const char **argv) {
    if (argc != 2) {
        std::cerr << "Write filename in params\n";
        exit(-1);
    }
    std::string fName = argv[1];
    UEFIParser parser(fName);
    parser.parse();
    BiosRegion bios = parser.getImage()->getBiosRegion();
    int c = 0;
    std::cout << std::hex << "Bios start: " << bios.address << std::endl;
    std::cout << std::hex << "Bios end: " << bios.size << std::endl;
    std::cout << "Size volumes: " << bios.volumes.size() << std::endl;
    for (const auto &volume: bios.volumes) {
        std::cout << std::hex << "fvLength: " << volume.volumeHeader.fvLength << std::endl;
        std::cout << "Count files: " << volume.files.size() << std::endl;
        for (const auto &file: volume.files) {
            std::cout << "Filename: ";
            for(int i = 0; i < 16; i++) {
                std::cout << std::hex << (int)file.header.Name[i];
            }
            std::cout << std::endl;
            std::cout << std::hex << "File size: " << calculateSize((uint8_t *)file.header.Size) << std::endl;
        }
    }
    return 0;
}