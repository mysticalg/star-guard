from pathlib import Path
import json,os,shutil,subprocess
root=Path(__file__).resolve().parents[1]
emcc=os.environ.get('EMCC') or shutil.which('emcc') or str(root.parent/'emsdk/upstream/emscripten/emcc.exe')
subprocess.run([emcc,'tests/seekers82-bench.c','-O2','--use-port=sdl3','-sALLOW_MEMORY_GROWTH=1','-o','tests/seekers82-bench.js'],cwd=root,check=True,shell=os.name=='nt')
a=json.loads((root/'tests/seekers82-baseline.json').read_text(encoding='utf-8'))
b=json.loads(subprocess.check_output(['node','--require','./tests/node-audio-window74.js','tests/seekers82-bench.js'],cwd=root,text=True))
assert len(a['radial'])==len(b['radial'])==72
for old,new in zip(a['radial'],b['radial']):
    for key in ['ship','variant','rank','echo']: assert old[key]==new[key]
    assert new['types'][0]==new['types'][1], 'laser-resistant and ordinary classes use the same missile family'
    for x,y in zip(old['types'],new['types']):
        assert all(q>=p for p,q in zip(x,y)), 'controlled damage/range never regresses'
        assert y[0]>x[0] and y[-1]==0, 'heavier finite-radius impact'
assert a['coverage']['environmentDamage']==a['coverage']['giantDamage']==a['coverage']['weatherDamage']==0
assert all(b['coverage'][key]>0 for key in ['environmentDamage','giantDamage','weatherDamage'])
(root.parent/'seekers82-final-bench.json').write_text(json.dumps({'baseline':a,'candidate':b},indent=2),encoding='utf-8')
print('PASS72 controlled radial/rank/variant/chassis/echo comparisons: heavier/farther actual damage, missile family consistency, finite radius, new environment/giant/weather splash coverage')
