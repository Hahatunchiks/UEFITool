#ifndef UEFI_IMAGE_PARSER_JSONPARSER_H
#define UEFI_IMAGE_PARSER_JSONPARSER_H

#include "../UEFIImage.h"
#include <fstream>

class JSONParser {

public:
    static void write(UEFIImage &img, const std::string &fileName);
};

#endif