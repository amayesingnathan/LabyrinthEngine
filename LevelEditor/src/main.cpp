//LevelEditor Tool for use with the Labyrinth engine

#include "BMP.h"
#include <string>

struct colour
{
    enum class RGB {green = 0, red = 1, blue = 2, none = -1};

    unsigned char bgr[3];
     
    colour() : bgr{ 0 } {}
    colour(unsigned char b, unsigned char g, unsigned char r) : bgr{ b, g, r } {}
    colour(const colour& other)
    {
        for (int i = 0; i < 3; i++)
        {
            bgr[i] = other.bgr[i];
        }
    }

    bool isEqual(const colour& other)
    {
        for (int i = 0; i < 3; i++)
        {
            if (bgr[i] != other.bgr[i])
            {
                return false;
            }
        }
        return true;
    }


    RGB getColour()
    {
        if (isEqual({ 255, 0, 0 }))
        {
            return RGB::blue;
        }
        if (isEqual({ 0, 255, 0 }))
        {
            return RGB::green;
        }
        if (isEqual({ 0, 0, 255 }))
        {
            return RGB::red;
        }
        return RGB::none;
    }
};


int main()
{

	std::string inFile;
	std::cout << "Enter the name of the file to convert." << std::endl;
	std::cin >> inFile;

    std::string outFile;
    std::cout << "Enter the name of the new file created." << std::endl;
    std::cin >> outFile;

    inFile = "C:\\Users\\natha\\Desktop\\Code\\WIP\\LabyrinthEngine\\LevelEditor\\io\\in\\" + inFile + ".bmp";
    outFile = "C:\\Users\\natha\\Desktop\\Code\\WIP\\LabyrinthEngine\\LevelEditor\\io\\out\\" + outFile + ".lvl";

	BMP level(inFile.c_str());
    if ((level.bmp_info_header.width != 50) || (level.bmp_info_header.height != 40))
    {
        std::cout << "Invalid bmp dimensions" << std::endl;
    }
    else
    {
        colour::RGB map[50][40];
        for (int x = 0; x < 50; x++)
        {
            for (int y = 0; y < 40; y++)
            {
                map[x][y] = colour::RGB::none;
            }
        }

        colour pixelCol;
        uint32_t channels = level.bmp_info_header.bit_count / 8;
        for (uint32_t y = 0; y < 40; ++y) {
            for (uint32_t x = 0; x < 50; ++x) {
                for (uint32_t i = 0; i < 3; ++i)
                {
                    pixelCol.bgr[i] = level.data[channels * (y * level.bmp_info_header.width + x) + i];
                }

                map[x][y] = pixelCol.getColour();

            }
        }

        std::ofstream output(outFile, std::ios::trunc);
        if (!output)
        {
            std::cout << "Could not open/create output file." << std::endl;
        }
        else
        {
            for (uint32_t y = 0; y < 40; ++y) {
                for (uint32_t x = 0; x < 50; ++x) {
                    output << static_cast<int>(map[x][y]) << " ";
                }
                output << "\n";
            }
            output.close();
        }
    }
}