#ifndef _INCLUDE_ALGORITHM_H
#define _INCLUDE_ALGORITHM_H

// 错误号定义
/*! \enum name
 *
 *  Detailed description
 */
enum error_t {
    ERROR_MALLOC,
    ERROR_ILLEGAL_PARAMETERS,
    ERROR_BAND_IS_NOT_GRAY,
    ERROR_BAND_IS_NOT_COLOR
};
extern uint8_t error_num;

#define SUCCESS     0
#define FAILED      -1

//定义像素个数，如果为rgb需要乘3
#define PIXEL_300DPI     (2592 * 3508)
#define PIXEL_600DPI     (PIXEL_300DPI * 4)
#define PIXEL_1200DPI    (PIXEL_600DPI * 4)

#endif  //_INCLUDE_ALGORITHM_H