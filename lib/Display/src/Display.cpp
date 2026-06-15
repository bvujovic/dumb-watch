#include "Display.h"

Display::Display(byte pinClock, byte pinDio, byte brightness)
{
    disp = new TM1637Display(pinClock, pinDio);
    disp->setBrightness(brightness);
}

void Display::clear()
{
    disp->clear();
}

void Display::clear(uint32_t itvDelay)
{
    clear();
    delay(itvDelay);
}

void Display::nums(int x, int y)
{
    int d = x * 100 + y;
    disp->showNumberDecEx(d, 0b11100000, true);
}

uint8_t Display::encodeCharToSegments(char c)
{
    switch (c)
    {
    case 'A':
    case 'a':
        return SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
    case 'B':
    case 'b':
        return SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
    case 'C':
    case 'c':
        // return SEG_D | SEG_E | SEG_G;
        return SEG_A | SEG_D | SEG_E | SEG_F;
    case 'D':
    case 'd':
        return SEG_B | SEG_C | SEG_D | SEG_E | SEG_G;
    case 'E':
    case 'e':
        return SEG_A | SEG_D | SEG_E | SEG_F | SEG_G;
    case 'F':
    case 'f':
        return SEG_A | SEG_E | SEG_F | SEG_G;
    case 'G':
    case 'g':
        // return SEG_A | SEG_C | SEG_D | SEG_E | SEG_F;
        return SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
    case 'H':
    case 'h':
        return SEG_C | SEG_E | SEG_F | SEG_G;
    case 'I':
    case 'i':
        // return SEG_E;
        return SEG_E | SEG_F;
    case 'J':
    case 'j':
        return SEG_B | SEG_C | SEG_D;
    case 'K':
    case 'k':
        // return SEG_C | SEG_E | SEG_F | SEG_G;
        return SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
    case 'L':
    case 'l':
        return SEG_D | SEG_E | SEG_F;
    case 'M':
    case 'm':
        return SEG_B | SEG_C | SEG_E | SEG_F | SEG_G;
    case 'N':
    case 'n':
        return SEG_C | SEG_E | SEG_G;
    case 'O':
    case 'o':
        return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
    case 'P':
    case 'p':
        return SEG_A | SEG_B | SEG_E | SEG_F | SEG_G;
    case 'Q':
    case 'q':
        return SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
    case 'R':
    case 'r':
        return SEG_E | SEG_G;
    case 'S':
    case 's':
        return SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
    case 'T':
    case 't':
        return SEG_D | SEG_E | SEG_F | SEG_G;
    case 'U':
    case 'u':
        return SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
    case 'Y':
    case 'y':
        return SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
    case 'Z':
    case 'z':
        return SEG_A | SEG_B | SEG_D | SEG_E | SEG_G;
    case '0':
        return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F;
    case '1':
        return SEG_B | SEG_C;
    case '2':
        return SEG_A | SEG_B | SEG_D | SEG_E | SEG_G;
    case '3':
        return SEG_A | SEG_B | SEG_C | SEG_D | SEG_G;
    case '4':
        return SEG_B | SEG_C | SEG_F | SEG_G;
    case '5':
        return SEG_A | SEG_C | SEG_D | SEG_F | SEG_G;
    case '6':
        return SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
    case '7':
        return SEG_A | SEG_B | SEG_C;
    case '8':
        return SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G;
    case '9':
        return SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G;
    case ' ':
        return 0;
    case '\'':
        return SEG_F;
    case '*':
        return SEG_A | SEG_B | SEG_F | SEG_G;
    default:
        return 0;
    }
}

void Display::string(const char *str, bool colon)
{
    uint8_t segments[4];
    for (int i = 0; i < 4; ++i)
        segments[i] = encodeCharToSegments(str[i]);
    if (colon)
        segments[1] |= SEG_DP;
    disp->setSegments(segments, 4);
}

// void displayTimerName(const TimerItem &timer)
// {
//   // if all charachers in timer.name are digits - display that data with displayNums(temp, hum)
//   // maybe this should be changed: if name is e.g. "----" - display time based on interval (convert it to min:sec)
//   auto allDigits = true;
//   for (auto i = 0; i < 4; ++i)
//     if (timer.name[i] < '0' || timer.name[i] > '9')
//       allDigits = false;
//   if (allDigits)
//   {
//     auto x = (timer.name[0] - '0') * 10 + timer.name[1] - '0';
//     auto y = (timer.name[2] - '0') * 10 + timer.name[3] - '0';
//     displayNums(x, y);
//     return;
//   }
//   uint8_t segments[4];
//   for (int i = 0; i < 4; ++i)
//     segments[i] = encodeCharToSegments(timer.name[i]);
//   display.setSegments(segments, 4);
//   delay(1000); //?
// }
