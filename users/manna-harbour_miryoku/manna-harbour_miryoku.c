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

  // Function to save the keystroke count to EEPROM
  void save_keystroke_count(void) {
    eeprom_update_block((const void*)&keystroke_count, (void*)KEYSTROKE_EEPROM_ADDR, sizeof(keystroke_count));
  }

  // Function to increment the keystroke count
  bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
      keystroke_count++;
      save_keystroke_count();
    }
    return true;
  }

  void stat_trak_sub_handler(uint8_t in_buflen, const void* in_data, uint8_t out_buflen, void* out_data) {
    const uint32_t* in_keystroke_count = (const uint32_t*)in_data;
    keystroke_count = *in_keystroke_count;
  }

  void housekeeping_task_user(void) {
    if (is_keyboard_master()) {
      // Interact with sub every 500ms
      static uint32_t last_sync = 0;
      if (timer_elapsed32(last_sync) > 500) {
        transaction_rpc_send(STAT_TRAK, sizeof(keystroke_count), &keystroke_count);
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
    load_keystroke_count(); // Your EEPROM or variable setup
    transaction_register_rpc(STAT_TRAK, stat_trak_sub_handler);
  }

  // OLED task to display information
  bool oled_task_user(void) {
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
        if (keymap_config.swap_lctl_lgui || keymap_config.swap_rctl_rgui) {
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
        oled_write_P(PSTR("v2.5"), false);
        oled_write_ln("", false);
    } else {
        oled_clear();

        // Print WPM information
        oled_write_P(PSTR("WPM: \n"), false);
        oled_write(get_u8_str(get_current_wpm(), '0'), false);
        oled_write_ln("\n", false);

        // Print StatTrak information
        oled_write_P(PSTR("\nStat Trak:\n"), false);
        oled_write(format_number_grouped(keystroke_count), false);
        oled_write_ln("\n", false);
    }

    return false;
  }
#endif
