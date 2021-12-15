const camelCase = require('lodash/camelCase');
const fs = require('fs');
const path = require('path');
const enumParser = require('./lvgl-enum-parse');

async function generateJS(fileName, enumName) {
  const nameValueArr = await enumParser(fileName, enumName);
  
  let jsFileName = enumName;
  if (jsFileName.endsWith('_t')) {
    jsFileName = jsFileName.slice(0, jsFileName.length - 2);
  }
  jsFileName = camelCase(jsFileName);

  const jsConent = `const ${jsFileName} = {\n${nameValueArr.map(([n, v, c]) => `  ${n}: ${v},${c ? ` ${c}` : ''}`).join('\n')}\n};\nexport default ${jsFileName};`;
  await fs.promises.writeFile(path.join('./constants', `${jsFileName}.js`), jsConent);

  const tsContent = `declare const ${jsFileName}: Record<string, number>;\nexport default ${jsFileName};`;
  await fs.promises.writeFile(path.join('./constants', `${jsFileName}.d.ts`), tsContent);

  return jsFileName;
}

async function main() {
  const names = await Promise.all([
    // lv_style
    generateJS('lv_style.h', 'lv_style_prop_t'),
    // lv_obj
    generateJS('lv_obj.h', 'lv_state_t'),
    generateJS('lv_obj.h', 'lv_part_t'),
    generateJS('lv_obj.h', 'lv_obj_flag_t'),
    // lv_event
    generateJS('lv_event.h', 'lv_event_code_t'),
  ]);

  const indexJsContent = names.map(n => `export { default as ${n} } from './${n}'`).join('\n');
  await fs.promises.writeFile('./constants/index.js', indexJsContent);
  await fs.promises.writeFile('./constants/index.d.ts', indexJsContent);
}

main();
