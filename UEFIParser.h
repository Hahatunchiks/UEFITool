#ifndef UEFI_IMAGE_PARSER_UEFIPARSER_H
#define UEFI_IMAGE_PARSER_UEFIPARSER_H

#include <iostream>
#include <fstream>
#include "UEFIImage.h"

class UEFIParser {

public:
    explicit UEFIParser(const std::string &fName) : fileName(fName) {
        img = new UEFIImage;
    }

    void processBiosFile(std::ifstream &image, BIOS_FILE &file, bool isFirst) const;
    void processBiosFV(std::ifstream &image, BiosRegion &biosRegion, bool isFirst) const;
    std::uint32_t calculateSize(std::uint8_t *size_) const;
    void parse() const;

    UEFIImage *getImage() {
        return img;
    }

private:
    UEFIImage *img;
    std::string fileName;
};

#endif //UEFI_IMAGE_PARSER_UEFIPARSER_H