#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <stdbool.h> // Header-file for boolean data-type.
#include <string.h> // Header-file for string functions.
#include "tinyexpr.h" // Header-file for the TinyExpr library.

#include <stdio.h>
#include <stdlib.h>

#define TAG "[c_calculator]" // Used for logging

const short MAX_TEXT_LENGTH = 20;

typedef struct {
    short x;
    short y;
} selectedPosition;

typedef struct {
    FuriMutex* mutex;
    selectedPosition position;
    //string with the inputted calculator text
    char text[20];
    short textLength;
    char log[20];
} Calculator;

char getKeyAtPosition(short x, short y) {
    char keys[6][4] = {
        {'C', '<', '%', '/'},
        {'7', '8', '9', '*'},
        {'4', '5', '6', '-'},
        {'1', '2', '3', '+'},
        {'0', '.', 'm', '^'},
        {'(', 'o', ')', '='}};

    if(x >= 0 && x < 4 && y >= 0 && y < 6) {
        return keys[y][x];
    } else {
        return ' ';
    }
}

short calculateStringWidth(const char* str, short length) {
    /* widths:
        1 = 2
        2, 3, 4, 5, 6, 7, 8, 9, 0, X, -, +,^, ., =,  = 5
        %, / = 7
        S = 5
        (, ) = 3

    */
    short width = 0;
    for(short i = 0; i < length; i++) {
        switch(str[i]) {
        case '1':
            width += 2;
            break;
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
        case '*':
        case '-':
        case '+':
        case '.':
        case '^':
            width += 5;
            break;
        case '%':
        case '/':
            width += 7;
            break;
        // case 'S':
        //     width += 5;
        //     break;
        case '(':
        case ')':
            width += 5;
            break;
        default:
            break;
        }
        width += 1;
    }

    return width;
}

void generate_calculator_layout(Canvas* canvas) {
    //draw dotted lines
    for(int i = 0; i <= 64; i++) {
        if(i % 2 == 0) {
            canvas_draw_dot(canvas, i, 14);
            canvas_draw_dot(canvas, i, 33);
        }
        if(i % 2 == 1) {
            canvas_draw_dot(canvas, i, 15);
            canvas_draw_dot(canvas, i, 34);
        }
    }

    // canvas_draw_box(canvas, x, y, width, height);
    // draw horizontal lines
    canvas_draw_box(canvas, 0, 36, 63, 1);
    canvas_draw_box(canvas, 0, 51, 63, 1);
    canvas_draw_box(canvas, 0, 66, 63, 1);
    canvas_draw_box(canvas, 0, 81, 63, 1);
    canvas_draw_box(canvas, 0, 96, 63, 1);
    canvas_draw_box(canvas, 0, 110, 63, 1);
    canvas_draw_box(canvas, 0, 126, 63, 1);

    // draw vertical lines
    canvas_draw_box(canvas, 0, 36, 1, 91);
    canvas_draw_box(canvas, 16, 36, 1, 91);
    canvas_draw_box(canvas, 32, 36, 1, 91);
    canvas_draw_box(canvas, 48, 36, 1, 91);
    canvas_draw_box(canvas, 63, 36, 1, 91);

    // draw buttons
    // row 0 (C, <, %, ÷)
    canvas_draw_str(canvas, 6, 47, "C");
    canvas_draw_str(canvas, 18, 47, " <-");
    canvas_draw_str(canvas, 35, 47, " %");
    canvas_draw_str(canvas, 51, 47, " /");

    // row 1 (7, 8, 9, X)
    canvas_draw_str(canvas, 4, 62, " 7");
    canvas_draw_str(canvas, 20, 62, " 8");
    canvas_draw_str(canvas, 36, 62, " 9");
    canvas_draw_str(canvas, 52, 62, " X");

    // row 2 (4, 5, 6, -)
    canvas_draw_str(canvas, 4, 77, " 4");
    canvas_draw_str(canvas, 20, 77, " 5");
    canvas_draw_str(canvas, 36, 77, " 6");
    canvas_draw_str(canvas, 52, 77, " -");

    // row 3 (1, 2, 3, +)
    canvas_draw_str(canvas, 5, 92, " 1");
    canvas_draw_str(canvas, 20, 92, " 2");
    canvas_draw_str(canvas, 36, 92, " 3");
    canvas_draw_str(canvas, 52, 92, " +");

    // row 4 (., 0, +/-, ^)
    canvas_draw_str(canvas, 4, 107, " 0");
    canvas_draw_str(canvas, 20, 107, "  .");
    canvas_draw_str(canvas, 35, 107, "+-");
    canvas_draw_str(canvas, 52, 107, " ^");

    // row 5 ((, 00, ), =)
    canvas_draw_str(canvas, 7, 122, "(");
    canvas_draw_str(canvas, 19, 122, "00");
    canvas_draw_str(canvas, 37, 122, " )");
    canvas_draw_str(canvas, 52, 122, " =");
}

