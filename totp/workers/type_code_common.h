#pragma once
#include <stdint.h>
#include "../types/token_info.h"
#include "../types/automation_kb_layout.h"

typedef bool (*TOTP_AUTOMATION_KEY_HANDLER)(uint16_t key);
typedef bool (*TOTP_AUTOMATION_KEY_HANDLER_CTX)(void* ctx, uint16_t key);

/**
 * @brief Executes token input automation using given key press\release handlers
 * @param key_press_fn key press handler
 * @param key_release_fn key release handler
 * @param code_buffer code buffer to be typed
 * @param code_buffer_size code buffer size
 * @param features automation features
 * @param keyboard_layout keyboard layout to be used
 * @param initial_delay initial delay before starting automation
 */
void totp_type_code_worker_execute_automation(
    TOTP_AUTOMATION_KEY_HANDLER key_press_fn,
    TOTP_AUTOMATION_KEY_HANDLER key_release_fn,
    const char* code_buffer,
    uint8_t code_buffer_size,
    TokenAutomationFeature features,
    AutomationKeyboardLayout keyboard_layout,
    uint16_t initial_delay);

/**
 * @brief Executes token input automation using given key press\release handlers
 * @param key_press_fn key press handler
 * @param key_release_fn key release handler
 * @param ctx user data context
 * @param code_buffer code buffer to be typed
 * @param code_buffer_size code buffer size
 * @param features automation features
 * @param keyboard_layout keyboard layout to be used
 * @param initial_delay initial delay before starting automation
 */
void totp_type_code_worker_execute_automation_ctx(
    TOTP_AUTOMATION_KEY_HANDLER_CTX key_press_fn,
    TOTP_AUTOMATION_KEY_HANDLER_CTX key_release_fn,
    void* ctx,
    const char* code_buffer,
    uint8_t code_buffer_size,
    TokenAutomationFeature features,
    AutomationKeyboardLayout keyboard_layout,
    uint16_t initial_delay);
