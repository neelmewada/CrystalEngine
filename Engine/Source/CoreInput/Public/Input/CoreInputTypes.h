#pragma once

#define SCANCODE_MASK (1<<30)
#define SDL_SCANCODE_TO_KEYCODE(X)  (X | SCANCODE_MASK)

namespace CE
{
    ENUM()
    enum class KeyCode : s32
    {
        Unknown = 0,
        Return = '\r',
        Escape = '\x1B',
        Backspace = '\b',
        Tab = '\t',
        Space = ' ',
        Exclaim = '!',
        DoubleQuote = '"',
        Hash = '#',
        Percent = '%',
        Dollar = '$',
        Ampersand = '&',
        Quote = '\'',
        LeftParen = '(',
        RightParen = ')',
        Asterisk = '*',
        Plus = '+',
        Comma = ',',
        Minus = '-',
        Period = '.',
        Slash = '/',
        N0 = '0',
        N1 = '1',
        N2 = '2',
        N3 = '3',
        N4 = '4',
        N5 = '5',
        N6 = '6',
        N7 = '7',
        N8 = '8',
        N9 = '9',
        Colon = ':',
        Semicolon = ';',
        Less = '<',
        Equals = '=',
        Greater = '>',
        Question = '?',
        At = '@',

        LeftBracket = '[',
        BackSlash = '\\',
        RightBracket = ']',
        Caret = '^',
        Underscore = '_',
        Backquote = '`',
        A = 'a',
        B = 'b',
        C = 'c',
        D = 'd',
        E = 'e',
        F = 'f',
        G = 'g',
        H = 'h',
        I = 'i',
        J = 'j',
        K = 'k',
        L = 'l',
        M = 'm',
        N = 'n',
        O = 'o',
        P = 'p',
        Q = 'q',
        R = 'r',
        S = 's',
        T = 't',
        U = 'u',
        V = 'v',
        W = 'w',
        X = 'x',
        Y = 'y',
        Z = 'z',

        Capslock = (57 | (1 << 30)),

        F1 = (58 | (1 << 30)),
        F2 = (59 | (1 << 30)),
        F3 = (60 | (1 << 30)),
        F4 = (61 | (1 << 30)),
        F5 = (62 | (1 << 30)),
        F6 = (63 | (1 << 30)),
        F7 = (64 | (1 << 30)),
        F8 = (65 | (1 << 30)),
        F9 = (66 | (1 << 30)),
        F10 = (67 | (1 << 30)),
        F11 = (68 | (1 << 30)),
        F12 = (69 | (1 << 30)),

        NumlockClear = (83 | (1 << 30)),
        KPDivide = (84 | (1 << 30)),
        KPMultiply = (85 | (1 << 30)),
        KPMinus = (86 | (1 << 30)),
        KPPlus = (87 | (1 << 30)),
        KPEnter = (88 | (1 << 30)),
        KP1 = (89 | (1 << 30)),
        KP2 = (90 | (1 << 30)),
        KP3 = (91 | (1 << 30)),
        KP4 = (92 | (1 << 30)),
        KP5 = (93 | (1 << 30)),
        KP6 = (94 | (1 << 30)),
        KP7 = (95 | (1 << 30)),
        KP8 = (96 | (1 << 30)),
        KP9 = (97 | (1 << 30)),
        KP0 = (98 | (1 << 30)),
        KPPeriod = (99 | (1 << 30)),

        LCtrl = (224 | (1 << 30)),
        LShift = (225 | (1 << 30)),
        LAlt = (226 | (1 << 30)),
        LGui = (227 | (1 << 30)),
        RCtrl = (228 | (1 << 30)),
        RShift = (229 | (1 << 30)),
        RAlt = (230 | (1 << 30)),
        RGui = (231 | (1 << 30)),

    };
    ENUM_CLASS(KeyCode);

    ENUM(Flags)
    enum class KeyModifier : u16
    {
        None = 0x0000,
        LShift = 0x0001,
        RShift = 0x0002,
        LCtrl = 0x0040,
        RCtrl = 0x0080,
        LAlt = 0x0100,
        RAlt = 0x0200,
        LGui = 0x0400,
        RGui = 0x0800,
        Num = 0x1000,
        Caps = 0x2000,
        Mode = 0x4000,
        Scroll = 0x8000,

        Ctrl = LCtrl | RCtrl,
        Shift = LShift | RShift,
        Alt = LAlt | RAlt,
        Gui = LGui | RGui,
    };
    ENUM_CLASS_FLAGS(KeyModifier);

    ENUM()
    enum class MouseButton
    {
        None = 0,
        Left = 1,
        Middle = 2,
        Right = 3,
        Button4 = 4,
        Button5 = 5
    };
    ENUM_CLASS(MouseButton);
    
} // namespace CE

#include "CoreInputTypes.rtti.h"