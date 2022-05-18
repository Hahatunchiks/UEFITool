#ifndef UEFI_IMAGE_PARSER_UEFIIMAGE_H
#define UEFI_IMAGE_PARSER_UEFIIMAGE_H
#include "Region.h"

class UEFIImage {
public:
    UEFIImage() : size(0) {
    }

    std::uint32_t getImageSize() const {
        return size;
    }

    void setImageSize(std::uint32_t size_) {
        size = size_;
    }
    void setFlashDescriptorRegion(const FlashDescriptorRegion &src) {
        flashDescriptorRegion = src;
    }

    FlashDescriptorRegion getFlashDescriptorRegion() const {
        return flashDescriptorRegion;
    }

    void setBiosRegion(const BiosRegion &src) {
        biosRegion = src;
    }

    BiosRegion getBiosRegion() {
        return biosRegion;
    }
private:
    FlashDescriptorRegion flashDescriptorRegion;
    BiosRegion biosRegion;
    std::uint32_t size;

};

#endif //UEFI_IMAGE_PARSER_UEFIIMAGE_H