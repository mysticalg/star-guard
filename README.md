# Star Guard

A C / SDL3 arcade space shooter compiled to WebAssembly for the browser. Eight weapons, twenty-two enemy types, ten sectors, insect formations, four-hit ricochet shots, an expanding player-origin nova bomb, GPU glow particles and synthesised stereo effects.

## Play

Open the GitHub Pages site, click **Launch Game**, then Space to start/fire. Move with arrows, Z/X, mouse or touch; hold mouse to fire. 1–8 selects weapons; B bombs; D shields; O pods; P pauses; R restarts; M mutes; F4 changes effects quality. Gamepads: stick, A fire, B bomb, X shield, Y pods, shoulders change weapon.

Rendering follows the display refresh rate, including 120 Hz displays. Simulation remains fixed at 50 Hz with interpolated rendering. Actual performance depends on the browser, GPU and effects quality. The maximum GPU particle pool is 12,288; use F4 on slower devices. Touch dragging flies and fires.

## Build

Install the official [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html), version **6.0.13**, activate it and enter its environment. Run `python build.py`. SDL3 is fetched through Emscripten's official port. The build emits `docs/game.js`, `.wasm` and `.data`.

For local play, run `python -m http.server 8080 --directory docs`, then open http://localhost:8080. A server is required; opening the HTML file directly will not work.

GitHub Pages serves `main:/docs`. Commit rebuilt files after source changes. No backend, account, analytics or API key is required. All game assets ship with the site.

## Implementation

`src/pc.c` contains the SDL platform, browser main loop, rendering and audio mixer. `src/game.c` and the expansion headers contain gameplay. `Assets/sprites.bin` includes the ImageGen insect sprites. The browser loop returns on each animation frame and performs no native sleeping or busy waiting. Sound begins after a user gesture; losing focus pauses play. Single-threaded WebAssembly avoids requiring special cross-origin hosting headers.

## Credits

Original game and generated artwork developed for Dave. SDL3 is provided under its zlib licence; see `SDL-LICENSE.txt`. Original/generated sprites and synthesised sound effects are included; the supplied OVG soundtrack is credited below. References to classic arcade games describe gameplay inspiration only.

Weapon 8 is a three-way split ricochet laser with five enemy hits per beam. All friendly projectiles remain harmless to the player after rebounds. Shots inherit two-thirds of horizontal ship velocity, capped at four pixels per simulation tick.

Ten continuous sectors now include Verdant Skies, Dune Sea, Frost Ridge and Magma Coast, with scrolling terrain below the ship, distant mountains and clouds moving at different parallax speeds. Each sector ends with a distinct boss silhouette, name, movement and projectile pattern. Live objects survive sector handovers; stages no longer clear enemies, hazards, bullets or pickups. Use the starting-sector menu to explore any level.

The browser canvas is 1920 by 1600, with a matching full-resolution particle target. Retro scenery and original sprites retain their pixel-art style; new enemy art has twice the texel density. F4 reduces particle density on slower hardware.

Specialists: Prism Strider (zigzag/triple fan), Shield Bulwark (cycling armour), Mine Serpent (sweeping mine laying), Lance Charger (telegraphed dive), Phase Stalker (blink/reappear), Brood Carrier (launches two drones on destruction), Orbit Weaver (spiral projectiles), Rail Sniper (target-lock tracer and fast shot).

Sector handovers play a 4.3-second filtered white-noise thruster sweep. Its envelope rises and fades, with the filter opening around the middle. Muting also mutes the whoosh.

## Soundtrack

