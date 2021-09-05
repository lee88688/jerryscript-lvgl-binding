---
to: ../jerry-lvgl-binding/<%= 'lvgl-' + h.changeCase.lower(widgetName) %>.h
---
#ifndef LVGL_<%= h.changeCase.upper(widgetName) %>_H
#define LVGL_<%= h.changeCase.upper(widgetName) %>_H

void js_lvgl_<%= h.changeCase.lower(widgetName) %>_init();

#endif /* LVGL_<%= h.changeCase.upper(widgetName) %>_H */