# Star Guard

A C / SDL3 arcade space shooter compiled to WebAssembly for the browser. Seven weapons, fourteen enemy types, six sectors, insect formations, four-hit ricochet shots, an expanding player-origin nova bomb, GPU glow particles and synthesised stereo effects.

## Play

Open the GitHub Pages site, click **Launch Game**, then Space to start/fire. Move with arrows, Z/X, mouse or touch; hold mouse to fire. 1–7 selects weapons; B bombs; D shields; O pods; P pauses; R restarts; M mutes; F4 changes effects quality. Gamepads: stick, A fire, B bomb, X shield, Y pods, shoulders change weapon.

Rendering follows the display refresh rate, including 120 Hz displays. Simulation remains fixed at 50 Hz with interpolated rendering. Actual performance depends on the browser, GPU and effects quality. The maximum GPU particle pool is 12,288; use F4 on slower devices. Touch dragging flies and fires.

## Build

Install the official [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html), version **6.0.13**, activate it and enter its environment. Run `python build.py`. SDL3 is fetched through Emscripten's official port. The build emits `docs/game.js`, `.wasm` and `.data`.

For local play, run `python -m http.server 8080 --directory docs`, then open http://localhost:8080. A server is required; opening the HTML file directly will not work.

GitHub Pages serves `main:/docs`. Commit rebuilt files after source changes. No backend, account, analytics or API key is required. All game assets ship with the site.

## Implementation

`src/pc.c` contains the SDL platform, browser main loop, rendering and audio mixer. `src/game.c` and the expansion headers contain gameplay. `Assets/sprites.bin` includes the ImageGen insect sprites. The browser loop returns on each animation frame and performs no native sleeping or busy waiting. Sound begins after a user gesture; losing focus pauses play. Single-threaded WebAssembly avoids requiring special cross-origin hosting headers.

## Credits

Original game and generated artwork developed for Dave. SDL3 is provided under its zlib licence; see `SDL-LICENSE.txt`. No third-party game art or audio is included. References to classic arcade games describe gameplay inspiration only.
