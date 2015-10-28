#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <FreeImage.h>
#include <unistd.h>

using namespace std;

#define LOG_MESSAGE(...) {printf(__VA_ARGS__); printf("\n");}
#define LOG_ERROR(...) {printf("ERROR: "); printf(__VA_ARGS__); printf("\n");}

enum YUVFormat
{
    YUV_YUV,
    YUV_UYVY,
    YUV_YUYV,
    YUV_YYUV
};

enum YUVScale
{
    SCALE_H2V2,
    SCALE_H2V1,
    SCALE_H1V2,
    SCALE_H1V1
};

class Config
{
  public:
    bool appendMode;            /* if not zero append YUV image(s) to output file */
    bool uvInterleave;          /* if not zero, UV rows in planar images are interleaved*/
    bool uvOrderSwap;               /**/
    uint32_t yuvFormat;         /* YUV output mode. Default: h2v2 */
    uint32_t uvScale;           /* Defines how UV components are scaled in planar mode */
    unsigned long seqStart;         /* Sequence start for multiple files */
    unsigned long seqEnd;           /* Sequence end for multiple files */
    string inFileNamePattern;
    string outFileNamePattern;
    vector<float>yuvMatrix;

    Config();

    // Returns "false" on errors and "true" if no errors
    bool ParseArgs(char* args[], int count);
    void LoadJPEGMatrix();
    void LoadSDTVMatrix();
    void LoadHDTVMatrix();

  private:
    bool ParseSequenceRange(string range);
};

void PrintHelp();
string ExpandPattern(string pattern, unsigned long counter);

