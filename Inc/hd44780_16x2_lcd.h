#include <stdint.h>

typedef enum lcd_bus_width_e { BUS_4BIT, BUS_8BIT } lcd_bus_width_e;
typedef enum lcd_display_line_e {
  ONE_LINE_DISPLAY,
  TWO_LINE_DISPLAY
} lcd_display_line_e;
typedef enum lcd_display_font_e { FONT_5X8, FONT_5X10 } lcd_display_font_e;
typedef enum lcd_cursor_direction_e {
  CURSOR_DECREMENT,
  CURSOR_INCREMENT
} lcd_cursor_direction_e;
typedef enum lcd_display_shift_e {
  DISPLAY_SHIFT_OFF,
  DISPLAY_SHIFT_ON
} lcd_display_shift_e;
typedef enum lcd_display_state_e { DISPLAY_OFF, DISPLAY_ON } lcd_display_state_e;
typedef enum lcd_cursor_state_e { CURSOR_OFF, CURSOR_ON } lcd_cursor_state_e;
typedef enum lcd_blink_state_e { BLINK_OFF, BLINK_ON } lcd_blink_state_e;
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
typedef struct lcd_handle_s {
  void *ctx;
  const lcd_ops_s *ops;
  lcd_bus_width_e bus_width;
  lcd_display_line_e display_lines;
  lcd_display_font_e display_font;
} lcd_handle_s;
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
void LCD_Entry_Mode_Set(lcd_handle_s *lcd_handle,
                        lcd_cursor_direction_e cursor_direction,
                        lcd_display_shift_e display_shift);
void LCD_Write_Char(lcd_handle_s *lcd_handle, char character);
void LCD_Write_String(lcd_handle_s *lcd_handle, const char *string);
void LCD_Clear_Display(lcd_handle_s *lcd_handle);
void LCD_Return_Home(lcd_handle_s *lcd_handle);
void LCD_Set_Cursor(lcd_handle_s *lcd_handle, uint8_t row, uint8_t col);
void LCD_Display_Control(lcd_handle_s *lcd_handle,
                         lcd_display_state_e display_state,
                         lcd_cursor_state_e cursor_state,
                         lcd_blink_state_e blink_state);
// vim: set filetype=c :
