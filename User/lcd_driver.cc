//
// Created by A on 2024/6/17.
//

#include "lcd_driver.h"

#include "main.h"

/*
 * @brief Lcd driver single instance
 */
LcdDriver &LcdDriver::instance() {
  static LcdDriver lcd_driver;
  return lcd_driver;
}

void LcdDriver::Init() {
  width_ = 320;
  height_ = 240;
  type_ = HX8347;
  direction_ = LANDSCAPE;
  spi_ = &hspi2;
  /* Keep rest pin high */
  LCD_RST(1);
  Delay(100);
  WriteRegData(0xEA, 0x00); // Command page 0
  WriteRegData(0xEB, 0x00); // SUB_SEL=0xF6
  /* Power saving for HX8357-A */
  WriteRegData(0xEC, 0x3C); // Command page 0
  WriteRegData(0xED, 0xC4); // GENON=0x00
  WriteRegData(0xE8, 0x48); // EQVCI_M1=0x00
  WriteRegData(0xE9, 0x38); // EQGND_M1=0x1C
  WriteRegData(0xF1, 0x01); // EQVCI_M0=0x1C
  WriteRegData(0xF2, 0x00); // EQGND_M0=0x1C
  WriteRegData(0x27, 0xA3); // For GRAM read/write speed
  WriteRegData(0x2E, 0x76); // For Gate timing, prevent the display abnormal in RGB I/F
  WriteRegData(0x60, 0x08);
  WriteRegData(0x29, 0x01);
  WriteRegData(0x2B, 0x02);
  WriteRegData(0x36, 0x09); // mode
  /* Gamma Setting */
  WriteRegData(0x40, 0x00);
  WriteRegData(0x41, 0x01);
  WriteRegData(0x42, 0x01);
  WriteRegData(0x43, 0x12);
  WriteRegData(0x44, 0x10);
  WriteRegData(0x45, 0x24);
  WriteRegData(0x46, 0x05);
  WriteRegData(0x47, 0x5B);
  WriteRegData(0x48, 0x03);
  WriteRegData(0x49, 0x11);
  WriteRegData(0x4A, 0x17);
  WriteRegData(0x4B, 0x18);
  WriteRegData(0x4C, 0x19);
  WriteRegData(0x50, 0x1B);
  WriteRegData(0x51, 0x2F);
  WriteRegData(0x52, 0x2D);
  WriteRegData(0x53, 0x3E);
  WriteRegData(0x54, 0x3E);
  WriteRegData(0x55, 0x3F);
  WriteRegData(0x56, 0x30);
  WriteRegData(0x57, 0x6E);
  WriteRegData(0x58, 0x06);
  WriteRegData(0x59, 0x07);
  WriteRegData(0x5A, 0x08);
  WriteRegData(0x5B, 0x0E);
  WriteRegData(0x5C, 0x1C);
  WriteRegData(0x5D, 0xCC);

  /* Set GRAM area 320x240 */
  WriteRegData(0x02, 0x00);
  WriteRegData(0x03, 0x00);
  WriteRegData(0x04, 0x00);
  WriteRegData(0x05, 0xEF);
  WriteRegData(0x06, 0x00);
  WriteRegData(0x07, 0x00);
  WriteRegData(0x08, 0x01);
  WriteRegData(0x09, 0x3F);

  /* Power Setting */
  WriteRegData(0xE2, 0x03);
  WriteRegData(0x1B, 0x1D);
  WriteRegData(0x1A, 0x01);
  WriteRegData(0x24, 0x37); // Set VCOMH voltage, VHH=0x64
  WriteRegData(0x25, 0x4F); // Set VCOML voltage, VML=0x71
  WriteRegData(0x23, 0x6A); // Set VCOM offset, VMF=0x52
//  /* Power on Setting */
  WriteRegData(0x18, 0x3A); // OSC_EN=1, Start to Oscillate
  WriteRegData(0x19, 0x01); // AP=011

  WriteRegData(0x01, 0x00); // Normal display(Exit Deep standby mode)
  WriteRegData(0x1F, 0x88); // Exit standby mode and Step-up circuit 1 enable
  // GAS_EN=1, VCOMG=0, PON=0, DK=0, XDK=0, DDVDH_TRI=0, STB=0
  Delay(6);
  WriteRegData(0x1F, 0x80); // Step-up circuit 2 enable
  // GAS_EN=1, VCOMG=0, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
  Delay(6);
  WriteRegData(0x1F, 0x90);
  Delay(6);
  WriteRegData(0x1F, 0xD0);
  // GAS_EN=1, VCOMG=1, PON=1, DK=0, XDK=1, DDVDH_TRI=0, STB=0
  Delay(6);
  // Display ON Setting
  WriteRegData(0x17, 0x05); // GON=1, DTE=1, D[1:0]=10
  Delay(6);
  WriteRegData(0x28, 0x38); // GON=1, DTE=1, D[1:0]=11
  FillOneColor(0, width_ - 1, 0, height_ - 1, 0x99F0);
  WriteRegData(0x28, 0x3C); // 16-bit/pixel
  /* Set lcd direction */
  SetDirection(LANDSCAPE);
  FillOneColor(0, width_ - 1, 0, height_ - 1, 0xFCF0);
  /* Test */
  Test();
}

