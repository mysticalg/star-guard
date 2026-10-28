"""Compare production combat metrics against the preserved release77 baseline."""
from pathlib import Path
import json, subprocess
root=Path(__file__).resolve().parents[1]
def run(name):
    p=subprocess.run(['node','--require','./tests/node-audio-window74.js','tests/'+name+'.js'],cwd=root,capture_output=True,text=True,check=True)
    data=json.loads(p.stdout.strip().splitlines()[-1])
    (root.parent/(name+'-78-results.json')).write_text(json.dumps(data,indent=2),encoding='utf-8')
    return data
baseline=json.loads((root/'tests/tesla78-baseline.json').read_text(encoding='utf-8'))
actual=run('tesla78-combat')
old={tuple(x[:4]):x for x in baseline['banks']}
other=[x for x in actual['banks'] if x[1]!=8]
tesla=[x for x in actual['banks'] if x[1]==8]
assert len(other)==288 and all(x==old[tuple(x[:4])] for x in other)
assert len(tesla)==36 and all(x[4]>old[tuple(x[:4])][4] for x in tesla)
assert all(x['killed'] for x in actual['ttk'])
assert len(actual['normalWave'])==36 and all(x['kills']>0 for x in actual['normalWave'])
groups=run('balance70-tesla')['teslaGroups']
assert [x['killsBeforeEmpty'] for x in groups]==[14,17,19,20]
print('PASS release78:288 other banks byte-equivalent,36 Tesla banks longer,12 actual target kills,36 normal moving-wave cases and mixed20-target groups14/17/19/20')
