// Copyright 2019 Manna Harbour
// https://github.com/manna-harbour/miryoku

// This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#define SPLIT_WPM_ENABLE

#define XXX KC_NO

// FPS friendly tap layer
#define MIRYOKU_LAYER_GAME \
KC_TAB,            KC_Q,              KC_W,             KC_E,             KC_R,              KC_Y,             KC_U,             KC_I,             KC_O,              KC_P,             \
KC_LSFT,           KC_A,              KC_S,             KC_D,             KC_F,              KC_H,             KC_J,             KC_K,             KC_L,              KC_QUOT,          \
KC_LCTL,           KC_Z,              KC_X,             KC_C,             KC_V,              KC_N,             KC_M,             KC_COMMA,         KC_DOT,            KC_SLASH,         \
U_NP,              U_NP,              KC_LALT,          KC_SPC,           MO(U_GAMENUM),     KC_ENT,           KC_BSPC,          KC_DEL,           U_NP,              U_NP

#define MIRYOKU_LAYER_GAMENUM \
KC_ESC,            KC_1,              KC_2,              KC_3,             KC_T,             KC_LBRC,          KC_F7,            KC_F8,            KC_F9,             KC_RBRC,         \
KC_LSFT,           KC_4,              KC_5,              KC_6,             KC_G,             KC_EQL,           KC_F4,            KC_F5,            KC_F6,             KC_SCLN,         \
KC_LCTL,           KC_7,              KC_8,              KC_9,             KC_B,             KC_BSLS,          KC_F1,            KC_F2,            KC_F3,             KC_GRAVE,        \
U_NP,              U_NP,              DF(U_BASE),        KC_SPC,           KC_NO,            KC_MINUS,         KC_0,             KC_DOT,           U_NP,              U_NP

#define MIRYOKU_LAYER_BASE \
KC_Q,              KC_W,              KC_E,              KC_R,              KC_T,              KC_Y,              KC_U,              KC_I,              KC_O,              KC_P,              \
LCTL_T(KC_A),      LALT_T(KC_S),      LGUI_T(KC_D),      LSFT_T(KC_F),      KC_G,              KC_H,              LSFT_T(KC_J),      LGUI_T(KC_K),      LALT_T(KC_L),      LCTL_T(KC_QUOT),   \
LT(U_BUTTON,KC_Z), ALGR_T(KC_X),      KC_C,              KC_V,              KC_B,              KC_N,              KC_M,              KC_COMM,           ALGR_T(KC_DOT),    LT(U_BUTTON,KC_SLSH),\
U_NP,              U_NP,              LT(U_MEDIA,KC_ESC),LT(U_NAV,KC_SPC),  LT(U_MOUSE,KC_TAB),LT(U_SYM,KC_ENT),  LT(U_NUM,KC_BSPC), LT(U_FUN,KC_DEL),  U_NP,              U_NP

#define MIRYOKU_LAYER_MOUSE \
TD(U_TD_BOOT),     TD(U_TD_U_TAP),    TD(U_TD_U_EXTRA),  TD(U_TD_U_BASE),   U_NA,              U_RDO,             U_PST,             U_CPY,             U_CUT,             U_UND,             \
KC_LGUI,           KC_LALT,           KC_LCTL,           KC_LSFT,           U_NA,              U_NU,              KC_MS_L,           KC_MS_D,           KC_MS_U,           KC_MS_R,           \
U_NA,              KC_ALGR,           TD(U_TD_U_SYM),    TD(U_TD_U_MOUSE),  U_NA,              U_NU,              KC_WH_L,           KC_WH_D,           KC_WH_U,           KC_WH_R,           \
U_NP,              U_NP,              DF(U_GAME),        U_NA,              U_NA,              KC_BTN2,           KC_BTN1,           KC_BTN3,           U_NP,              U_NP

#define MIRYOKU_LAYER_LIST \
MIRYOKU_X(BASE,   "Base") \
MIRYOKU_X(EXTRA,  "Extra") \
MIRYOKU_X(TAP,    "Tap") \
MIRYOKU_X(BUTTON, "Button") \
MIRYOKU_X(NAV,    "Nav") \
MIRYOKU_X(MOUSE,  "Mouse") \
MIRYOKU_X(MEDIA,  "Media") \
MIRYOKU_X(NUM,    "Num") \
MIRYOKU_X(SYM,    "Sym") \
MIRYOKU_X(FUN,    "Fun") \
MIRYOKU_X(GAME,   "Game") \
MIRYOKU_X(GAMENUM,"GNum")

#define MIRYOKU_LAYERMAPPING_GAME( \
      K00,  K01,  K02,  K03,  K04,         K05,  K06,  K07,  K08,  K09, \
      K10,  K11,  K12,  K13,  K14,         K15,  K16,  K17,  K18,  K19, \
      K20,  K21,  K22,  K23,  K24,         K25,  K26,  K27,  K28,  K29, \
      N30,  N31,  K32,  K33,  K34,         K35,  K36,  K37,  N38,  N39 \
) \
LAYOUT_split_3x5_3( \
K00,  K01,  K02,  K03,  K04,         K05,  K06,  K07,  K08,  K09, \
K10,  K11,  K12,  K13,  K14,         K15,  K16,  K17,  K18,  K19, \
K20,  K21,  K22,  K23,  K24,         K25,  K26,  K27,  K28,  K29, \
            K32,  K33,  K34,         K35,  K36,  K37 \
)

#define MIRYOKU_LAYERMAPPING_GAMENUM( \
    K00,  K01,  K02,  K03,  K04,         K05,  K06,  K07,  K08,  K09, \
    K10,  K11,  K12,  K13,  K14,         K15,  K16,  K17,  K18,  K19, \
    K20,  K21,  K22,  K23,  K24,         K25,  K26,  K27,  K28,  K29, \
    N30,  N31,  K32,  K33,  K34,         K35,  K36,  K37,  N38,  N39 \
) \
LAYOUT_split_3x5_3( \
K00,  K01,  K02,  K03,  K04,         K05,  K06,  K07,  K08,  K09, \
K10,  K11,  K12,  K13,  K14,         K15,  K16,  K17,  K18,  K19, \
K20,  K21,  K22,  K23,  K24,         K25,  K26,  K27,  K28,  K29, \
            K32,  K33,  K34,         K35,  K36,  K37 \
)
