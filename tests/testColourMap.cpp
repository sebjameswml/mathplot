#include <limits>
#include <vector>
#include <list>
#include <array>
#include <iostream>
#include <mplot/ColourMap.h>

int main ()
{
    int rtn = 0;

    std::array<float,3> c = {0,0,0};
    std::array<float,3> mid_jet = {0.541f, 0.76f, 0.018f};

    // NB: This test assumes that all colour maps default to Jet

    mplot::ColourMap<float> cmf(mplot::ColourMapType::Jet);
    c = cmf.convert(0.0f);
    std::cout << "(float) Colour (0.0): " << c[0] << "," << c[1] << ","<< c[2] << std::endl;
    c = cmf.convert(0.5f);
    std::cout << "(float) Colour (0.5): " << c[0] << "," << c[1] << ","<< c[2] << std::endl;
    if (c != mid_jet) { --rtn; std::cout << "f fail\n"; }
    c = cmf.convert(1.0f);
    std::cout << "(float) Colour (1.0): " << c[0] << "," << c[1] << ","<< c[2] << std::endl;

    mplot::ColourMap<double> cmd(mplot::ColourMapType::Jet);
    c = cmd.convert(0.5);
    if (c != mid_jet) { --rtn;std::cout << "d fail\n"; }
    std::cout << "(double) Colour: " << c[0] << "," << c[1] << ","<< c[2] << std::endl;

    mplot::ColourMap<unsigned char> cmuc(mplot::ColourMapType::Jet);
    std::cout << "(uchar) range_max: " << (int)cmuc.range_max << std::endl;
    cmuc.range_max = 254;
    std::cout << "(uchar) now range_max: " << (int)cmuc.range_max << std::endl;
    c = cmuc.convert(127);
    if (c != mid_jet) { --rtn; std::cout << "uc fail\n"; }
    std::cout << "(uchar) Colour: " << c[0] << "," << c[1] << ","<< c[2] << std::endl;

    mplot::ColourMap<char> cmc(mplot::ColourMapType::Jet);
    std::cout << "(char) range_max: " << (int)cmc.range_max << std::endl;
    // Because 127 is prime, change range to 126 to get exact output suitable for testing
    cmc.range_max = 126;
    c = cmc.convert(63);
    if (c != mid_jet) { --rtn;std::cout << "c fail\n"; }
    std::cout << "(char) Colour: " << c[0] << "," << c[1] << ","<< c[2] << std::endl;

    // Int colour map has range 0-255 by default. Up this to something else
    mplot::ColourMap<int> cmi(mplot::ColourMapType::Jet);
    cmi.range_max = 20000;
    c = cmi.convert(10000);
    if (c != mid_jet) { --rtn; std::cout << "i fail\n"; }
    std::cout << "(int) Colour: " << c[0] << "," << c[1] << ","<< c[2] << std::endl;

    mplot::ColourMap<unsigned int> cmui(mplot::ColourMapType::Jet);
    cmui.range_max = 10000;
    c = cmui.convert(5000);
    if (c != mid_jet) { --rtn;std::cout << "ui fail\n"; }
    std::cout << "(uint) Colour: " << c[0] << "," << c[1] << ","<< c[2] << std::endl;

    mplot::ColourMap<short> cms(mplot::ColourMapType::Jet);
    cms.range_max = 1000;
    c = cms.convert(500);
    if (c != mid_jet) { --rtn;  std::cout << "s fail\n"; }
    std::cout << "(short) Colour: " << c[0] << "," << c[1] << ","<< c[2] << std::endl;

    mplot::ColourMap<unsigned short> cmus(mplot::ColourMapType::Jet);
    cmus.range_max = 1000;
    c = cmus.convert(500);
    if (c != mid_jet) { --rtn; std::cout << "us fail\n"; }
    std::cout << "(ushort) Colour: " << c[0] << "," << c[1] << ","<< c[2] << std::endl;

    mplot::ColourMap<unsigned long long int> cmlli(mplot::ColourMapType::Jet);
    cmlli.range_max = 256;
    c = cmlli.convert(128);
    if (c != mid_jet) { --rtn; std::cout << "lli fail\n"; }
    std::cout << "(unsigned long long int) Colour: " << c[0] << "," << c[1] << ","<< c[2] << std::endl;

    mplot::ColourMap<unsigned long long int> cmulli(mplot::ColourMapType::Jet);
    cmulli.range_max = 1000000;
    c = cmulli.convert(500000);
    if (c != mid_jet) { --rtn; std::cout << "ulli fail\n"; }
    std::cout << "(unsigned long long int) Colour: " << c[0] << "," << c[1] << ","<< c[2] << std::endl;

    return rtn;
}
