// Copyright 2022 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include QMK_KEYBOARD_H

#include "manna-harbour_miryoku.h"

// Custom Imports
#include "wpm.h" // Required for wpm calculation
#include "transactions.h" // Required for transaction RPC
#include "eeconfig.h"  // Required for reading config
#include "host.h"  // Required for host_keyboard_led_state()

enum custom_keycodes {
  RESET_KEYSTROKE = SAFE_RANGE,
  RESET_KEYSTROKE_B,
  ML_SHIFT,      // Mouseless shift tap
  ML_CTRL_CMD,   // Ctrl tap (or Cmd if swapped)
  ML_ALT,        // Alt tap
  ML_CMD_CTRL,   // Cmd tap (or Ctrl if swapped)
  ML_COMMA,      // Comma with proper hold behavior
};

// Additional Features double tap guard

enum {
    U_TD_BOOT,
#define MIRYOKU_X(LAYER, STRING) U_TD_U_##LAYER,
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};

void u_td_fn_boot(tap_dance_state_t *state, void *user_data) {
  if (state->count == 2) {
    reset_keyboard();
  }
}

#define MIRYOKU_X(LAYER, STRING) \
void u_td_fn_U_##LAYER(tap_dance_state_t *state, void *user_data) { \
  if (state->count == 2) { \
    default_layer_set((layer_state_t)1 << U_##LAYER); \
  } \
}
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X

tap_dance_action_t tap_dance_actions[] = {
    [U_TD_BOOT] = ACTION_TAP_DANCE_FN(u_td_fn_boot),
#define MIRYOKU_X(LAYER, STRING) [U_TD_U_##LAYER] = ACTION_TAP_DANCE_FN(u_td_fn_U_##LAYER),
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};


// keymap

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
#define MIRYOKU_X(LAYER, STRING) [U_##LAYER] = U_MACRO_VA_ARGS(MIRYOKU_LAYERMAPPING_##LAYER, MIRYOKU_LAYER_##LAYER),
MIRYOKU_LAYER_LIST
#undef MIRYOKU_X
};


// shift functions

const key_override_t capsword_key_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);

const key_override_t **key_overrides = (const key_override_t *[]){
    &capsword_key_override,
    NULL
};


// thumb combos

#if defined (MIRYOKU_KLUDGE_THUMBCOMBOS)
const uint16_t PROGMEM thumbcombos_base_right[] = {LT(U_SYM, KC_ENT), LT(U_NUM, KC_BSPC), COMBO_END};
const uint16_t PROGMEM thumbcombos_base_left[] = {LT(U_NAV, KC_SPC), LT(U_MOUSE, KC_TAB), COMBO_END};
const uint16_t PROGMEM thumbcombos_nav[] = {KC_ENT, KC_BSPC, COMBO_END};
const uint16_t PROGMEM thumbcombos_mouse[] = {KC_BTN2, KC_BTN1, COMBO_END};
const uint16_t PROGMEM thumbcombos_media[] = {KC_MSTP, KC_MPLY, COMBO_END};
const uint16_t PROGMEM thumbcombos_num[] = {KC_0, KC_MINS, COMBO_END};
  #if defined (MIRYOKU_LAYERS_FLIP)
const uint16_t PROGMEM thumbcombos_sym[] = {KC_UNDS, KC_LPRN, COMBO_END};
  #else
const uint16_t PROGMEM thumbcombos_sym[] = {KC_RPRN, KC_UNDS, COMBO_END};
  #endif
const uint16_t PROGMEM thumbcombos_fun[] = {KC_SPC, KC_TAB, COMBO_END};
combo_t key_combos[COMBO_COUNT] = {
  COMBO(thumbcombos_base_right, LT(U_FUN, KC_DEL)),
  COMBO(thumbcombos_base_left, LT(U_MEDIA, KC_ESC)),
  COMBO(thumbcombos_nav, KC_DEL),
  COMBO(thumbcombos_mouse, KC_BTN3),
  COMBO(thumbcombos_media, KC_MUTE),
  COMBO(thumbcombos_num, KC_DOT),
  #if defined (MIRYOKU_LAYERS_FLIP)
  COMBO(thumbcombos_sym, KC_RPRN),
  #else
  COMBO(thumbcombos_sym, KC_LPRN),
  #endif
  COMBO(thumbcombos_fun, KC_APP)
};
#endif

