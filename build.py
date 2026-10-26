import subprocess,os
from pathlib import Path
root=Path(__file__).resolve().parent
emcc=os.environ.get('EMCC','emcc')
subprocess.run([emcc,'src/pc.c','-O3','--use-port=sdl3','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=67108864','-sMAX_WEBGL_VERSION=2','-sMIN_WEBGL_VERSION=2','-sINVOKE_RUN=0','-sEXPORTED_FUNCTIONS=["_main","_web_command","_web_held","_web_pointer","_web_mute","_web_stat","_web_pilot","_web_motion"]','-sEXPORTED_RUNTIME_METHODS=["callMain"]','--preload-file','Assets','--exclude-file','*asteroid-cavern.jpg','--exclude-file','*diamond-cavern.jpg','--exclude-file','*phoenix.png','--exclude-file','*giant-hulls-v1.png','--exclude-file','*player-ships-v1.png','--exclude-file','*organic-parasites-v1.png','--exclude-file','*giant-worm-v1.png','--exclude-file','*boss-frames.png','--exclude-file','*giant-worm-v2.png','-o','docs/game.js'],cwd=root,check=True,shell=os.name=='nt')
subprocess.run([emcc,'src/armory_range.c','-O3','--use-port=sdl3','-sMODULARIZE=1','-sEXPORT_NAME=StarGuardRange','-sENVIRONMENT=web,node','-sALLOW_MEMORY_GROWTH=1','-sEXPORTED_FUNCTIONS=["_range_init","_range_step","_range_read"]','-o','docs/armory-range.js'],cwd=root,check=True,shell=os.name=='nt')
