#include "lvgl.h"
#include "jerryscript-core.h"
#include "jerryscript-ext/handler.h"
#include "lvgl-common.h"

STYLE_FUNC(width, lvgl_obj_native_info, 0);
STYLE_FUNC(min_width, lvgl_obj_native_info, 0);
STYLE_FUNC(max_width, lvgl_obj_native_info, 0);
STYLE_FUNC(height, lvgl_obj_native_info, 0);
STYLE_FUNC(min_height, lvgl_obj_native_info, 0);
STYLE_FUNC(max_height, lvgl_obj_native_info, 0);
STYLE_FUNC(x, lvgl_obj_native_info, 0);
STYLE_FUNC(y, lvgl_obj_native_info, 0);
STYLE_FUNC(align, lvgl_obj_native_info, 0);
STYLE_FUNC(transform_width, lvgl_obj_native_info, 0);
STYLE_FUNC(transform_height, lvgl_obj_native_info, 0);
STYLE_FUNC(translate_x, lvgl_obj_native_info, 0);
STYLE_FUNC(translate_y, lvgl_obj_native_info, 0);
STYLE_FUNC(transform_zoom, lvgl_obj_native_info, 0);
STYLE_FUNC(transform_angle, lvgl_obj_native_info, 0);
STYLE_FUNC(pad_top, lvgl_obj_native_info, 0);
STYLE_FUNC(pad_bottom, lvgl_obj_native_info, 0);
STYLE_FUNC(pad_left, lvgl_obj_native_info, 0);
STYLE_FUNC(pad_right, lvgl_obj_native_info, 0);
STYLE_FUNC(pad_row, lvgl_obj_native_info, 0);
STYLE_FUNC(pad_column, lvgl_obj_native_info, 0);
STYLE_FUNC(radius, lvgl_obj_native_info, 0);
STYLE_FUNC(clip_corner, lvgl_obj_native_info, 0);
STYLE_FUNC(opa, lvgl_obj_native_info, 0);
STYLE_FUNC(color_filter_opa, lvgl_obj_native_info, 0);
STYLE_FUNC(anim_time, lvgl_obj_native_info, 0);
STYLE_FUNC(anim_speed, lvgl_obj_native_info, 0);
STYLE_FUNC(blend_mode, lvgl_obj_native_info, 0);
STYLE_FUNC(layout, lvgl_obj_native_info, 0);
STYLE_FUNC(base_dir, lvgl_obj_native_info, 0);
STYLE_FUNC(bg_color, lvgl_obj_native_info, 1);
STYLE_FUNC(bg_color_filtered, lvgl_obj_native_info, 1);
STYLE_FUNC(bg_opa, lvgl_obj_native_info, 0);
STYLE_FUNC(bg_grad_color, lvgl_obj_native_info, 1);
STYLE_FUNC(bg_grad_color_filtered, lvgl_obj_native_info, 1);
STYLE_FUNC(bg_grad_dir, lvgl_obj_native_info, 0);
STYLE_FUNC(bg_main_stop, lvgl_obj_native_info, 0);
STYLE_FUNC(bg_grad_stop, lvgl_obj_native_info, 0);
STYLE_FUNC(bg_img_opa, lvgl_obj_native_info, 0);
STYLE_FUNC(bg_img_recolor, lvgl_obj_native_info, 1);
STYLE_FUNC(bg_img_recolor_filtered, lvgl_obj_native_info, 1);
STYLE_FUNC(bg_img_recolor_opa, lvgl_obj_native_info, 0);
STYLE_FUNC(bg_img_tiled, lvgl_obj_native_info, 0);
STYLE_FUNC(border_color, lvgl_obj_native_info, 1);
STYLE_FUNC(border_color_filtered, lvgl_obj_native_info, 1);
STYLE_FUNC(border_opa, lvgl_obj_native_info, 0);
STYLE_FUNC(border_width, lvgl_obj_native_info, 0);
STYLE_FUNC(border_side, lvgl_obj_native_info, 0);
STYLE_FUNC(border_post, lvgl_obj_native_info, 0);
STYLE_FUNC(text_color, lvgl_obj_native_info, 1);
STYLE_FUNC(text_color_filtered, lvgl_obj_native_info, 1);
STYLE_FUNC(text_opa, lvgl_obj_native_info, 0);
STYLE_FUNC(text_letter_space, lvgl_obj_native_info, 0);
STYLE_FUNC(text_line_space, lvgl_obj_native_info, 0);
STYLE_FUNC(text_decor, lvgl_obj_native_info, 0);
STYLE_FUNC(text_align, lvgl_obj_native_info, 0);
STYLE_FUNC(img_opa, lvgl_obj_native_info, 0);
STYLE_FUNC(img_recolor, lvgl_obj_native_info, 1);
STYLE_FUNC(img_recolor_filtered, lvgl_obj_native_info, 1);
STYLE_FUNC(img_recolor_opa, lvgl_obj_native_info, 0);
STYLE_FUNC(outline_width, lvgl_obj_native_info, 0);
STYLE_FUNC(outline_color, lvgl_obj_native_info, 1);
STYLE_FUNC(outline_color_filtered, lvgl_obj_native_info, 1);
STYLE_FUNC(outline_opa, lvgl_obj_native_info, 0);
STYLE_FUNC(outline_pad, lvgl_obj_native_info, 0);
STYLE_FUNC(shadow_width, lvgl_obj_native_info, 0);
STYLE_FUNC(shadow_ofs_x, lvgl_obj_native_info, 0);
STYLE_FUNC(shadow_ofs_y, lvgl_obj_native_info, 0);
STYLE_FUNC(shadow_spread, lvgl_obj_native_info, 0);
STYLE_FUNC(shadow_color, lvgl_obj_native_info, 1);
STYLE_FUNC(shadow_color_filtered, lvgl_obj_native_info, 1);
STYLE_FUNC(shadow_opa, lvgl_obj_native_info, 0);
STYLE_FUNC(line_width, lvgl_obj_native_info, 0);
STYLE_FUNC(line_dash_width, lvgl_obj_native_info, 0);
STYLE_FUNC(line_dash_gap, lvgl_obj_native_info, 0);
STYLE_FUNC(line_rounded, lvgl_obj_native_info, 0);
STYLE_FUNC(line_color, lvgl_obj_native_info, 1);
STYLE_FUNC(line_color_filtered, lvgl_obj_native_info, 1);
STYLE_FUNC(line_opa, lvgl_obj_native_info, 0);
STYLE_FUNC(arc_width, lvgl_obj_native_info, 0);
STYLE_FUNC(arc_rounded, lvgl_obj_native_info, 0);
STYLE_FUNC(arc_color, lvgl_obj_native_info, 1);
STYLE_FUNC(arc_color_filtered, lvgl_obj_native_info, 1);
STYLE_FUNC(arc_opa, lvgl_obj_native_info, 0);

