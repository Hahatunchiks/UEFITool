#include "UEFIParser.h"
#include "parser/JSONParser.h"


int main(int argc, const char **argv) {
    if (argc != 2) {
        std::cerr << "Write filename in params\n";
        exit(-1);
    }
    std::string fName = argv[1];
    UEFIParser parser(fName);
    parser.parse();
    UEFIImage *image = parser.getImage();
    std::string outFile = fName + ".json";
    JSONParser::write(*image, outFile);
    JSONParser::read(outFile);
    return 0;
}