// CUSTOM CONFIG



// OLED display configuration
#ifdef OLED_ENABLE
  #define KEYSTROKE_EEPROM_ADDR 32 // EEPROM address for keystroke count
  static uint32_t keystroke_count = 0; // Variable to store the keystroke count
  #define KEYSTROKE_B_EEPROM_ADDR (KEYSTROKE_EEPROM_ADDR + sizeof(uint32_t)) // EEPROM address for keystroke count
  static uint32_t keystroke_count_b = 0; // Variable to store the keystroke count
  static bool oled_screensaver_active = false; // Variable to track if the OLED screensaver is active
  static bool oled_cleared = false; // Variable to track if the OLED has been cleared
  static uint32_t last_sync = 0;
  static uint32_t last_save = 0;
  #define M_WID 5
  #define M_HEIGHT 16
  #define CHAR_START 33  // Start of visible ASCII
  #define CHAR_END 126   // End of visible ASCII
  #define M_INTERVAL 100 // ms between updates

  static char blank_chars[M_HEIGHT][M_WID];

  // Function to rotate the OLED display
  oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
  }

  // Layer names for OLED display
  enum layer_names {
    #define MIRYOKU_X(NAME, LABEL) LAYER_##NAME,
    MIRYOKU_LAYER_LIST
    #undef MIRYOKU_X
    LAYER_COUNT
  };

  // Function to get the highest layer and return its name
  const char* get_layer_name(uint8_t layer) {
    switch (layer) {
        #define MIRYOKU_X(NAME, LABEL) case LAYER_##NAME: return LABEL;
        MIRYOKU_LAYER_LIST
        #undef MIRYOKU_X
        default: return "Unknown";
    }
  }

  // Function to check if Ctrl and GUI keys are swapped
  bool is_ctrl_gui_swapped(void) {
      return (keymap_config.swap_lctl_lgui || keymap_config.swap_rctl_rgui);
  }

  // Function to load the keystroke count from EEPROM
  void load_keystroke_count(void) {
    eeprom_read_block((void*)&keystroke_count, (const void*)KEYSTROKE_EEPROM_ADDR, sizeof(keystroke_count));
  }
  // Function to load the keystroke count from EEPROM
  void load_keystroke_b_count(void) {
    eeprom_read_block((void*)&keystroke_count_b, (const void*)KEYSTROKE_B_EEPROM_ADDR, sizeof(keystroke_count_b));
  }

  // Function to save the keystroke count to EEPROM
  void save_keystroke_count(void) {
    eeprom_update_block((const void*)&keystroke_count, (void*)KEYSTROKE_EEPROM_ADDR, sizeof(keystroke_count));
  }
  // Function to save the keystroke count to EEPROM
  void save_keystroke_b_count(void) {
    eeprom_update_block((const void*)&keystroke_count_b, (void*)KEYSTROKE_B_EEPROM_ADDR, sizeof(keystroke_count_b));
  }

  // Helper to tap a key and count the keystroke
  void tap_and_count(uint16_t keycode) {
    tap_code(keycode);
    keystroke_count++;
    keystroke_count_b++;
  }

  // Helper to hold a key (register on press, unregister on release) and count
  void hold_and_count(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
      register_code(keycode);
      keystroke_count++;
      keystroke_count_b++;
    } else {
      unregister_code(keycode);
    }
  }

  // Function to increment the keystroke count
  bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Handle ML_COMMA - hold to repeat
    if (keycode == ML_COMMA) {
      hold_and_count(KC_COMM, record);
      return false;
    }

    // Handle mouseless.click keys - send taps on press
    if (record->event.pressed) {
      switch (keycode) {
        case ML_SHIFT:
          tap_and_count(KC_LSFT);
          return false;
        case ML_CTRL_CMD:
          tap_and_count(is_ctrl_gui_swapped() ? KC_LGUI : KC_LCTL);
          return false;
        case ML_ALT:
          tap_and_count(KC_LALT);
          return false;
        case ML_CMD_CTRL:
          tap_and_count(is_ctrl_gui_swapped() ? KC_LCTL : KC_LGUI);
          return false;
      }
    }

    if (record->event.pressed) {
      switch (keycode) {
        case RESET_KEYSTROKE:
          keystroke_count = 0;
          break;
        case RESET_KEYSTROKE_B:
          keystroke_count_b = 0;
          break;
        default:
          keystroke_count++;
          keystroke_count_b++;
          break;
      }
    }
    return true;
  }

  void stat_trak_sub_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const uint32_t* in_keystroke_count = (const uint32_t*)in_data;
    keystroke_count = *in_keystroke_count;
  }

  void stat_trak_b_sub_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const uint32_t* in_keystroke_count_b = (const uint32_t*)in_data;
    keystroke_count_b = *in_keystroke_count_b;
  }

  void housekeeping_task_user(void) {
    if (is_keyboard_master()) {
      // Interact with sub every 100ms
      if (timer_elapsed32(last_sync) > 100) {
        last_sync = timer_read32();
        transaction_rpc_send(STAT_TRAK, sizeof(keystroke_count), &keystroke_count);
        transaction_rpc_send(STAT_TRAK_B, sizeof(keystroke_count_b), &keystroke_count_b);
      }
      // Save the keystroke count every 10 seconds
      if (timer_elapsed32(last_save) > 10000) {
        last_save = timer_read32();
        save_keystroke_count();
        save_keystroke_b_count();
      }
    }
  }

  // Function to format the keystroke count as a string
  char* format_number_grouped(uint32_t num) {
    static char buffer[32];
    char digits[12];  // 10 digits max for uint32_t, + extra
    char* out = buffer;
    int len = 0;

    // Convert number to string, right-aligned
    snprintf(digits, sizeof(digits), "%lu", num);
    len = strlen(digits);

    // Calculate total padded length to nearest multiple of 3
    int padded_len = len;
    if (padded_len % 3 != 0) {
      padded_len += 3 - (padded_len % 3);
    }

    int pad_zeros = padded_len - len;

    // Write leading zeros
    for (int i = 0; i < pad_zeros; i++) {
      *out++ = '0';
    }

    // Copy digits
    for (int i = 0; i < len; i++) {
      *out++ = digits[i];
    }

    *out = '\0';

    // Now split into 3-digit groups with commas
    static char grouped[32];
    char* g = grouped;
    for (int i = 0; i < padded_len; i++) {
      if (i > 0 && i % 3 == 0) {
        *g++ = ',';
        *g++ = ' ';
      }
      *g++ = buffer[i];
    }
    *g = '\0';

    return grouped;
  }

  // Task to initialize the stored variables
  void keyboard_post_init_user(void) {
    if (is_keyboard_master()) {
      load_keystroke_count(); // Your EEPROM or variable setup
      load_keystroke_b_count();
    }
    transaction_register_rpc(STAT_TRAK, stat_trak_sub_handler);
    transaction_register_rpc(STAT_TRAK_B, stat_trak_b_sub_handler);
    for (int col = 0; col < M_WID; col++) {
      for (int row = 0; row < M_HEIGHT; row++) {
        blank_chars[row][col] = ' ';
      }
    }
  }

  static uint32_t last_matrix_update = 0;
  static char matrix_chars[M_HEIGHT][M_WID];
  static uint8_t matrix_pos[M_WID] = {0};

  void render_animation(void) {
    if (timer_elapsed32(last_matrix_update) < M_INTERVAL) return;
    last_matrix_update = timer_read32();

    // Advance character positions
    for (int col = 0; col < M_WID; col++) {
      matrix_pos[col] = (matrix_pos[col] + 1) % M_HEIGHT;
      for (int row = 0; row < M_HEIGHT; row++) {
        if (row == matrix_pos[col]) {
          // Generate a random printable ASCII character
          matrix_chars[row][col] = (char)(CHAR_START + (rand() % (CHAR_END - CHAR_START)));
        } else {
          // Leave previous char or blank
          if (rand() % 3 == 0) {
            matrix_chars[row][col] = ' ';
          }
        }
      }
    }

    oled_clear();

    // Print matrix to OLED
    for (int row = 0; row < M_HEIGHT; row++) {
      oled_set_cursor(0, row);
      for (int col = 0; col < M_WID; col++) {
        oled_write_char(matrix_chars[row][col], false);
      }
    }
  }

  void print_blank(void) {
    for (int row = 0; row < M_HEIGHT; row++) {
      oled_set_cursor(0, row);
      for (int col = 0; col < M_WID; col++) {
        oled_write_char(blank_chars[row][col], false);
      }
    }
    oled_cleared = true; // Mark that the OLED has been cleared
  }

  // OLED task to display information
  bool oled_task_user(void) {
    if(last_input_activity_elapsed() < OLED_SHORT_TIMEOUT) {
      // Turn the OLED on and get current layer state
      if (!is_oled_on() || oled_screensaver_active) {
        oled_on();
        print_blank(); // Load the keystroke count from EEPROM
        oled_screensaver_active = false;
        return false;
      }
      oled_cleared = false; // Reset cleared state when OLED is active
      if (is_keyboard_master()) {
        oled_clear();

        // Print Layer information
        uint8_t layer = get_highest_layer(layer_state);
        if (layer == 0) {
          layer = get_highest_layer(default_layer_state);
        }
        oled_write_P(PSTR("Layer"), false);
        const char* layer_name = get_layer_name(layer);
        oled_write_ln(layer_name, false);
        if (strlen(layer_name) % 5 != 0) {
          oled_write_ln("", false);
        }

        // Print SWAP status
        if (is_ctrl_gui_swapped()) {
          oled_write_P(PSTR("+SWAP"), true);
        } else {
          oled_write_P(PSTR("-SWAP"), false);
        }
        oled_write_ln("", false);

        // Print Caps Lock status
        if (host_keyboard_led_state().caps_lock) {
          oled_write_P(PSTR("+CAPS"), true);
        } else {
          oled_write_P(PSTR("-CAPS"), false);
        }
        oled_write_ln("", false);

        // Print Software Version
        oled_write_P(PSTR("v3.14"), false);
        oled_write_ln("", false);
      } else {
        oled_clear();
        // Print WPM information
        uint8_t wpm = get_current_wpm();
        oled_write_P(PSTR("WPM--\n "), false);
        if (wpm < 2) {
          oled_write("000", false);
        } else {
          oled_write(get_u8_str(wpm, '0'), false);
        }
        oled_write_ln("", false);

        // Print StatTrak information
        oled_write_P(PSTR("\nA----\n "), false);
        oled_write(format_number_grouped(keystroke_count), false);
        oled_write_ln("", false);
        oled_write_P(PSTR("\nB----\n "), false);
        oled_write(format_number_grouped(keystroke_count_b), false);
        oled_write_ln("", false);
      }
      return false;
    }
    // If the oled is on, but has been idle for longer than the screensaver time, turn the OLED off
    if(is_oled_on() && last_input_activity_elapsed() > OLED_LONG_TIMEOUT) {
      if (!oled_cleared) {
        print_blank();
        return false;
      }
      oled_off();
      oled_screensaver_active = false;
    // If the OLED is on, but has been idle for a while, turn the screensaver on
    } else if(is_oled_on() && last_input_activity_elapsed() > OLED_SHORT_TIMEOUT) {
      oled_screensaver_active = true;
    }
    // If idle, render the animation.
    if (oled_screensaver_active) {
      // Use some render animation
      oled_cleared = false; // Reset cleared state for animation
      render_animation();
      return false;
    }
    return false;
  }
#endif
