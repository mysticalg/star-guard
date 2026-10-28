from pathlib import Path
import subprocess,urllib.request,hashlib,json,concurrent.futures
commit=subprocess.check_output(['git','rev-parse','HEAD'],text=True).strip();changed=subprocess.check_output(['git','diff','--name-only','41422e3',commit,'--','docs'],text=True).splitlines();files=sorted(set([f[5:] for f in changed]+['audio-output76.js','recorded-drive76.js','vo/pickup-names-v76.json','vo/pickup-names-v76.mp3','audio/sfx/SOURCES-v76.txt','armory-ui.css','pilot-integration.js','highscores.js','ship-voice.js','music-catalog.js','weapon-variants.js','armory-range.js','armory-range.wasm','armory-ui.js','weather-audio.js','sample-sfx.js','index.html','game.js','game.wasm','game.data','app.js','pilot-store.js','music-director.js','style.css','images/journey-ships-v2.png','audio/weather-thunder-v65.ogg','audio/weather-thunder-v65-2.ogg','audio/weather-thunder-v65-3.ogg','audio/weather-rain-v65.ogg','audio/weather-credits.txt','images/asteroid-cavern.jpg','images/diamond-cavern.jpg','images/phoenix.png']))
def check(file):
 url='https://mysticalg.github.io/star-guard/'+file+'?release=81';live=urllib.request.urlopen(urllib.request.Request(url,headers={'Cache-Control':'no-cache'}),timeout=90).read();expected=subprocess.check_output(['git','show',commit+':docs/'+file]);assert live==expected,file;return {'file':file,'bytes':len(live),'sha256':hashlib.sha256(live).hexdigest(),'commit':commit}
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:rows=list(pool.map(check,files))
for q in [35,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81]:
 live=urllib.request.urlopen('https://mysticalg.github.io/star-guard/?release='+str(q),timeout=60).read();assert live==subprocess.check_output(['git','show',commit+':docs/index.html']),q;rows.append({'old_query':q,'matches_current_index':True})

for suffix in ['', '?diagnostics=1&release=35']:
 live=urllib.request.urlopen('https://mysticalg.github.io/star-guard/'+suffix,timeout=60).read();assert live==subprocess.check_output(['git','show',commit+':docs/index.html']),suffix;rows.append({'entry':suffix,'matches_current_index':True})
Path('../release81-live-hashes.json').write_text(json.dumps(rows,indent=2));print('PASS',len(files),'deployed files equal canonical commit blobs, plus root/current/old-query entry variants')
