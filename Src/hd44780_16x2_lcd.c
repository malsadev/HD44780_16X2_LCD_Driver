#include "hd44780_16x2_lcd.h"
#include <stdint.h>

// FUNCTION SET BITMASKS
#define FUNCTION_SET 0x20
#define FUNCTION_SET_8BIT_INTERFACE 0x10
#define FUNCTION_SET_4BIT_INTERFACE 0x00
#define FUNCTION_SET_1LINE_DISPLAY 0x00
#define FUNCTION_SET_2LINE_DISPLAY 0x08
#define FUNCTION_SET_5X8_FONT 0x00
#define FUNCTION_SET_5X10_FONT 0x04

// DISPLAY CONTROL BITMASKS
#define DISPLAY_CONTROL 0x08
#define DISPLAY_CONTROL_DISPLAY_ON 0x04
#define DISPLAY_CONTROL_DISPLAY_OFF 0x00
#define DISPLAY_CONTROL_CURSOR_ON 0x02
#define DISPLAY_CONTROL_CURSOR_OFF 0x00
#define DISPLAY_CONTROL_BLINK_ON 0x01
#define DISPLAY_CONTROL_BLINK_OFF 0x00

#define DISPLAY_CLEAR 0x01

// ENTRY MODE SET BITMASKS
#define ENTRY_MODE_SET 0x04
#define ENTRY_MODE_SET_INCREMENT 0x02
#define ENTRY_MODE_SET_DECREMENT 0x00
#define ENTRY_MODE_SET_DISPLAY_SHIFT_ON 0x01
#define ENTRY_MODE_SET_DISPLAY_SHIFT_OFF 0x00

#define DISPLAY_RAM_ADDR_SET 0x80
#define DISPLAY_FIRST_ROW_OFFSET 0x00
#define DISPLAY_SECOND_ROW_OFFSET 0x40

typedef enum signal_type_e { INSTRUCTION_SIG, DATA_SIG } signal_type_e;
static void Write_Bus(uint8_t bitmask, lcd_handle_s *h) {
  for (lcd_pins_e lcd_pin = DB0; lcd_pin <= DB7; lcd_pin++) {
    h->ops->set_pin(h->ctx, lcd_pin, (bitmask >> lcd_pin) & 0x0001);
  }
}

static void Latch(lcd_handle_s *h) {
  h->ops->set_pin(h->ctx, EN, 1);
  h->ops->delay_ms(1);
  h->ops->set_pin(h->ctx, EN, 0);
}

static void Send_Byte(signal_type_e signal_type, uint8_t bitmask,
                      lcd_handle_s *h) {
  h->ops->set_pin(h->ctx, RS, signal_type);
  h->ops->set_pin(h->ctx, RW, 0);
  Write_Bus(bitmask, h);
  Latch(h);
}

static void Send_Command(uint8_t bitmask, lcd_handle_s *h) {
  Send_Byte(INSTRUCTION_SIG, bitmask, h);
}

static void Send_Data(uint8_t data, lcd_handle_s *h) {
  Send_Byte(DATA_SIG, data, h);
}

void LCD_Write_Char(lcd_handle_s *h, char character) {
  Send_Data(character, h);
}

void LCD_Write_String(lcd_handle_s *h, const char *string) {
  for (const char *c = string; *c != '\0'; c++) {
    LCD_Write_Char(h, *c);
  }
}

// 0 indexed
void LCD_Set_Cursor(lcd_handle_s *h, uint8_t row, uint8_t col) {

  if (row == 0) {
    Send_Command(DISPLAY_RAM_ADDR_SET | (DISPLAY_FIRST_ROW_OFFSET | col), h);
  } else if (row == 1) {
    Send_Command(DISPLAY_RAM_ADDR_SET | (DISPLAY_SECOND_ROW_OFFSET | col), h);
  }
}
void LCD_Entry_Mode_Set(lcd_handle_s *h,
                        lcd_cursor_direction_e cursor_direction,
                        lcd_display_shift_e display_shift) {
  uint8_t entry_mode_set = ENTRY_MODE_SET;
  if (cursor_direction == CURSOR_DECREMENT) {
    entry_mode_set |= ENTRY_MODE_SET_DECREMENT;
  } else {
    entry_mode_set |= ENTRY_MODE_SET_INCREMENT;
  }
  if (display_shift == DISPLAY_SHIFT_OFF) {
    entry_mode_set |= ENTRY_MODE_SET_DISPLAY_SHIFT_OFF;
  } else {
    entry_mode_set |= ENTRY_MODE_SET_DISPLAY_SHIFT_ON;
  }
  Send_Command(entry_mode_set, h);
}

void LCD_Display_Control(lcd_handle_s *h, lcd_display_state_e display_state,
                         lcd_cursor_state_e cursor_state,
                         lcd_blink_state_e blink_state) {
  uint8_t display_control = DISPLAY_CONTROL;
  if (display_state == DISPLAY_OFF) {
    display_control |= DISPLAY_CONTROL_DISPLAY_OFF;
  } else {
    display_control |= DISPLAY_CONTROL_DISPLAY_ON;
  }
  if (cursor_state == CURSOR_OFF) {
    display_control |= DISPLAY_CONTROL_CURSOR_OFF;
  } else {
    display_control |= DISPLAY_CONTROL_CURSOR_ON;
  }
  if (blink_state == BLINK_OFF) {
    display_control |= DISPLAY_CONTROL_BLINK_OFF;
  } else {
    display_control |= DISPLAY_CONTROL_BLINK_ON;
  }
  Send_Command(display_control, h);
}

void LCD_Function_Set(lcd_handle_s *h, lcd_bus_width_e bus_width,
                      lcd_display_line_e display_lines,
                      lcd_display_font_e display_font) {
  uint8_t function_set = FUNCTION_SET;
  if (bus_width == BUS_4BIT) {
    function_set |= FUNCTION_SET_4BIT_INTERFACE;
  } else {
    function_set |= FUNCTION_SET_8BIT_INTERFACE;
  }
  if (display_lines == ONE_LINE_DISPLAY) {
    function_set |= FUNCTION_SET_1LINE_DISPLAY;
  } else {
    function_set |= FUNCTION_SET_2LINE_DISPLAY;
  }
  if (display_font == FONT_5X8) {
    function_set |= FUNCTION_SET_5X8_FONT;
  } else {
    function_set |= FUNCTION_SET_5X10_FONT;
  }
  Send_Command(function_set, h);
}
void LCD_Init(lcd_handle_s *h, lcd_config_s *c) {
  h->ctx = c->ctx;
  h->ops = c->ops;
  h->bus_width = c->bus_width;
  h->display_lines = c->display_lines;
  h->display_font = c->display_font;

  if (h->ops->init) {
    h->ops->init(h->ctx);
  }

  Send_Command(FUNCTION_SET | FUNCTION_SET_8BIT_INTERFACE, h);
  h->ops->delay_ms(5);
  Send_Command(FUNCTION_SET | FUNCTION_SET_8BIT_INTERFACE, h);
  h->ops->delay_ms(1);
  Send_Command(FUNCTION_SET | FUNCTION_SET_8BIT_INTERFACE, h);

  LCD_Function_Set(h, h->bus_width, h->display_lines, h->display_font);
  LCD_Display_Control(h, DISPLAY_OFF, CURSOR_OFF, BLINK_OFF);
  Send_Command(DISPLAY_CLEAR, h);
  LCD_Entry_Mode_Set(h, CURSOR_INCREMENT, DISPLAY_SHIFT_OFF);
  // INIT ENDS //
}