int main(int argc, char* argv[])
{
    Config cfg;
    string inFileName, outFileName;
    FILE* hOutFile = 0;
    uint8_t errorFlag = 1; // By default we will exiting with error
    FIBITMAP *inImage = 0;
    uint32_t bpp = 0;
    uint32_t lumaWidth, lumaHeight;
    uint32_t chromaWidth, chromaHeight;
    uint32_t x, y, xMask, yMask;
    uint8_t Rc, Gc, Bc;
    uint8_t *rgbPixels;
    uint8_t *yPixels, *uPixels, *vPixels;
    uint8_t *yPtr, *uPtr, *vPtr;
    bool warned = false;

    if(!cfg.ParseArgs(argv, argc))
        exit(1);

    FreeImage_Initialise();

    LOG_MESSAGE("Processing:");

    /* First expanding of output filename. If append mode - this is only one place
       where name is expanded */
    outFileName = ExpandPattern(cfg.outFileNamePattern, cfg.seqStart);

    // For performance reasons get matrix values here to put them on stack
    // instead of accessing them in deep loops from vector
    float yr = cfg.yuvMatrix[0], yg = cfg.yuvMatrix[1], yb = cfg.yuvMatrix[2];
    float ur = cfg.yuvMatrix[3], ug = cfg.yuvMatrix[4], ub = cfg.yuvMatrix[5];
    float vr = cfg.yuvMatrix[6], vg = cfg.yuvMatrix[7], vb = cfg.yuvMatrix[8];

    /* Main loop of passing through all images */
    while(cfg.seqStart <= cfg.seqEnd)
    {
        inFileName = ExpandPattern(cfg.inFileNamePattern, cfg.seqStart);

        FREE_IMAGE_FORMAT format = FreeImage_GetFileType(inFileName.c_str());
        if(format == FIF_UNKNOWN)
        {
            LOG_ERROR("Input image format is unknown or unsupported...");
            goto HandleError;
        }

        inImage = FreeImage_Load(format, inFileName.c_str());

        if(inImage == 0)
        {
            LOG_ERROR("Some problem occurred with reading of input image...");
            goto HandleError;
        }

        if( !cfg.appendMode )
            outFileName = ExpandPattern(cfg.outFileNamePattern, cfg.seqStart);

        hOutFile = fopen( outFileName.c_str(), cfg.appendMode ? "ab" : "wb" );

        if(hOutFile == 0)
        {
            LOG_ERROR("Can not open output file...");
            goto HandleError;
        }

        bpp = FreeImage_GetBPP(inImage);
        lumaWidth = FreeImage_GetWidth(inImage);
        lumaHeight = FreeImage_GetHeight(inImage);

        LOG_MESSAGE("\t%s [%dx%d %ubpp]", inFileName.c_str(), lumaWidth, lumaHeight, bpp);

        if( (lumaWidth & 1) && cfg.yuvFormat != YUV_YUV)
        {
            LOG_ERROR("Width of source image is odd - this is incompatible with packed format...");
            goto HandleError;
        }

        if( !warned && ((lumaWidth & 1) || (lumaHeight & 1)) && (cfg.yuvFormat == YUV_YUV || cfg.yuvFormat == YUV_YYUV))
        {
            LOG_MESSAGE("Warning! Dimensions of the source image are odd. This may cause incompatibility with some YUV viewers.");
            warned = true; // Show warning only once
        }

        if (bpp != 24)
        {
            FIBITMAP *newBitmap = FreeImage_ConvertTo24Bits(inImage);

            if (newBitmap == NULL)
            {
                LOG_ERROR("Failed to convert image to 24bpp");
                goto HandleError;
            }

            FreeImage_Unload(inImage);
            inImage = newBitmap;
        }

        FreeImage_FlipVertical(inImage);

        /* Calculate dimensions of destination UV components */
        switch(cfg.uvScale)
        {
            default: /* Default scale h1v1 */
            case SCALE_H1V1:
                chromaWidth = lumaWidth;
                chromaHeight = lumaHeight;
                xMask = 0;
                yMask = 0;
                break;
            case SCALE_H2V2:
                chromaWidth = lumaWidth / 2;
                chromaHeight = lumaHeight / 2;
                yMask = xMask = 1;
                break;
            case SCALE_H1V2:
                chromaWidth = lumaWidth;
                chromaHeight = lumaHeight / 2;
                xMask = 0;
                yMask = 1;
                break;
            case SCALE_H2V1:
                chromaWidth = lumaWidth / 2;
                chromaHeight = lumaHeight;
                xMask = 1;
                yMask = 0;
                break;
        }

        // Pointers that are always pointing on buffers
        yPixels = new uint8_t[lumaHeight * lumaWidth];
        uPixels = new uint8_t[chromaHeight * chromaWidth];
        vPixels = new uint8_t[chromaHeight * chromaWidth];

        // Pointers we are working with
        yPtr = yPixels;
        uPtr = uPixels;
        vPtr = vPixels;

        /* Main converting cycle */
        for(y = 0; y < lumaHeight; y++)
        {
            rgbPixels = FreeImage_GetScanLine(inImage, y);

            for(x = 0; x < lumaWidth; x++)
            {
                Rc = *rgbPixels++;
                Gc = *rgbPixels++;
                Bc = *rgbPixels++;

                *yPtr++ = uint8_t(Rc * yr + Gc * yg + Bc * yb);
                if((y & yMask) == 0 && (x & xMask) == 0 && (y / 2) < chromaHeight && (x / 2) < chromaWidth)
                {
                    *uPtr++ = uint8_t(Rc * ur + Gc * ug + Bc * ub + 128);
                    *vPtr++ = uint8_t(Rc * vr + Gc * vg + Bc * vb + 128);
                }
            }
        }

        if(cfg.uvOrderSwap)
        {   // UV components should be swapped, so just swap pointers
            cout << "Swapping UV order" << endl;
            uint8_t* tmp = uPixels;
            uPixels = vPixels;
            vPixels = tmp;
        }

        yPtr = yPixels;
        uPtr = uPixels;
        vPtr = vPixels;

        if(cfg.yuvFormat == YUV_YUV)
        {   // Writing planar image
            fwrite(yPixels, 1, lumaWidth * lumaHeight, hOutFile);

            if(cfg.uvInterleave)
            {   // U and V rows should be interleaved after each other
                while(chromaHeight--)
                {
                    fwrite(uPtr, 1, chromaWidth, hOutFile); // Write U line
                    fwrite(vPtr, 1, chromaWidth, hOutFile); // Write V line
                    uPtr += chromaWidth;
                    vPtr += chromaWidth;
                }
            }else{
                // Simply write U and V planes
                fwrite(uPixels, 1, chromaWidth * chromaHeight, hOutFile);
                fwrite(vPixels, 1, chromaWidth * chromaHeight, hOutFile);
            }
        }else if(cfg.yuvFormat == YUV_YYUV)
        {   // Writing planar image
            fwrite(yPixels, 1, lumaWidth * lumaHeight, hOutFile);

            // U and V columns should be interleaved after each other
            for(uint32_t row = 0; row < chromaHeight; row++)
            {
                for(uint32_t col = 0; col < chromaWidth; col++)
                {   // Write in following order Y, U, Y, V
                    fwrite(uPtr++, 1, 1, hOutFile);
                    fwrite(vPtr++, 1, 1, hOutFile);
                }
            }
        }else{
            // Writing packed image
            if(cfg.yuvFormat == YUV_YUYV)
            {
                for(uint32_t row = 0; row < lumaHeight; row++)
                {
                    for(uint32_t col = 0; col < lumaWidth; col += 2)
                    {   // Write in following order Y, U, Y, V
                        fwrite(yPtr++, 1, 1, hOutFile);
                        fwrite(uPtr++, 1, 1, hOutFile);
                        fwrite(yPtr++, 1, 1, hOutFile);
                        fwrite(vPtr++, 1, 1, hOutFile);
                    }
                }
            }else{
                for(uint32_t row = 0; row < lumaHeight; row++)
                {
                    for(uint32_t col = 0; col < lumaWidth; col += 2)
                    {   // Write in following order U, Y, V, Y
                        fwrite(uPtr++, 1, 1, hOutFile);
                        fwrite(yPtr++, 1, 1, hOutFile);
                        fwrite(vPtr++, 1, 1, hOutFile);
                        fwrite(yPtr++, 1, 1, hOutFile);
                    }
                }
            }
        }

        FreeImage_Unload(inImage);
        inImage = 0;

        fclose(hOutFile);
        hOutFile = 0;

        delete[] yPixels;
        delete[] uPixels;
        delete[] vPixels;

        cfg.seqStart++;
    }

    LOG_MESSAGE("Done!");

    errorFlag = 0; // We successful passed all stages, so set flag to zero which means - OK

HandleError:
    if(inImage)
        FreeImage_Unload(inImage);

    if(hOutFile)
        fclose(hOutFile);

    FreeImage_DeInitialise();

    return errorFlag;
}

