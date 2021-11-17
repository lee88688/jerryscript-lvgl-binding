const fs = require('fs');
const path = require('path');
const yargs = require('yargs/yargs');
const { hideBin } = require('yargs/helpers')

const enumReg = /enum\s*\{[^]*?\}[^]*?lv_align_t/

const argv = yargs(hideBin(process.argv))
  .options({
    f: {
      alias: 'file',
      demandOption: true,
      type: 'string',
      describe: 'enum file name'
    },
    d: {
      alias: 'define',
      demandOption: true,
      type: 'string',
      describe: 'define type name'
    }
  })
  .help()
  .parse();

// console.log(argv);
const lvglPath = path.resolve(__dirname, '../lvgl/src');

async function main() {
  const dirs = [lvglPath];
  let p;
  let found = false;
  let foundPath = '';
  while ((p = dirs.shift())) {
    const paths = await fs.promises.readdir(p);
    found = paths.includes(argv.f);
    if (found) {
      foundPath = path.resolve(p, argv.f);
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
    console.log(`can not find file(${argv.f})`);
  }
  const content = await fs.promises.readFile(foundPath, 'utf8');
  const m = content.match(new RegExp(`enum\\s*\\{([^]*?)\\}[^]*?${argv.d};`));
  const statements = m[1];
  const matches = statements.matchAll(/^\s*(?!\/\*|\/\/)(\w+)\s*=?(.*?)$/gm);
  let lastValue = 0;
  const arr = [];
  for (const [_, name, value] of matches) {
    const parsed = Number.parseInt(value);
    if (value && !Number.isNaN(parsed)) {  
      lastValue = parsed;
    }
    arr.push([name, lastValue]);
    lastValue++;
  }
  console.log(arr.join('\n'));
}

main();