//LevelEditor Tool for use with the Labyrinth engine

#include "BMP.h"
#include <string>

constexpr int MAP_WIDTH = 25;
constexpr int MAP_HEIGHT = 20;

struct colour
{
    enum class RGB {none = -1, green = 0, red = 1, blue = 2 };

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

    std::string bgFile;
    std::cout << "Enter the name of the file to be the background of the display." << std::endl;
    std::cin >> bgFile;

    std::string outFile;
    std::cout << "Enter the name of the new file created." << std::endl;
    std::cin >> outFile;

    inFile = "C:\\Users\\natha\\Desktop\\Code\\WIP\\LabyrinthEngine\\LevelEditor\\io\\in\\" + inFile + ".bmp";
    bgFile = "assets/textures/" + bgFile + ".png";
    outFile = "C:\\Users\\natha\\Desktop\\Code\\WIP\\LabyrinthEngine\\LevelEditor\\io\\out\\" + outFile + ".lvl";

	BMP level(inFile.c_str());
    if ((level.bmp_info_header.width != MAP_WIDTH) || (level.bmp_info_header.height != MAP_HEIGHT))
    {
        std::cout << "Invalid bmp dimensions" << std::endl;
    }
    else
    {
        colour::RGB map[MAP_WIDTH][MAP_HEIGHT];
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                map[x][y] = colour::RGB::none;
            }
        }

        colour pixelCol;
        uint32_t channels = level.bmp_info_header.bit_count / 8;
        for (uint32_t y = 0; y < MAP_HEIGHT; ++y) {
            for (uint32_t x = 0; x < MAP_WIDTH; ++x) {
                for (uint32_t i = 0; i < 3; ++i)
                {
                    pixelCol.bgr[i] = level.data[channels * (y * level.bmp_info_header.width + x) + i];
                }

                map[x][MAP_HEIGHT - y - 1] = pixelCol.getColour();

            }
        }

        std::ofstream output(outFile, std::ios::trunc);
        if (!output)
        {
            std::cout << "Could not open/create output file." << std::endl;
        }
        else
        {
            output << bgFile << "\n";
            for (uint32_t y = 0; y < MAP_HEIGHT; ++y) {
                for (uint32_t x = 0; x < MAP_WIDTH; ++x) {
                    output << static_cast<int>(map[x][y]) << ",";
                }
                output << "\n";
            }
            output.close();
        }
    }
}