string toString(unsigned long value)
{
    ostringstream oss;
    oss << value;
    return oss.str();
}

/*
  Description: Scan for '#' symbols and replaces them with integer value (and leading zeros if needed)
  Returns: Formed string
*/

string ExpandPattern(string pattern, unsigned long counter)
{
    string result;
    string::iterator it = pattern.begin();
    uint32_t cntr = 0;
    string counterStr = toString(counter);

    // Copy from input pattern to resulting string until we meet '#' sign
    while( it != pattern.end() && *it != '#') result += *it++;

    // Calculate number of '#' signs in input pattern
    while( it != pattern.end() && *it == '#')
    {
        it++;
        cntr++;
    }

    // If we not found any patterns - just return unmodified input pattern
    if( !cntr )
        return pattern;

    // Determine leading zeros
    if(cntr > counterStr.length())
    {
        // We have leading zeros
        cntr -= counterStr.length();
    }else{
        // Resulting counter bigger than pattern has defined,
        // therefore there are no leading zeros
        cntr = 0;
    }
    // cntr from now contains number of leading zeros

    // Append to result leading zeros
    if(cntr)
        result.append(cntr, '0');

    // Append integer value
    result.append(counterStr);

    // Copy from input pattern to resulting string rest of input pattern
    while( it != pattern.end() ) result += *it++;

    return result;
}

