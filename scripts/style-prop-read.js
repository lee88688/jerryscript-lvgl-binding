const fs = require('fs');
const path = require('path');

const content = fs.readFileSync(path.join(__dirname, '../lvgl/src/misc/lv_style.h'), 'utf8');
const stylePropContentReg = /typedef enum \{([^]*?)\}lv_style_prop_t/;
const stylePropStatementReg = /(_?LV_STYLE_.*?)\s=(.*?),/g;

const matched = content.match(stylePropContentReg)
const statements = matched[1];
const matches = statements.matchAll(stylePropStatementReg);
const props = [];
for (const [_, propName, value] of matches) {
  const values = value.split('|').map(item => item.trim());
  let sum = parseInt(values[0]);
  for (let i = 1; i < values.length; i++) {
    const reg = new RegExp(`#define\\s*${values[i]}\\s*\\((.*?)\\)`);
    const m = content.match(reg);
    sum |= eval(m[1]);
  }
  props.push({name: propName, value: sum});
}

for (let {name, value} of props) {
  console.log(`${name}: ${value},`);
}