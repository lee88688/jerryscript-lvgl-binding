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

  const jsConent = `const ${jsFileName} = {\n${nameValueArr.map(([n, v, c]) => `  ${n}: ${v},${c ? ` ${c}` : ''}`).join('\n')}\n};\nmodule.exports = ${jsFileName};`;
  await fs.promises.writeFile(path.join('./constants', `${jsFileName}.js`), jsConent);

  const tsContent = `const ${jsFileName}: Record<string, number> = {\n${nameValueArr.map(([n, v, c]) => `  ${n}: ${v},${c ? ` ${c}` : ''}`).join('\n')}\n};\nexport = ${jsFileName};`;
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
    // lv_flex
    generateJS('lv_flex.h', 'lv_flex_align_t'),
    generateJS('lv_flex.h', 'lv_flex_flow_t'),
  ]);

  const indexJsRequireContent = names.map(n => `const ${n} = require('./${n}');`).join('\n');
  await fs.promises.writeFile(
    './constants/index.js',
    `${indexJsRequireContent}\nmodule.exports = {${names.join(', ')}};`
  );
  const indexTsRequireContent = names.map(n => `import * as ${n} from './${n}';`).join('\n');
  await fs.promises.writeFile(
    './constants/index.d.ts',
    `${indexTsRequireContent}\nexport {${names.join(', ')}};`
  );
}

main();