OVG Alien 8 - Follow the Valves, Version 1 (168.24 s), supplied by Dave from his downloaded WAV. [Original track](https://suno.com/song/7ce95598-c808-4dd5-8163-2251faf26b3a). Compressed to 128 kb/s MP3 for the game. Starts on Launch, loops, pauses with gameplay, follows master mute and has its own music-volume slider. Music ducks during sector transitions.

The playlist also includes OVG Sabre Wulf - Follow the Orchid (Version 2) and OVG Shinobi School Run File (Version 1), supplied by Dave from Downloads. The selector changes tracks; each track loops until the next level advances the soundtrack. Original downloaded audio stays untouched.

Flying enemies rotate toward actual frame-to-frame movement, with a bounded turn rate and shortest-path angular interpolation. Bug atlas art faces upward at zero rotation; other ships use their own orientation offset. Rotated quads stay in the same sprite batch, avoiding extra draw calls.

## Cinematic graphics update

The renderer uses full-colour ImageGen environment JPEGs and an HD RGBA sprite atlas, rather than enlarging the old 320x256 scenery. Choose Balanced (1920x1600), Cinematic (2880x2400, default), or Ultra (3840x3200) before launching. The terrain images are 1536x1024 native assets; the larger canvas improves compositing and edges and does not invent additional photographic detail. The game remains a 2D shooter with photorealistic-style artwork, rather than a real-time 3D simulation.

Six generated environment plates cover space, cavern rock, verdant valleys, dunes, frost and magma. Other space sectors use distinct colour treatments. A separately generated transparent cloud layer scrolls at two depths. Stage changes crossfade the environment, while the moving cavern walls remain aligned with their collision corridor. The player, flying enemies and ten bosses now use detailed generated artwork, retaining their movement and combat rules. Rotations stay GPU-batched. The HUD uses crisp HTML typography at every resolution.

Built-in ImageGen produced the assets; prompts are recorded in Artwork/IMAGEGEN-PROMPTS.json. Optimised runtime files live in Assets/modern. Full generated PNG originals are preserved in the local delivered Artwork folder but excluded from Git to avoid duplicating large runtime assets.

JPEG/PNG textures are decoded with stb_image, from the official nothings/stb repository; its public-domain/MIT licence is embedded in src/stb_image.h. No additional image SDK is required to build.

## Alternating flight modes

Odd-numbered sectors use vertical flight; even-numbered sectors use horizontal side flight. Side stages use a separately generated side-profile interceptor and fire right into enemies arriving from the right. The combat camera rotates smoothly during handover, while landscape backgrounds and HUD remain upright. Collision, shots, lasers, ricochets, bombs, cave walls and particles share one combat transform, preserving their alignment. Keyboard, mouse, touch and gamepad movement is mapped through the same inverse transform, including during the turn.

Backgrounds scroll continuously in the flight direction: three depths of stars over space, a slower scenery plate and two faster transparent cloud layers over landscapes. Sprite and glow geometry compensates for the side-view camera so ships keep their proportions.

Player projectiles and beams intercept ordinary hostile bullets. Ricochet shots and split lasers spend one hit per interception. The heavy Shield Bulwark and Brood Carrier fire amber armored rounds that must be dodged; nova bombs still clear them. Swept collision checks prevent fast crossing shots from slipping through.

Sector completion is gated by surviving enemies and hazards. After the encounter clears, a two-second warning precedes a smooth boss entrance from outside the playfield. Bosses cannot attack or take damage during entry. Surviving hostile rounds finish travelling before the cleared sector advances. Bug squadrons finish their sorties and leave; cavern enemies enter from outside the viewport.

The player hull renders at half width and height in both flight modes, with a matching smaller collision area and closer muzzle/exhaust effects. Fast dense foreground clouds pass at 880/1920 logical pixels per second (a further fourfold boost) at cruise speed; space uses two additive blue/violet gas layers at 100/360, behind the moving stars. These reuse the resident cloud texture without extra downloads.

Bosses bank up to twelve degrees with their actual lateral movement, easing back toward level while keeping their collision centres aligned.

Right-click over the canvas triggers the nova bomb (B and the touch button still work). Music loops within each level and advances to the next OVG track when the next sector begins; the manual track selector remains available.

## Shared high scores

The game remains on GitHub Pages. `score-service/` is a separate Netlify Functions/Postgres backend for the world top-20 table at `https://star-guard-scores-mysticalg.netlify.app/api/scores`. Runs register at launch, and game-over scores are saved after the player submits a public pilot name. Gameplay does not depend on the score service. The browser remembers only the pilot name and an unsent result for retry; the table itself lives online.

The API validates names and score ranges, registers run start times, limits requests and accepts one entry per run. This is a casual arcade table, not a server-authoritative anti-cheat system. SQL parameters are bound and names render as text. Schema changes use the timestamped migrations under `score-service/netlify/database/migrations/`; connection strings and Netlify state are excluded from Git. Run API tests with `npm test` from `score-service/`.

Weapon 4 is a thin translucent rainbow beam: two energy every six simulation ticks, sustained while held. Weapon 6 fires three rainbow beams for three energy per pulse. Weapon 8 fires narrow cyan bouncing laser bolts with five-hit persistence and much lighter trails.

Scroll the mouse wheel over the canvas to change weapon (down: next, up: previous), wrapping through all eight. High-resolution wheel deltas accumulate into steps; wheel input outside the canvas scrolls the page normally.


## Earned equipment and new stages

Start with no option pods. Collect up to five, plus a separate orbiting satellite and bomb dropper. Weapon capsules equip their weapon and raise its saved power tier (maximum four); generic power capsules upgrade the current weapon. Higher tiers increase projectile size, damage and fire rate. Weak support shots, capped tiers, enemy armour progression and larger boss health keep upgrades useful without instantly erasing encounters. Pickups pulse, glow and orbit with sparks; equipment and remaining enemy hulls now use full-alpha ImageGen sprites.

The twelve-sector campaign adds Low Earth Orbit with a fast top-down Earth flyover and quality-scaled directional background blur, and Ground Assault with a horizontal solid floor, tanks and aimed turrets. Contact with the floor blocks movement without damaging the player. Ground guns enter from outside the viewport and flash before firing. Each new stage has its own boss artwork and attacks. Blur only affects scenery, leaving combat and the HUD sharp. New ImageGen prompts are recorded in Artwork/EQUIPMENT-PROMPTS.json and Artwork/LEVEL-PROMPTS.json.


## Boss forms and warp camera

All twelve bosses have two extra ImageGen poses, played as a three-pose ping-pong animation. Boss health is now 300 + 32 per campaign sector. At two-thirds and one-third health, a protected 55-tick transformation unfolds additional weapon pods and changes the attack cadence. The final form adds more fire from the deployed pods. Damage cannot skip a transformation threshold. Hull scorch points and orange/electrical sparking accumulate with damage. Nova bombs deal one heavy 90-HP hit per boss per detonation instead of deleting bosses.

During sector transit the camera eases toward the ship, zooms to 2.4x, holds, then smoothly returns. Input mapping accounts for zoom, and combat continues through the transition. The additional scene target is reused, and the extra compositing pass runs only during warp. Animation poses and their prompts are recorded in Artwork/BOSS-ANIMATION-PROMPTS.json.
