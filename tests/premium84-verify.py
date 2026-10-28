from pathlib import Path
import json,os,shutil,subprocess
root=Path(__file__).resolve().parents[1]
emcc=os.environ.get('EMCC') or shutil.which('emcc') or str(root.parent/'emsdk/upstream/emscripten/emcc.exe')
subprocess.run([emcc,'tests/weapons81-bench.c','-O2','--use-port=sdl3','-sALLOW_MEMORY_GROWTH=1','-o','tests/weapons81-bench.js'],cwd=root,check=True,shell=os.name=='nt')
current=json.loads(subprocess.check_output(['node','--require','./tests/node-audio-window74.js','tests/weapons81-bench.js'],cwd=root,text=True))
before=json.loads((root/'tests/weapons81-baseline.json').read_text(encoding='utf-8'))
assert len(current['banks'])==324 and current['banks']==before['banks'], 'every-step bank/cooldown/heat/remainder checksum and duration must match80'
for row in current['spread']:
    assert len(row['rays'])==[5,7,9,12][row['rank']-1]
    assert len({(r[0],r[1]) for r in row['rays']})==len(row['rays'])
for a,b in zip(before['triple'],current['triple']):
    assert a['ticksToEmpty']==b['ticksToEmpty']
    assert b['kills']>=a['kills'] and b['firstKillTicks']<=a['firstKillTicks']
    if b['rank']==1: assert a==b
    else: assert b['halfWidth']>a['halfWidth'] and b['damage']>a['damage']
report={'exactBankCases':324,'spreadCases':len(current['spread']),'baselineTriple':before['triple'],'currentTriple':current['triple'],'fixture':current['fixture']}
(root.parent/'premium84-bank-bench.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print('PASS 324 exact every-step bank hashes/durations,36 Spread patterns, four controlled Triple width/damage/TTK/kill comparisons')