static const jerry_function_entry js_obj_prototype_methods[] = {
  JERRY_CFUNC_DEF("setWidth", js_lvgl_obj_set_width),
  JERRY_CFUNC_DEF("setMinWidth", js_lvgl_obj_set_min_width),
  JERRY_CFUNC_DEF("setMaxWidth", js_lvgl_obj_set_max_width),
  JERRY_CFUNC_DEF("setHeight", js_lvgl_obj_set_height),
  JERRY_CFUNC_DEF("setMinHeight", js_lvgl_obj_set_min_height),
  JERRY_CFUNC_DEF("setMaxHeight", js_lvgl_obj_set_max_height),
  JERRY_CFUNC_DEF("setX", js_lvgl_obj_set_x),
  JERRY_CFUNC_DEF("setY", js_lvgl_obj_set_y),
  JERRY_CFUNC_DEF("setAlign", js_lvgl_obj_set_align),
  JERRY_CFUNC_DEF("setTransformWidth", js_lvgl_obj_set_transform_width),
  JERRY_CFUNC_DEF("setTransformHeight", js_lvgl_obj_set_transform_height),
  JERRY_CFUNC_DEF("setTranslateX", js_lvgl_obj_set_translate_x),
  JERRY_CFUNC_DEF("setTranslateY", js_lvgl_obj_set_translate_y),
  JERRY_CFUNC_DEF("setTransformZoom", js_lvgl_obj_set_transform_zoom),
  JERRY_CFUNC_DEF("setTransformAngle", js_lvgl_obj_set_transform_angle),
  JERRY_CFUNC_DEF("setPadTop", js_lvgl_obj_set_pad_top),
  JERRY_CFUNC_DEF("setPadBottom", js_lvgl_obj_set_pad_bottom),
  JERRY_CFUNC_DEF("setPadLeft", js_lvgl_obj_set_pad_left),
  JERRY_CFUNC_DEF("setPadRight", js_lvgl_obj_set_pad_right),
  JERRY_CFUNC_DEF("setPadRow", js_lvgl_obj_set_pad_row),
  JERRY_CFUNC_DEF("setPadColumn", js_lvgl_obj_set_pad_column),
  JERRY_CFUNC_DEF("setRadius", js_lvgl_obj_set_radius),
  JERRY_CFUNC_DEF("setClipCorner", js_lvgl_obj_set_clip_corner),
  JERRY_CFUNC_DEF("setOpa", js_lvgl_obj_set_opa),
  JERRY_CFUNC_DEF("setColorFilterOpa", js_lvgl_obj_set_color_filter_opa),
  JERRY_CFUNC_DEF("setAnimTime", js_lvgl_obj_set_anim_time),
  JERRY_CFUNC_DEF("setAnimSpeed", js_lvgl_obj_set_anim_speed),
  JERRY_CFUNC_DEF("setBlendMode", js_lvgl_obj_set_blend_mode),
  JERRY_CFUNC_DEF("setLayout", js_lvgl_obj_set_layout),
  JERRY_CFUNC_DEF("setBaseDir", js_lvgl_obj_set_base_dir),
  JERRY_CFUNC_DEF("setBgColor", js_lvgl_obj_set_bg_color),
  JERRY_CFUNC_DEF("setBgColorFiltered", js_lvgl_obj_set_bg_color_filtered),
  JERRY_CFUNC_DEF("setBgOpa", js_lvgl_obj_set_bg_opa),
  JERRY_CFUNC_DEF("setBgGradColor", js_lvgl_obj_set_bg_grad_color),
  JERRY_CFUNC_DEF("setBgGradColorFiltered", js_lvgl_obj_set_bg_grad_color_filtered),
  JERRY_CFUNC_DEF("setBgGradDir", js_lvgl_obj_set_bg_grad_dir),
  JERRY_CFUNC_DEF("setBgMainStop", js_lvgl_obj_set_bg_main_stop),
  JERRY_CFUNC_DEF("setBgGradStop", js_lvgl_obj_set_bg_grad_stop),
  JERRY_CFUNC_DEF("setBgImgOpa", js_lvgl_obj_set_bg_img_opa),
  JERRY_CFUNC_DEF("setBgImgRecolor", js_lvgl_obj_set_bg_img_recolor),
  JERRY_CFUNC_DEF("setBgImgRecolorFiltered", js_lvgl_obj_set_bg_img_recolor_filtered),
  JERRY_CFUNC_DEF("setBgImgRecolorOpa", js_lvgl_obj_set_bg_img_recolor_opa),
  JERRY_CFUNC_DEF("setBgImgTiled", js_lvgl_obj_set_bg_img_tiled),
  JERRY_CFUNC_DEF("setBorderColor", js_lvgl_obj_set_border_color),
  JERRY_CFUNC_DEF("setBorderColorFiltered", js_lvgl_obj_set_border_color_filtered),
  JERRY_CFUNC_DEF("setBorderOpa", js_lvgl_obj_set_border_opa),
  JERRY_CFUNC_DEF("setBorderWidth", js_lvgl_obj_set_border_width),
  JERRY_CFUNC_DEF("setBorderSide", js_lvgl_obj_set_border_side),
  JERRY_CFUNC_DEF("setBorderPost", js_lvgl_obj_set_border_post),
  JERRY_CFUNC_DEF("setTextColor", js_lvgl_obj_set_text_color),
  JERRY_CFUNC_DEF("setTextColorFiltered", js_lvgl_obj_set_text_color_filtered),
  JERRY_CFUNC_DEF("setTextOpa", js_lvgl_obj_set_text_opa),
  JERRY_CFUNC_DEF("setTextLetterSpace", js_lvgl_obj_set_text_letter_space),
  JERRY_CFUNC_DEF("setTextLineSpace", js_lvgl_obj_set_text_line_space),
  JERRY_CFUNC_DEF("setTextDecor", js_lvgl_obj_set_text_decor),
  JERRY_CFUNC_DEF("setTextAlign", js_lvgl_obj_set_text_align),
  JERRY_CFUNC_DEF("setImgOpa", js_lvgl_obj_set_img_opa),
  JERRY_CFUNC_DEF("setImgRecolor", js_lvgl_obj_set_img_recolor),
  JERRY_CFUNC_DEF("setImgRecolorFiltered", js_lvgl_obj_set_img_recolor_filtered),
  JERRY_CFUNC_DEF("setImgRecolorOpa", js_lvgl_obj_set_img_recolor_opa),
  JERRY_CFUNC_DEF("setOutlineWidth", js_lvgl_obj_set_outline_width),
  JERRY_CFUNC_DEF("setOutlineColor", js_lvgl_obj_set_outline_color),
  JERRY_CFUNC_DEF("setOutlineColorFiltered", js_lvgl_obj_set_outline_color_filtered),
  JERRY_CFUNC_DEF("setOutlineOpa", js_lvgl_obj_set_outline_opa),
  JERRY_CFUNC_DEF("setOutlinePad", js_lvgl_obj_set_outline_pad),
  JERRY_CFUNC_DEF("setShadowWidth", js_lvgl_obj_set_shadow_width),
  JERRY_CFUNC_DEF("setShadowOfsX", js_lvgl_obj_set_shadow_ofs_x),
  JERRY_CFUNC_DEF("setShadowOfsY", js_lvgl_obj_set_shadow_ofs_y),
  JERRY_CFUNC_DEF("setShadowSpread", js_lvgl_obj_set_shadow_spread),
  JERRY_CFUNC_DEF("setShadowColor", js_lvgl_obj_set_shadow_color),
  JERRY_CFUNC_DEF("setShadowColorFiltered", js_lvgl_obj_set_shadow_color_filtered),
  JERRY_CFUNC_DEF("setShadowOpa", js_lvgl_obj_set_shadow_opa),
  JERRY_CFUNC_DEF("setLineWidth", js_lvgl_obj_set_line_width),
  JERRY_CFUNC_DEF("setLineDashWidth", js_lvgl_obj_set_line_dash_width),
  JERRY_CFUNC_DEF("setLineDashGap", js_lvgl_obj_set_line_dash_gap),
  JERRY_CFUNC_DEF("setLineRounded", js_lvgl_obj_set_line_rounded),
  JERRY_CFUNC_DEF("setLineColor", js_lvgl_obj_set_line_color),
  JERRY_CFUNC_DEF("setLineColorFiltered", js_lvgl_obj_set_line_color_filtered),
  JERRY_CFUNC_DEF("setLineOpa", js_lvgl_obj_set_line_opa),
  JERRY_CFUNC_DEF("setArcWidth", js_lvgl_obj_set_arc_width),
  JERRY_CFUNC_DEF("setArcRounded", js_lvgl_obj_set_arc_rounded),
  JERRY_CFUNC_DEF("setArcColor", js_lvgl_obj_set_arc_color),
  JERRY_CFUNC_DEF("setArcColorFiltered", js_lvgl_obj_set_arc_color_filtered),
  JERRY_CFUNC_DEF("setArcOpa", js_lvgl_obj_set_arc_opa),
};