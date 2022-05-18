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

    void parse() const;

    UEFIImage *getImage() {
        return img;
    }

private:
    UEFIImage *img;
    std::string fileName;
};

#endif //UEFI_IMAGE_PARSER_UEFIPARSER_H