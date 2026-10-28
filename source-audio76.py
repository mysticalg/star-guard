from pathlib import Path
import urllib.request,re,json,hashlib,concurrent.futures
root=Path('art-source/audio-v76');root.mkdir(exist_ok=True)
sources=[('qubodup',239138,'mechanical gun burst'),('C-V',704945,'recorded J79 military jet start-cart'),('moxobna',40158,'physical metal crash'),('Joao_Janz',483782,'recorded monster vocal growl'),('Kodack',256602,'recorded varied monster vocals'),('decadylan',825806,'recorded lotion squelches')]
def fetch(row):
 author,id,role=row;url=f'https://freesound.org/people/{author}/sounds/{id}/'
 html=urllib.request.urlopen(urllib.request.Request(url,headers={'User-Agent':'Mozilla/5.0'}),timeout=40).read()
 text=html.decode('utf-8');assert 'creativecommons.org/publicdomain/zero' in text,(author,id,'CC0 missing')
 links=sorted(set(re.findall(r'https://cdn\.freesound\.org/previews/[^\s\"<>]+-hq\.mp3',text)));assert links,(author,id,'no public HQ preview')
 audio=urllib.request.urlopen(links[0],timeout=60).read();assert len(audio)>5000
 name=f'{author}-{id}';(root/(name+'-license-page.html')).write_bytes(html);(root/(name+'-public-hq-original.mp3')).write_bytes(audio)
 meta=dict(author=author,id=id,role=role,source=url,license='CC0 1.0',licenseURL='https://creativecommons.org/publicdomain/zero/1.0/',download=links[0],downloadKind='Highest public HQ MP3 preview; compressed licensed source, not raw author WAV/FLAC',bytes=len(audio),sha256=hashlib.sha256(audio).hexdigest(),originalFile=name+'-public-hq-original.mp3')
 (root/(name+'.json')).write_text(json.dumps(meta,indent=2),encoding='utf-8');return meta
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:
 rows=list(pool.map(fetch,sources))
(root/'sources.json').write_text(json.dumps(rows,indent=2),encoding='utf-8')
print(json.dumps(rows,indent=2))
