/**
 * MIT License
 *
 * Copyright (c) 2025 Sahil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file window-bank.h
 * @brief This file provides single header implementation of window bank
 * covering various window functions:
 *  - Hanning Window
 *  - Hamming Window
 *  - Blackman Window
 *  - Blackman-Harris Window
 */

#ifndef WINDOW_BANK_H_
#define WINDOW_BANK_H_

/**
 * @brief Type used for window values (can be changed to float, long double,
 * etc.).
 */
#ifndef WTYPE
#define WTYPE double
#endif // WTYPE

/**
 * @brief Pi constant used in window calculations.
 */
#ifndef PI
#define PI 3.14159265358979323846
#endif // PI

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/**
 * @enum WinType
 * @brief Enum representing different types of window functions.
 */
typedef enum WinType {
    HANNING,        /**< Hanning window */
    HAMMING,        /**< Hamming window */
    BLACKMAN,       /**< Blackman window */
    BLACKMAN_HARRIS /**< Blackman-Harris window */
} WinType;

/**
 * @struct Window
 * @brief Represents a single window of a given size and type.
 */
typedef struct Window {
    int size;      /**< Number of elements in the window */
    WTYPE *values; /**< Pointer to array of window values */
    WinType type;  /**< Type of the window function */
} Window;

/**
 * @struct WindowBank
 * @brief Holds multiple windows of the same type with varying sizes.
 */
typedef struct WindowBank {
    int count;      /**< Number of windows */
    Window **pwins; /**< Array of pointers to Window structures */
    WinType type;   /**< Type of all windows in the bank */
} WindowBank;

/**
 * @brief Fill a Hanning window of given size.
 * @param size Size of the window.
 * @param out Output array of size `size`.
 * @return 0 on success, -1 on error.
 */
int fill_hanning(int size, WTYPE *out);

/**
 * @brief Fill a Hamming window of given size.
 * @param size Size of the window.
 * @param out Output array of size `size`.
 * @return 0 on success, -1 on error.
 */
int fill_hamming(int size, WTYPE *out);

/**
 * @brief Fill a Blackman window of given size.
 * @param size Size of the window.
 * @param out Output array of size `size`.
 * @return 0 on success, -1 on error.
 */
int fill_blakman(int size, WTYPE *out);

/**
 * @brief Fill a Blackman-Harris window of given size.
 * @param size Size of the window.
 * @param out Output array of size `size`.
 * @return 0 on success, -1 on error.
 */
int fill_blkmhar(int size, WTYPE *out);

/**
 * @brief Initialize a Window structure.
 * @param size Size of the window.
 * @param type Window function type.
 * @return Pointer to initialized Window on success, NULL on failure.
 */
Window *window_init(int size, WinType type);

/**
 * @brief Free memory associated with a Window structure.
 * @param pwin Pointer to Window to free.
 */
void window_deinit(Window *pwin);

/**
 * @brief Fill the window values according to its type.
 * @param pwin Pointer to a valid Window structure.
 * @return 0 on success, -1 on error.
 */
int window_fill(Window *pwin);

/**
 * @brief Initialize a WindowBank structure.
 * @param count Number of windows.
 * @param sizes Array of sizes for each window.
 * @param type Window function type.
 * @return Pointer to WindowBank on success, NULL on failure.
 */
WindowBank *window_bank_init(int count, int *sizes, WinType type);

/**
 * @brief Free memory associated with a WindowBank and its windows.
 * @param pwb Pointer to WindowBank to free.
 */
void window_bank_deinit(WindowBank *pwb);

/**
 * @brief Fill all windows in a WindowBank with values.
 * @param pwb Pointer to initialized WindowBank.
 * @return 0 on success, -1 on failure.
 */
int window_bank_fill(WindowBank *pwb);

/**
 * @brief Get a window of a specific size from a WindowBank.
 * @param pwb Pointer to WindowBank.
 * @param size Desired window size.
 * @return Pointer to Window with matching size, or NULL if not found.
 */
Window *window_bank_get(WindowBank *pwb, int size);

/**
 * @brief Fill a Hanning window of given size.
 */
int fill_hanning(int size, WTYPE *out) {
    if (out == NULL)
        return -1;

    assert(size > 1);

    for (int i = 0; i < size; i++) {
        out[i] = 0.5 - 0.5 * cos(2.0 * PI * i / (size - 1));
    }

    return 0;
}

/**
 * @brief Fill a Hamming window of given size.
 */
int fill_hamming(int size, WTYPE *out) {
    if (out == NULL)
        return -1;

    assert(size > 1);

    for (int i = 0; i < size; i++) {
        out[i] = (25.0 / 46.0) - (21.0 / 46.0) * cos(2.0 * PI * i / (size - 1));
    }

    return 0;
}