void LcdDriver::Test() {
  /* Display Bgr green */
  //FillOneColor(0, width_, 0, height_, 0x07E0);
  /* Display Bgr yellow */
  FillOneColor(0, width_ / 2, 0, height_ / 2, 0x40F0);
}
/*
 * @brief port function to send data to lcd
 */
void LcdDriver::SendBytes(uint8_t *data, uint16_t size, uint8_t mode) {
  if (mode == 1) {
    /* DMA send */
    HAL_SPI_Transmit_DMA((SPI_HandleTypeDef *) spi_, data, size);
    //return;
  } else {
    /* Normal spi send */
    HAL_SPI_Transmit((SPI_HandleTypeDef *) spi_, data, size, 1000);
  }
}

void LcdDriver::Delay(int32_t ms) {
  HAL_Delay(ms);
}

void LcdDriver::WriteReg(uint8_t reg) {
  LCD_CS(0);
  LCD_DC(0);
  SendBytes(&reg, 1);
  LCD_CS(1);
}

void LcdDriver::WriteData(uint8_t data) {
  // spi_trans_comp = 0; //updating
  LCD_CS(0);
  LCD_DC(1);
  SendBytes(&data, 1);
  LCD_CS(1);
}

void LcdDriver::WriteData16Bits(uint16_t data) {
  uint8_t h = data >> 8;
  uint8_t l = (uint8_t) data;
  LCD_CS(0);
  LCD_DC(1);
  uint8_t data16[2] = {h, l};
  SendBytes(data16, 2);
  LCD_CS(1);
}

void LcdDriver::WriteData16Bits(uint16_t *data, int32_t size) {
  LCD_CS(0);
  LCD_DC(1);
  SendBytes((uint8_t *) data, size * 2, 1);
  LCD_CS(1);
}

void LcdDriver::WriteRegData(uint8_t reg, uint8_t data) {
  WriteReg(reg);
  WriteData(data);
}

/*
 * @brief set window that will be draw
 */
void LcdDriver::SetWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end) {
  /* Set column/width that will be draw */
  /* 0-239; 0-319*/
  /* 1-240; 1-320*/
  WriteRegData(0x02, (x_start) >> 8);
  WriteRegData(0x03, (x_start) & 0xFF);
  WriteRegData(0x04, (x_end) >> 8);
  WriteRegData(0x05, (x_end) & 0xFF);

  /* Set row/height that will be draw */
  WriteRegData(0x06, (y_start) >> 8);
  WriteRegData(0x07, (y_start) & 0xFF);
  WriteRegData(0x08, (y_end) >> 8);
  WriteRegData(0x09, (y_end) & 0xFF);

  /* Prepare write to gram */
  WriteReg(0x22);
}

void LcdDriver::SetDirection(LcdDriver::Direction direction) {
  direction_ = direction;
  uint16_t direction_code = 0;
  switch (direction_) {
    case PORTRAIT:direction_code = BGR | MV | MY;
      break;
    case LANDSCAPE:direction_code = BGR | MV | MY;
      width_ = 320;
      height_ = 240;
      break;
    case PORTRAIT_REV:break;
    case LANDSCAPE_REV:break;
  }
  WriteRegData(0x16, direction_code);
}
//extern bool spi_trans_comp; //SPI
/*
 * @brief flush the color map to lcd
 */
void LcdDriver::FlushColor(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end, uint16_t *color_map) {
  if (x_end >= width_)
    x_end = width_ - 1;
  if (y_end >= height_)
    y_end = height_ - 1;
  int width = x_end - x_start + 1;
  int height = y_end - y_start + 1;
  int size = width * height;

  SetWindow(x_start, y_start, x_end, y_end);
//  spi_trans_comp = true;
  WriteData16Bits(color_map, size);
}

void LcdDriver::FillOneColor(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end, uint16_t color) {
  if (x_end >= width_)
    x_end = width_ - 1;
  if (y_end >= height_)
    y_end = height_ - 1;

  SetWindow(x_start, y_start, x_end, y_end); //can not fill full
  int width = x_end - x_start + 1;
  int height = y_end - y_start + 1;
  for (int i = 0; i < height * width; i++) {
    WriteData16Bits(color); // can not over window size
  }
}






