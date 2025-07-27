#include <UserInterface.h>

UInterface::UInterface()
    : _display(U8G2_R0, /* reset=*/U8X8_PIN_NONE) {}

void UInterface::begin()
{
    _display.begin();
    _display.clearBuffer();

    const char *line1 = "ROBOTIC CLUB";
    const char *line2 = "Universitas Negeri";
    const char *line3 = "Jakarta";
    const char *line4 = "[ === BOOTING === ]";

    _display.setFont(u8g2_font_10x20_me);

    int16_t width1 = _display.getStrWidth(line1);
    int16_t x1 = (128 - width1) / 2;
    _display.drawStr(x1, 30, line1);
    _display.sendBuffer();

    _display.setFont(u8g2_font_6x10_tf);

    int16_t width2 = _display.getStrWidth(line2);
    int16_t x2 = (128 - width2) / 2;
    _display.drawStr(x2, 40, line2);

    int16_t width3 = _display.getStrWidth(line3);
    int16_t x3 = (128 - width3) / 2;
    _display.drawStr(x3, 50, line3);
    _display.sendBuffer();

    _display.setFont(u8g2_font_5x7_t_cyrillic);

    int16_t width4 = _display.getStrWidth(line4);
    int16_t x4 = (128 - width4) / 2;
    _display.drawStr(x4, 64, line4);
    _display.sendBuffer();

    _display.display();
}

void UInterface::_drawTopSection()
{
    _display.clearBuffer();

    _display.setFont(u8g2_font_6x10_tf);
    _display.drawStr(5, 7, "RC UNJ");
    _display.drawStr(105, 7, "RFID");

    _display.sendBuffer();
}

void UInterface::_drawToScreen()
{
    _display.sendBuffer();
    _display.display();
}

void UInterface::drawStandbyLogo()
{
    _display.clearBuffer();

    _display.drawXBMP(0, 0, 128, 64, Robotic);

    this->_drawToScreen();
}

void UInterface::drawStandbyText()
{
    this->_drawTopSection();

    _display.setFont(u8g2_font_10x20_me);
    _display.drawStr(37, 30, "TEMPEL");
    _display.drawStr(41, 50, "KARTU");

    this->_drawToScreen();
}

void UInterface::drawFindingData()
{
    this->_drawTopSection();

    static uint8_t dotCount = 0;
    dotCount = (dotCount + 1) % 4;

    char dots[5] = {0};
    for (uint8_t i = 0; i < dotCount; i++)
    {
        dots[i] = '.';
    }

    _display.setFont(u8g2_font_10x20_me);
    char text[20];
    sprintf(text, "MENCARI%s", dots);

    int16_t width = _display.getStrWidth(text);
    int16_t x = (128 - width) / 2;

    // Tampil di tengah layar
    _display.drawStr(x, 40, text);

    this->_drawToScreen();
}

void UInterface::drawFoundState()
{
    this->_drawTopSection();

    _display.setFont(u8g2_font_10x20_me);

    const char *line1 = "SILAKAN";
    const char *line2 = "MASUK";

    int16_t width1 = _display.getStrWidth(line1);
    int16_t x1 = (128 - width1) / 2;
    _display.drawStr(x1, 30, line1);

    int16_t width2 = _display.getStrWidth(line2);
    int16_t x2 = (128 - width2) / 2;
    _display.drawStr(x2, 50, line2);

    this->_drawToScreen();
}

void UInterface::drawOpenByNonCardRead()
{
    this->_drawTopSection();

    _display.setFont(u8g2_font_10x20_me);

    const char *msg = "TERBUKA";

    int16_t width = _display.getStrWidth(msg);
    int16_t x = (128 - width) / 2;

    _display.drawStr(x, 40, msg);

    this->_drawToScreen();
}

void UInterface::drawNotFoundState()
{
    this->_drawTopSection();

    _display.setFont(u8g2_font_10x20_me);

    const char *line1 = "ANDA TIDAK";
    const char *line2 = "DIZINKAN";

    int16_t width1 = _display.getStrWidth(line1);
    int16_t x1 = (128 - width1) / 2;
    _display.drawStr(x1, 30, line1);

    int16_t width2 = _display.getStrWidth(line2);
    int16_t x2 = (128 - width2) / 2;
    _display.drawStr(x2, 50, line2);

    this->_drawToScreen();
}