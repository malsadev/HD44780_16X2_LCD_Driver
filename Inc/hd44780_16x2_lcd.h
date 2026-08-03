#include <stdint.h>

typedef struct lcd_handle_s lcd_handle_s;
typedef enum lcd_bus_width_e { BUS_4BIT, BUS_8BIT } lcd_bus_width_e;
typedef enum lcd_display_line_e {
  ONE_LINE_DISPLAY,
  TWO_LINE_DISPLAY
} lcd_display_line_e;
typedef enum lcd_display_font_e { FONT_5X8, FONT_5X10 } lcd_display_font_e;
typedef enum lcd_pins_e {
  DB0,
  DB1,
  DB2,
  DB3,
  DB4,
  DB5,
  DB6,
  DB7,
  RS,
  RW,
  EN
} lcd_pins_e;
typedef void (*lcd_pin_set_fn)(void *ctx, lcd_pins_e lcd_pin, uint8_t state);
typedef void (*delay_fn)(uint32_t ms);
typedef struct lcd_ops_s {
  void (*init)(void *ctx);
  lcd_pin_set_fn set_pin;
  delay_fn delay_ms;
} lcd_ops_s;
typedef struct lcd_config_s {
  void *ctx;
  const lcd_ops_s *ops;
  lcd_bus_width_e bus_width;
  lcd_display_line_e display_lines;
  lcd_display_font_e display_font;
} lcd_config_s;
void LCD_Init(lcd_handle_s *lcd_handle, lcd_config_s *lcd_config);
void LCD_Function_Set(lcd_handle_s *lcd_handle, lcd_bus_width_e bus_width,
                      lcd_display_line_e display_lines,
                      lcd_display_font_e display_font);
// vim: set filetype=c :
