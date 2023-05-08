#pragma once

namespace CE
{

    enum KeyCode : u32
    {
        KEY_0 = '0',
        KEY_1 = '1',
        KEY_2 = '2',
        KEY_3 = '3',
        KEY_4 = '4',
        KEY_5 = '5',
        KEY_6 = '6',
        KEY_7 = '7',
        KEY_8 = '8',
        KEY_9 = '9',
        KEY_EQUAL = '=',
        KEY_MINUS = '-',
        KEY_ASTERISK = '*',
        KEY_FORWARD_SLASH = '/',
        KEY_BACKWARD_SLASH = '\\',
        KEY_LEFT_BRACE = '[',
        KEY_RIGHT_BRACE = ']',
        KEY_A = 'a',
        KEY_B = 'b',
        KEY_C = 'c',
        KEY_D = 'd',
        KEY_E = 'e',
        KEY_F = 'f',
        KEY_G = 'g',
        KEY_H = 'h',
        KEY_I = 'i',
        KEY_J = 'j',
        KEY_K = 'k',
        KEY_L = 'l',
        KEY_M = 'm',
        KEY_N = 'n',
        KEY_O = 'o',
        KEY_P = 'p',
        KEY_Q = 'q',
        KEY_R = 'r',
        KEY_S = 's',
        KEY_T = 't',
        KEY_U = 'u',
        KEY_V = 'v',
        KEY_W = 'w',
        KEY_X = 'x',
        KEY_Y = 'y',
        KEY_Z = 'z',
        KEY_SPACE,
        KEY_ENTER,
        KEY_BACKSPACE,
        KEY_ARRAY_UP,
        KEY_ARROW_LEFT,
        KEY_ARROW_DOWN,
        KEY_ARROW_RIGHT,
        KEY_NUM_0,
        KEY_NUM_1,
        KEY_NUM_2,
        KEY_NUM_3,
        KEY_NUM_4,
        KEY_NUM_5,
        KEY_NUM_6,
        KEY_NUM_7,
        KEY_NUM_8,
        KEY_NUM_9,
    };
    ENUM_CLASS_FLAGS(KeyCode);

    FORCE_INLINE bool KeyCode_IsCharacter(KeyCode code)
    {
        return (code >= KEY_A && code <= KEY_Z) || (code >= KEY_0 && code <= KEY_9) || (code >= KEY_NUM_0 && code <= KEY_NUM_9) ||
            code == KEY_EQUAL || code == KEY_MINUS;
    }

    class ApplicationMessageHandler
    {
    public:

        virtual ~ApplicationMessageHandler() = default;

        virtual void OnKeyDown(KeyCode keyCode, char character)
        {
            
        }

        virtual void OnKeyUp(KeyCode keyCode, char character)
        {

        }
    };
    
} // namespace CE