void calculator_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    const Calculator* calculator_state = ctx;
    furi_mutex_acquire(calculator_state->mutex, FuriWaitForever);

    canvas_clear(canvas);

    //show selected button
    short startX = 1;

    // short startY = 43;
    short startY = 36;

    canvas_set_color(canvas, ColorBlack);

    canvas_draw_box(
        canvas,
        startX + (calculator_state->position.x) * 16,
        (startY) + (calculator_state->position.y) * 15,
        16,
        15);

    canvas_set_color(canvas, ColorWhite);

    canvas_draw_box(
        canvas,
        startX + (calculator_state->position.x) * 16 + 2,
        (startY) + (calculator_state->position.y) * 15 + 2,
        11,
        11);

    canvas_set_color(canvas, ColorBlack);
    generate_calculator_layout(canvas);

    //draw text
    short stringWidth = calculateStringWidth(calculator_state->text, calculator_state->textLength);
    short startingPosition = 5;
    if(stringWidth > 60) {
        startingPosition += 60 - (stringWidth + 5);
    }
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str(canvas, startingPosition, 28, calculator_state->text);

    //draw cursor
    canvas_draw_box(canvas, stringWidth + 5, 29, 5, 1);

    furi_mutex_release(calculator_state->mutex);
}

void calculator_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

void calculate(Calculator* calculator_state) {
    double result;

    FURI_LOG_I(TAG, "calculator_state->text %s", calculator_state->text);
    result = te_interp(calculator_state->text, 0);

    FURI_LOG_I(TAG, "===> result: %f", result);

    calculator_state->textLength = 0;
    calculator_state->text[0] = '\0';

    //invert sign if negative
    if(result < 0) {
        calculator_state->text[calculator_state->textLength++] = '-';
        result = -result;
    }

    //get numbers before and after decimal
    int beforeDecimal = result;
    int afterDecimal = (result - beforeDecimal) * 100;

    char beforeDecimalString[10];
    char afterDecimalString[10];
    int i = 0;

    //parse to a string
    while(beforeDecimal > 0) {
        beforeDecimalString[i++] = beforeDecimal % 10 + '0';
        beforeDecimal /= 10;
    }

    // invert string
    for(int j = 0; j < i / 2; j++) {
        char temp = beforeDecimalString[j];
        beforeDecimalString[j] = beforeDecimalString[i - j - 1];
        beforeDecimalString[i - j - 1] = temp;
    }

    //add it to the answer
    for(int j = 0; j < i; j++) {
        calculator_state->text[calculator_state->textLength++] = beforeDecimalString[j];
    }

    i = 0;

    if(afterDecimal > 0) {
        while(afterDecimal > 0) {
            afterDecimalString[i++] = afterDecimal % 10 + '0';
            afterDecimal /= 10;
        }

        // invert string
        for(int j = 0; j < i / 2; j++) {
            char temp = afterDecimalString[j];
            afterDecimalString[j] = afterDecimalString[i - j - 1];
            afterDecimalString[i - j - 1] = temp;
        }

        //add decimal point
        calculator_state->text[calculator_state->textLength++] = '.';

        //add numbers after decimal
        for(int j = 0; j < i; j++) {
            calculator_state->text[calculator_state->textLength++] = afterDecimalString[j];
        }
    }
    calculator_state->text[calculator_state->textLength] = '\0';
}