/**
 * @brief Fill a Blackman window using standard Blackman coefficients.
 */
int fill_blakman(int size, WTYPE *out) {
    if (out == NULL)
        return -1;

    assert(size > 1);

    const WTYPE a0 = 7938.0 / 18608.0;
    const WTYPE a1 = 9240.0 / 18608.0;
    const WTYPE a2 = 1430.0 / 18608.0;

    for (int i = 0; i < size; i++) {
        out[i] = a0 - a1 * cos(2.0 * PI * i / (size - 1)) +
                 a2 * cos(4.0 * PI * i / (size - 1));
    }

    return 0;
}

/**
 * @brief Fill a Blackman-Harris window using standard coefficients.
 */
int fill_blkmhar(int size, WTYPE *out) {
    if (out == NULL)
        return -1;

    assert(size > 1);

    const WTYPE a0 = 0.35875;
    const WTYPE a1 = 0.48829;
    const WTYPE a2 = 0.14128;
    const WTYPE a3 = 0.01168;

    for (int i = 0; i < size; i++) {
        out[i] = a0 - a1 * cos(2.0 * PI * i / (size - 1)) +
                 a2 * cos(4.0 * PI * i / (size - 1)) +
                 a3 * cos(6.0 * PI * i / (size - 1));
    }

    return 0;
}

/**
 * @brief Allocate and initialize a window of given size and type.
 */
Window *window_init(int size, WinType type) {
    if (size <= 0)
        return NULL;
    Window *pwin = (Window *)malloc(sizeof(Window));
    if (pwin == NULL)
        return NULL;
    pwin->size = size;
    pwin->type = type;
    pwin->values = (WTYPE *)malloc(sizeof(WTYPE) * size);
    if (pwin->values == NULL) {
        free(pwin);
        return NULL;
    }
    return pwin;
}

/**
 * @brief Deinitialize a window and free associated memory.
 */
void window_deinit(Window *pwin) {
    if (pwin) {
        if (pwin->values) {
            free(pwin->values);
        }
        free(pwin);
    }
}

/**
 * @brief Fill window values using its specified type.
 */
int window_fill(Window *pwin) {
    if (pwin) {
        if (pwin->type == HANNING) {
            return fill_hanning(pwin->size, pwin->values);
        } else if (pwin->type == HAMMING) {
            return fill_hamming(pwin->size, pwin->values);
        } else if (pwin->type == BLACKMAN) {
            return fill_blakman(pwin->size, pwin->values);
        } else if (pwin->type == BLACKMAN_HARRIS) {
            return fill_blkmhar(pwin->size, pwin->values);
        }
    }
    return -1;
}

/**
 * @brief Initialize a bank of windows with given sizes and type.
 */
WindowBank *window_bank_init(int count, int *sizes, WinType type) {
    if (count <= 0)
        return NULL;
    WindowBank *pwb = (WindowBank *)malloc(sizeof(WindowBank));
    if (pwb == NULL)
        return NULL;
    pwb->count = count;
    pwb->type = type;
    pwb->pwins = (Window **)malloc(sizeof(Window *) * count);
    if (pwb->pwins == NULL) {
        free(pwb);
        return NULL;
    }
    // initialize the memory with null pointers
    memset(pwb->pwins, 0, sizeof(Window *) * count);
    for (int i = 0; i < count; i++) {
        int size = sizes[i];
        Window *pwin = window_init(size, type);
        if (pwin == NULL) {
            window_bank_deinit(pwb);
            return NULL;
        }
        pwb->pwins[i] = pwin;
    }
    return pwb;
}

/**
 * @brief Deinitialize a window bank and free all windows.
 */
void window_bank_deinit(WindowBank *pwb) {
    if (pwb) {
        if (pwb->pwins) {
            for (int i = 0; i < pwb->count; i++) {
                window_deinit(pwb->pwins[i]);
            }
            free(pwb->pwins);
        }
        free(pwb);
    }
}

/**
 * @brief Fill all windows in the bank with their respective values.
 */
int window_bank_fill(WindowBank *pwb) {
    if (pwb) {
        if (pwb->pwins) {
            for (int i = 0; i < pwb->count; i++) {
                if (window_fill(pwb->pwins[i]) != 0)
                    return -1;
            }
            return 0;
        }
    }
    return -1;
}

/**
 * @brief Retrieve a window of specific size from the window bank.
 */
Window *window_bank_get(WindowBank *pwb, int size) {
    if (pwb) {
        if (pwb->pwins) {
            for (int i = 0; i < pwb->count; i++) {
                Window *pwin = pwb->pwins[i];
                if (pwin) {
                    if (pwin->size == size)
                        return pwin;
                }
            }
        }
    }
    return NULL;
}

#endif // WINDOW_BANK_H_
