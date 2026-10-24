import subprocess,os
from pathlib import Path
root=Path(__file__).resolve().parent
emcc=os.environ.get('EMCC','emcc')
subprocess.run([emcc,'src/pc.c','-O3','--use-port=sdl3','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=67108864','-sMAX_WEBGL_VERSION=2','-sMIN_WEBGL_VERSION=2','-sINVOKE_RUN=0','-sEXPORTED_FUNCTIONS=["_main","_web_command","_web_held","_web_pointer","_web_mute","_web_stat"]','-sEXPORTED_RUNTIME_METHODS=["callMain"]','--preload-file','Assets','--exclude-file','*asteroid-cavern.jpg','--exclude-file','*diamond-cavern.jpg','-o','docs/game.js'],cwd=root,check=True,shell=os.name=='nt')
