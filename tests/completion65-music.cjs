const fs=require('fs'),assert=require('assert/strict'),{resolve}=require('../docs/music-director.js'),src=fs.readFileSync('docs/app.js','utf8');
const code=src.slice(src.indexOf('function readMusicState()')).split('\n')[0],values={10:1,160:0,162:0,65:120,187:1,24:0};
const adapt=new Function('started','preflightContext','Module',code+';return readMusicState();');
let s=adapt(true,'ship',{_web_stat:n=>values[n]||0});assert.equal(s.victory,0);assert.notEqual(resolve(s).index,23);
values[10904]=1;s=adapt(true,'ship',{_web_stat:n=>values[n]||0});assert.equal(resolve(s).index,23);
values[10904]=0;values[200]=1;s=adapt(true,'ship',{_web_stat:n=>values[n]||0});assert.equal(resolve(s).index,7);
console.log('PASS production music adapter suppresses internal victory, true wholemission victory/refit retained');
