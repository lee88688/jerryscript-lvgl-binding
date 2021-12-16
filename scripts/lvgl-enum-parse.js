const fs = require('fs');
const path = require('path');
const yargs = require('yargs/yargs');
const { hideBin } = require('yargs/helpers')

// const argv = yargs(hideBin(process.argv))
//   .options({
//     f: {
//       alias: 'file',
//       demandOption: true,
//       type: 'string',
//       describe: 'enum file name'
//     },
//     d: {
//       alias: 'define',
//       demandOption: true,
//       type: 'string',
//       describe: 'define type name'
//     }
//   })
//   .help()
//   .parse();


async function parser(file, define) {
  const lvglPath = path.resolve(__dirname, '../lvgl/src');
  const dirs = [lvglPath];
  let p;
  let found = false;
  let foundPath = '';
  while ((p = dirs.shift())) {
    const paths = await fs.promises.readdir(p);
    found = paths.includes(file);
    if (found) {
      foundPath = path.resolve(p, file);
      break;
    }
    await Promise.all(paths.map(async subp => {
      const stat = await fs.promises.stat(path.resolve(p, subp));
      if (stat.isDirectory()) {
        dirs.push(path.resolve(p, subp));
      }
    }));
  }
  if (!found) {
    console.log(`can not find file(${file})`);
  }
  const content = await fs.promises.readFile(foundPath, 'utf8');
  const m = content.match(new RegExp(`enum\\s*\\{([^{]*?)\\}[^{]*?${define};`));
  const statements = m[1];
  const matches = statements.matchAll(/^\s*(?!\/\*|\/\/)(\w+)\s*=?(.*?)$/gm);
  let lastValue = 0;
  const arr = []; // store already parsed enum names
  const map = {}; // store already parsed enum names for quick search
  for (const [_, name, value] of matches) {
    let finalValue = value;
    let commentStr = '';
    const mComment = finalValue.match(/(\/\/.*?$)|(\/\*.*?\*\/)/);
    if (mComment) {
      commentStr = mComment[1] ? mComment[1] : mComment[2];
      finalValue = finalValue.slice(0, mComment.index)
    }

    finalValue = finalValue.replace(',', '');

    const mDefine = finalValue.match(/\s([A-Za-z_]+)/g);
    let hasDefine = false;
    if (mDefine) {
      for (let i = 0; i < mDefine.length; i++) {
        const m = content.match(new RegExp(`#define\\s*${mDefine[i]}\\s*(\\(.*?\\))`));
        const preDefine = mDefine[i].trim();
        if (!m && map[preDefine]) {
          // if name is the previous enum
          finalValue = finalValue.replace(preDefine, map[preDefine][1]);
          continue;
        } else if (!m) {
          throw new Error(`${mDefine[i]} can't be resolve at enum(${name})`);
        }
        finalValue = finalValue.replace(mDefine[i], m[1]);
      }
      hasDefine = true
    }

    const parsed = hasDefine ? eval(finalValue) : Number.parseInt(finalValue);
    if (value && !Number.isNaN(parsed)) {  
      lastValue = parsed;
    }
    arr.push([name, lastValue, commentStr]);
    map[name] = [name, lastValue, commentStr];
    lastValue++;
  }
  return arr;
}

// main(argv.f, argv.define);
module.exports = parser;