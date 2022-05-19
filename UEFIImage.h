#ifndef UEFI_IMAGE_PARSER_UEFIIMAGE_H
#define UEFI_IMAGE_PARSER_UEFIIMAGE_H
#include "Region.h"

typedef struct {
    std::uint8_t GUID[16];
    std::uint32_t headerSie;
    std::uint32_t flags;
    std::uint32_t capsuleImageSize;
    std::uint16_t FWImageOffset;
    std::uint16_t OEMHeaderOffset;
} EFI_CAPSULE_HEADER;

class UEFIImage {
public:

    UEFIImage() : isCapsuledFlag(false) {

    }

    EFI_CAPSULE_HEADER getCapsuleHeader() const {
        return capsuleHeader;
    }

    void setCapsuleHeader(const EFI_CAPSULE_HEADER capsuleHeader_) {
        capsuleHeader = capsuleHeader_;
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

    bool isCapsuled() const{
        return isCapsuledFlag;
    }

    void setCapsuledFlag(const bool flag) {
        isCapsuledFlag = flag;
    }

private:
    FlashDescriptorRegion flashDescriptorRegion;
    BiosRegion biosRegion;
    EFI_CAPSULE_HEADER capsuleHeader;
    bool isCapsuledFlag;

};

#endif //UEFI_IMAGE_PARSER_UEFIIMAGE_H