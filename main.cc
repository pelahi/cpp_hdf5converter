#include <getopt.h>
#include "Converter.h"


bool getOptions(int argc, char** argv, std::string& inputFileName, std::string& outputFileName, Flags& flags) {
    extern int optind;
    extern char *optarg;
    
    int opt;
    bool err(false);
    std::string usage = "Usage: hdf_convert [-o output_filename] [-s] [-q] input_filename\n\nConvert a FITS file to an HDF5 file with the IDIA schema\n\nOptions:\n\n-o\tOutput filename\n-s\tUse slower but less memory-intensive method (enable if memory allocation fails)\n-c\tUse a chunked dataset (will be turned on automatically if -s is used)\n-m\tEnable mipmaps\n-q\tSuppress all non-error output";
    
    while ((opt = getopt(argc, argv, ":o:scmq")) != -1) {
        switch (opt) {
            case 'o':
                outputFileName.assign(optarg);
                break;
            case 's':
                flags.slow = true;
                break;
            case 'c':
                flags.chunked = true;
                break;
            case 'm':
                flags.mipmaps = true;
                break;
            case 'q':
                std::cout.rdbuf(nullptr);
                break;
            case ':':
                err = true;
                std::cerr << "Missing argument for option " << opt << "." << std::endl;
                break;
            case '?':
                err = true;
                std::cerr << "Unknown option " << opt << "." << std::endl;
                break;
        }
    }
    
    if (optind >= argc) {
        err = true;
        std::cerr << "Missing input filename parameter." << std::endl;
    } else {
        inputFileName.assign(argv[optind]);
        optind++;
    }
    
    if (argc > optind) {
        err = true;
        std::cerr << "Unexpected additional parameters." << std::endl;
    }
        
    if (err) {
        std::cerr << usage << std::endl;
        return false;
    }
    
    if (outputFileName.empty()) {
        auto fitsIndex = inputFileName.find_last_of(".fits");
        if (fitsIndex != std::string::npos) {
            outputFileName = inputFileName.substr(0, fitsIndex - 4);
            outputFileName += ".hdf5";
        } else {
            outputFileName = inputFileName + ".hdf5";
        }
    }
    
    if (flags.slow) {
        flags.chunked = true;
    }
    
    return true;
}

int main(int argc, char** argv) {
    std::string inputFileName;
    std::string outputFileName;
    Flags flags;
    
    if (!getOptions(argc, argv, inputFileName, outputFileName, flags)) {
        return 1;
    }
    
    std::unique_ptr<Converter> converter;
        
    try {
        converter = Converter::getConverter(inputFileName, outputFileName, flags);
    
        std::cout << "Converting FITS file " << inputFileName << " to HDF5 file " << outputFileName << (flags.slow ? " using slower, memory-efficient method" : "") << ". Dataset will " << (flags.chunked ? "" : "not ") << "be chunked. Mipmaps will " << (flags.mipmaps ? "" : "not ") << "be generated." << std::endl;

        converter->convert();
    } catch (const char* msg) {
        std::cerr << "Error: " << msg << ". Aborting." << std::endl;
        return 1;
    }

    return 0;
}
