
/*******************************************************************************
* generated by lcd-image-converter rev.030b30d from 2019-03-17 01:38:34 +0500
* image
* filename: C:/LW/bmp/lcd-image-converter/img_left.xml
* name: img_left
*
* preset name: Монохромное
* data block size: 8 bit(s), uint8_t
* RLE compression enabled: no
* conversion type: Monochrome, Diffuse Dither 128
* split to rows: no
* bits per pixel: 1
*
* preprocess:
*  main scan direction: top_to_bottom
*  line scan direction: forward
*  inverse: no
*******************************************************************************/

/*
 typedef struct {
     const uint8_t *data;
     uint16_t width;
     uint16_t height;
     uint8_t dataSize; , 8
     } tImage;
*/
#include <stdint.h>
#include "bmp.h"




static const uint8_t image_data_img_left[11] = {
    // #####∙∙∙
    // ####∙∙∙#
    // ###∙∙∙##
    // ##∙∙∙###
    // #∙∙∙####
    // ∙∙∙#####
    // #∙∙∙####
    // ##∙∙∙###
    // ###∙∙∙##
    // ####∙∙∙#
    // #####∙∙∙
    0x07, 0x0e, 0x1c, 0x38, 0x70, 0xe0, 0x70, 0x38, 0x1c, 0x0e, 0x07
};
const tImage img_left = { image_data_img_left, 8, 11};