Config::Config()
{
    appendMode = false; /* if not zero append YUV image(s) to output file */
    uvOrderSwap = false; /* no UV order swap */
    uvInterleave = false; /* no UV interleaving */
    yuvFormat = YUV_YUV;    /* YUV output mode. Default: h2v2 */
    uvScale = SCALE_H1V1;   /* UV scaling for planar mode. Default: h1v1 */
    seqStart = 0;   /* Sequence start for multiple files */
    seqEnd = 0;     /* Sequence end for multiple files */
    LoadJPEGMatrix();
}

void Config::LoadJPEGMatrix()
{
    yuvMatrix = { 0.299,     0.587,     0.114,
                  -0.168736, -0.331264,  0.5,
                  0.5,      -0.418688, -0.081312};
}

void Config::LoadSDTVMatrix()
{
    yuvMatrix = { 0.299,    0.587,    0.114,
                  -0.14713, -0.28886,  0.436,
                  0.615,   -0.51499, -0.10001};
}

void Config::LoadHDTVMatrix()
{
    yuvMatrix = { 0.2126,   0.7152,   0.0722,
                  -0.09991, -0.33609,  0.436,
                  0.615,   -0.55861, -0.05639};
}

bool Config::ParseArgs(char* args[], int count)
{
    string seqRangeOption;
    string yuvFormatOption;
    string uvScaleOption;
    string matrixOption;
    bool error = true;

    while(true) {
        int c = getopt(count, args, "haiwm:f:s:o:x:");

        if (c == -1)
            break;

        switch (c) {
            case 'h':
                goto HandleError;
            case 'a':
                appendMode = true;
                break;
            case 'i':
                uvInterleave = true;
                break;
            case 'w':
                uvOrderSwap = true;
                break;
            case 'm':
                seqRangeOption = optarg;
                break;
            case 'f':
                yuvFormatOption = optarg;
                break;
            case 's':
                uvScaleOption = optarg;
                break;
            case 'o':
                outFileNamePattern = optarg;
                break;
            case 'x':
                matrixOption = optarg;
                break;
            default:
                cerr << "Unknown option: '" << (char)optopt << "'" << endl;
                break;
        };
    }

    if (optind != count)
    {
        inFileNamePattern = args[optind];
    } else {
        cerr << "Input file not specified" << endl;
        goto HandleError;
    }

    if (outFileNamePattern.length() == 0)
        outFileNamePattern = inFileNamePattern + ".yuv";

    // Scaling could be overridden by format, then select scale first
    if( uvScaleOption == "h2v2")
        uvScale = SCALE_H2V2;
    else if(uvScaleOption == "h1v1")
        uvScale = SCALE_H1V1;
    else if(uvScaleOption == "h1v2")
        uvScale = SCALE_H1V2;
    else if(uvScaleOption == "h2v1")
        uvScale = SCALE_H2V1;
    else if( !uvScaleOption.empty())
    {
        LOG_ERROR("Unknown UV scaling...");
        goto HandleError;
    }

    if( yuvFormatOption == "yuv")
        yuvFormat = YUV_YUV;
    else if(yuvFormatOption == "yyuv")
        yuvFormat = YUV_YYUV;
    else if(yuvFormatOption == "yuyv")
        yuvFormat = YUV_YUYV, uvScale = SCALE_H2V1; // Packed format always h2v1
    else if(yuvFormatOption == "uyvy")
        yuvFormat = YUV_UYVY, uvScale = SCALE_H2V1; // Packed format always h2v1
    else if( !yuvFormatOption.empty())
    {
        LOG_ERROR("Unknown YUV format...");
        goto HandleError;
    }

    if(!seqRangeOption.empty())
        if(!ParseSequenceRange(seqRangeOption))
        {
            LOG_ERROR("You've specified bad sequence range...");
            goto HandleError;
        }

    if (!matrixOption.empty())
    {
        if (matrixOption == "jpeg")
            LoadJPEGMatrix();
        else if (matrixOption == "sdtv")
            LoadSDTVMatrix();
        else if (matrixOption == "hdtv")
            LoadHDTVMatrix();
        else {
            LOG_ERROR("Unknown matrix option '%s'", matrixOption.c_str());
            goto HandleError;
        }
    }

    error = false;
HandleError:

    return !error;
}

