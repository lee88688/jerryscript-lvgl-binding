const fs = require('fs');
const {camelCase} = require('lodash');

const reg = /lv_obj_set_style_(.*?)\((.*?)\);/g;
const content = fs.readFileSync('../lvgl/src/core/lv_obj_style_gen.h', 'utf8');

const typeAlignMap = {
  // uint32_t: 0,
  lv_color_t: 1,
}

const value_types = new Set();
const js_func_def = [];

const matchs = content.matchAll(reg);
for (const [_, name, paramsStr] of matchs) {
  const params = paramsStr.split(',').map(item => item.trim());
  if (params.length !== 3) {
    console.warn(`have ${params.length} params.`);
    continue;
  }
  if (params[1].includes('*')) {
    // console.log(`unsupport parameter type: ${params[1]}.`);
    continue;
  }
  const [pType] = params[1].split(' ');
  value_types.add(pType);

  let alignType = 0;
  for (const key of Object.keys(typeAlignMap)) {
    if (pType.includes(key)) {
      alignType = typeAlignMap[key];
    }
  }
  console.log(`STYLE_FUNC(${name}, INT, ${alignType});`);
  js_func_def.push(`JS_CFUNC_DEF("${camelCase(`set ${name}`)}", 1, js_lvgl_obj_set_${name}),`);
}

console.log(`\n\n${js_func_def.join('\n')}`);
// console.log(`\n\n${[...value_types].join('\n')}`);