int32_t calculator_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    Calculator* calculator_state = malloc(sizeof(Calculator));
    calculator_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!calculator_state->mutex) {
        FURI_LOG_E("calculator", "cannot create mutex\r\n");
        free(calculator_state);
        return -1;
    }

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, calculator_draw_callback, calculator_state);
    view_port_input_callback_set(view_port, calculator_input_callback, event_queue);
    view_port_set_orientation(view_port, ViewPortOrientationVertical);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    //NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);

    InputEvent event;

    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        //break out of the loop if the back key is pressed
        if(event.type == InputTypeShort && event.key == InputKeyBack) {
            break;
        }

        if(event.type == InputTypeShort) {
            switch(event.key) {
            case InputKeyUp:
                if(calculator_state->position.y > 0) {
                    calculator_state->position.y--;
                }
                break;
            case InputKeyDown:
                FURI_LOG_I(TAG, "position.y: %d", calculator_state->position.y);
                if(calculator_state->position.y < 5) {
                    calculator_state->position.y++;
                }
                break;
            case InputKeyLeft:
                if(calculator_state->position.x > 0) {
                    calculator_state->position.x--;
                }
                break;
            case InputKeyRight:
                if(calculator_state->position.x < 3) {
                    calculator_state->position.x++;
                }
                break;
            case InputKeyOk: {
                //add the selected button to the text
                FURI_LOG_I(
                    TAG,
                    "position.x: %d; position.y: %d",
                    calculator_state->position.x,
                    calculator_state->position.y);
                char key =
                    getKeyAtPosition(calculator_state->position.x, calculator_state->position.y);
                switch(key) {
                case 'C':
                    while(calculator_state->textLength > 0) {
                        calculator_state->text[calculator_state->textLength--] = '\0';
                    }
                    calculator_state->text[0] = '\0';
                    calculator_state->log[2] = key;
                    break;
                case '<':
                    calculator_state->log[2] = key;
                    if(calculator_state->textLength > 0) {
                        calculator_state->text[--calculator_state->textLength] = '\0';
                    } else {
                        calculator_state->text[0] = '\0';
                    }
                    break;
                case '=':
                    calculator_state->log[2] = key;
                    FURI_LOG_I(TAG, "log[2] %d", calculator_state->log[2]);
                    // Log calculator_state
                    calculate(calculator_state);
                    break;
                case '%':
                case '/':
                case '*':
                case '-':
                case '+':
                case '^':
                case '.':
                case '(':
                case ')':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '0':
                    if(calculator_state->textLength < MAX_TEXT_LENGTH) {
                        calculator_state->text[calculator_state->textLength++] = key;
                        calculator_state->text[calculator_state->textLength] = '\0';
                    }
                    break;
                case 'm':
                    if(calculator_state->textLength < MAX_TEXT_LENGTH) {
                        if(calculator_state->text[0] == '-') {
                            // If the number is negative, remove the '-' to make it positive
                            memmove(
                                calculator_state->text,
                                calculator_state->text + 1,
                                calculator_state->textLength);
                            calculator_state->textLength--;
                        } else {
                            // If the number is positive, prepend a '-' to make it negative
                            memmove(
                                calculator_state->text + 1,
                                calculator_state->text,
                                calculator_state->textLength + 1);
                            calculator_state->text[0] = '-';
                            calculator_state->textLength++;
                        }
                        calculator_state->text[calculator_state->textLength] = '\0';
                    }
                    FURI_LOG_I(TAG, "state text: %s", calculator_state->text);
                    break;
                case 'o':
                    if(calculator_state->textLength < MAX_TEXT_LENGTH - 1) {
                        calculator_state->text[calculator_state->textLength++] = '0';
                        calculator_state->text[calculator_state->textLength++] = '0';
                        calculator_state->text[calculator_state->textLength] = '\0';
                    }
                    break;
                default:
                    break;
                }
            }
            default:
                break;
            }
            view_port_update(view_port);
        }

        if(event.type == InputTypeLong) {
            switch(event.key) {
            case InputKeyOk:
                if(calculator_state->position.x == 0 && calculator_state->position.y == 4) {
                    if(calculator_state->textLength < MAX_TEXT_LENGTH) {
                        calculator_state->text[calculator_state->textLength++] = ')';
                        calculator_state->text[calculator_state->textLength] = '\0';
                    }
                    view_port_update(view_port);
                }
                break;
            default:
                break;
            }
        }
    }
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_mutex_free(calculator_state->mutex);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    free(calculator_state);

    return 0;
}
