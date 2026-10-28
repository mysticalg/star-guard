"""Build/run affected native suites; use --focused for the three Tesla fixtures."""
from pathlib import Path
import concurrent.futures,json,os,shutil,subprocess,sys
root=Path(__file__).resolve().parents[1]
emcc=os.environ.get('EMCC') or shutil.which('emcc') or str(root.parent/'emsdk/upstream/emscripten/emcc.exe')
names=['tesla78','tesla78-combat','balance70-tesla']
if '--focused' not in sys.argv:
    names+=['expanded-combat','boss65','waves72','protection70','audio-events76']
def run(name):
    p=subprocess.run([emcc,'tests/'+name+'.c','-O2','--use-port=sdl3','-sALLOW_MEMORY_GROWTH=1','-o','tests/'+name+'.js'],cwd=root,capture_output=True,text=True,shell=os.name=='nt')
    if p.returncode:return dict(name=name,compile=p.returncode,out=p.stderr[-1800:])
    p=subprocess.run(['node','--require','./tests/node-audio-window74.js','tests/'+name+'.js'],cwd=root,capture_output=True,text=True)
    output=p.stdout+p.stderr
    (root.parent/(name+'-78-output.txt')).write_text(output,encoding='utf-8')
    return dict(name=name,exit=p.returncode,out=output[-300:] if name not in ['tesla78-combat','balance70-tesla'] else 'Metrics recorded; see tesla78-verify.py for baseline comparison')
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:
    rows=list(pool.map(run,names))
target=root.parent/'checkpoint78-native.json'
if '--focused' in sys.argv and target.exists():
    old=json.loads(target.read_text(encoding='utf-8'))
    rows+= [x for x in old if x['name'] not in names]
target.write_text(json.dumps(rows,indent=2),encoding='utf-8')
print(json.dumps(rows,indent=2))
if any(x.get('exit',1) for x in rows):raise SystemExit(1)
subprocess.run([sys.executable,'tests/tesla78-verify.py'],cwd=root,check=True)