bool Config::ParseSequenceRange(string range)
{
    string::iterator it = range.begin();
    string seqStartOpt;
    string seqEndOpt;
    char *end = 0;

    // Copy from input to seqStartOpt until we will find ':' character
    while(it != range.end() && *it != ':')
        seqStartOpt += *it++;

    if(it == range.end() || *it++ != ':')
        return false;

    // Copy from input to seqEndOpt till the end
    while(it != range.end())
        seqEndOpt += *it++;

    if(seqStartOpt.empty() || seqEndOpt.empty())
        return false;

    seqStart = strtoul(seqStartOpt.c_str(), &end, 10);
    seqEnd = strtoul(seqEndOpt.c_str(), &end, 10);

    return true;
}

void PrintHelp()
{
    cout <<
            "\nUsage: yuvit [options] <file>\n\n"
            "Options:\n"
            "    -h\n"
            "        This help\n"
            "    -o\n"
            "        Output filename. Could be a pattern for read multifile sequences.\n"
            "    -a\n"
            "       Append mode. Images will be append to output file. Doesn't truncate output file.\n"
            "    -m <start>:<end>\n"
            "       Start and end numbers for multifile sequences.\n"
            "    -i\n"
            "       Interleave UV rows for planar formats\n"
            "    -w\n"
            "       Swap UV components order\n"
            "    -x <jpeg|sdtv|hdtv>\n"
            "       Use YUV conversion matrix. Default: jpeg\n"
            "            jpeg\n"
            "                JFIF specification matrix:\n"
            "                |Y|   | 0.299     0.587     0.114|   |R|\n"
            "                |U| = |-0.168736 -0.331264  0.5  | x |G|\n"
            "                |V|   | 0.5      -0.418688 -0.081|   |B|\n"
            "            sdtv\n"
            "                ITU-R BT.601 for SDTV specification matrix:\n"
            "                |Y|   | 0.299    0.587    0.114  |   |R|\n"
            "                |U| = |-0.14713 -0.28886  0.436  | x |G|\n"
            "                |V|   | 0.615   -0.51499 -0.10001|   |B|\n"
            "            hdtv\n"
            "                ITU-R BT.709 for HDTV specification matrix:\n"
            "                |Y|   | 0.2126   0.7152   0.0722 |   |R|\n"
            "                |U| = |-0.09991 -0.33609  0.436  | x |G|\n"
            "                |V|   | 0.615   -0.55861 -0.05639|   |B|\n"

            "    -f <yuv|yuyv|uyuv|yyuv>\n"
            "        Output YUV format. Default: yuv"
            "            yuv\n"
            "                Planar format\n"
            "            yuyv\n"
            "                Packed format\n"
            "            uyvy\n"
            "                Packed format\n"
            "            yyuv\n"
            "                Planar packed chroma format\n"
            "    -s <h1v1|h2v2|h2v1|h1v2>\n"
            "        Chroma scaling. Used only for planar formats. Default: h1v1\n"
            "            h1v1\n"
            "                UV not scaled down [DEFAULT]\n"
            "            h2v2\n"
            "                UV scaled down by 2x horizontally and vertically\n"
            "            h2v1\n"
            "                UV scaled down by 2x horizontally\n"
            "            h1v2\n"
            "                UV scaled down by 2x vertically\n"
            "\nMultifile sequences:\n"
            "    Use '#' in file names, so they will be replaced with numbers.\n"
            "    Examples:\n"
            "        yuvit -a -m 0:100 -o out.yuv test###.bmp\n"
            "            Converts: 'test000.bmp'...'test100.bmp' -> 'out.yuv'\n"
            "        yuvit -m 10:200 -o out###.yuv test######.jpg\n"
            "            Converts: 'test000010.jpg'...'test000200.jpg' -> 'out010.yuv'...'out200.yuv'\n"
            "\nYUV converter v" PROJECT_VERSION "\n"
            "by " PROJECT_AUTHOR "\n"
            "License: " PROJECT_LICENSE "\n"
            "URL: " PROJECT_URL "\n"
            ;
}
