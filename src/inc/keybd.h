#ifndef KEYBD_H
#define KEYBD_H

#define KB_STAT 0x64
#define KB_DATA 0x60

#define KB_STAT_DIB 0x1 /* got data in buffer. */

// Special keycodes
#define KEY_HOME        0xE0
#define KEY_END         0xE1
#define KEY_UP          0xE2
#define KEY_DN          0xE3
#define KEY_LF          0xE4
#define KEY_RT          0xE5
#define KEY_PGUP        0xE6
#define KEY_PGDN        0xE7
#define KEY_INS         0xE8
#define KEY_DEL         0xE9

static uchar keybd_map[256] = {
  NUL,  0x1B, '1',  '2',  '3',  '4',  '5',  '6',  // 0x00
  '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
  'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',  // 0x10
  'o',  'p',  '[',  ']',  '\n', NUL,   'a',  's',
  'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',  // 0x20
  '\'', '`',  NUL,  '\\', 'z',  'x',  'c',  'v',
  'b',  'n',  'm',  ',',  '.',  '/',  NUL,  '*',  // 0x30
  NUL,  ' ',  NUL,  NUL,  NUL,  NUL,  NUL,  NUL,
  NUL,  NUL,  NUL,  NUL,  NUL,  NUL,  NUL,  '7',  // 0x40
  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',
  '2',  '3',  '0',  '.',  NUL,  NUL,  NUL,  NUL,   // 0x50
  [0x9C] '\n',      // KP_Enter
  [0xB5] '/',       // KP_Div
  [0xC8] KEY_UP,    [0xD0] KEY_DN,
  [0xC9] KEY_PGUP,  [0xD1] KEY_PGDN,
  [0xCB] KEY_LF,    [0xCD] KEY_RT,
  [0x97] KEY_HOME,  [0xCF] KEY_END,
  [0xD2] KEY_INS,   [0xD3] KEY_DEL
};


#endif
