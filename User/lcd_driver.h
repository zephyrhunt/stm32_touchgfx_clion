//
// Created by A on 2024/6/17.
//

#ifndef STM32F411_MEASURE_USER_LCD_DRIVER_H_
#define STM32F411_MEASURE_USER_LCD_DRIVER_H_

#include <cstdint>

#include "spi.h"

#define LCD_DC(x)   HAL_GPIO_WritePin(SPI2_DC_GPIO_Port,SPI2_DC_Pin,(GPIO_PinState)x)
#define LCD_RST(x)  HAL_GPIO_WritePin(SPI2_RST_GPIO_Port,SPI2_RST_Pin,(GPIO_PinState)x)
#define LCD_CS(x)    // HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,(GPIO_PinState)x)

class LcdDriver{
  enum LcdType {
    HX8347,
    ILI9325,
    ILI9328,
  };

  enum Direction {
    PORTRAIT, /* 竖屏 */
    LANDSCAPE, /* 横屏 */
    PORTRAIT_REV, /* 竖屏反转 */
    LANDSCAPE_REV,
  };

  enum Flag {
    RGB = 0x00,
    BGR = 0x08,
    MY = 0x80,
    MX = 0x40,
    MV = 0x20,
    ML = 0x10,
  };
 public:
  static LcdDriver &instance();

  explicit LcdDriver(int16_t width = 320, int16_t height = 240, LcdType type = HX8347)
      : width_(width), height_(height), type_(type) {
				width_ = 240;
				height_ = 320;
				type_ = HX8347;
				direction_ = LANDSCAPE;
    spi_ = &hspi2;
  };

  void Init();
  void Delay(int32_t ms);
  void Test();
  void FlushColor(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end, uint16_t *color_map);
	void FillOneColor(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end, uint16_t color);
 private:
  int16_t width_;
  int16_t height_;
  LcdType type_;
  Direction direction_;
  SPI_HandleTypeDef *spi_;

  void SendBytes(uint8_t *data, uint16_t size, uint8_t mode = 0);
  void WriteReg(uint8_t reg);
  void WriteData(uint8_t data);
  void WriteData16Bits(uint16_t data);
  void WriteData16Bits(uint16_t *data, int32_t size);
  void WriteRegData(uint8_t reg, uint8_t data);
  void SetWindow(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);
  void SetDirection(Direction);

  
};

#endif //STM32F411_MEASURE_USER_LCD_DRIVER_H_
