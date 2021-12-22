const clipboardy = require('clipboardy');
const constants = require('@quickgl/constants');

const json = clipboardy.readSync();
const tree = JSON.parse(json);

const stylePropMap = {};
Object.keys(constants.lvStyleProp).forEach(k => {
  stylePropMap[constants.lvStyleProp[k]] = k;
});

const queue = [];
queue.push(tree);

while(queue.length) {
  const cur = queue.shift();
  if (cur.children && cur.children.length) {
    queue.push(...cur.children);
  }
  if (cur.styles && cur.styles.length) {
    for (const style of cur.styles) {
      if (style.attrs && style.attrs.length) {
        const newAttrs = {};
        for (const attr of style.attrs) {
          if (attr.name in stylePropMap) {
            newAttrs[stylePropMap[attr.name]] = attr.value;
          } else {
            newAttrs[attr.name] = attr.value;
          }
        }
        style.attrs = newAttrs;
      }
    }
  }
}

clipboardy.writeSync(JSON.stringify(tree));

