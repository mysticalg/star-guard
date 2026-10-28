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



Weapon 4 is a thin translucent rainbow beam: two energy every six simulation ticks, sustained while held. Weapon 6 fires three death laser beams for three energy per pulse. Weapon 8 fires narrow cyan bouncing laser bolts with five-hit persistence and much lighter trails.



Scroll the mouse wheel over the canvas to change weapon (down: next, up: previous), wrapping through all eight. High-resolution wheel deltas accumulate into steps; wheel input outside the canvas scrolls the page normally.





## Earned equipment and new stages



Start with no option pods. Collect up to five, plus a separate orbiting satellite and bomb dropper. Weapon capsules equip their weapon and raise its saved power tier (maximum four); generic power capsules upgrade the current weapon. Higher tiers increase projectile size, damage and fire rate. Weak support shots, capped tiers, enemy armour progression and larger boss health keep upgrades useful without instantly erasing encounters. Pickups pulse, glow and orbit with sparks; equipment and remaining enemy hulls now use full-alpha ImageGen sprites.



The twelve-sector campaign adds Low Earth Orbit with a fast top-down Earth flyover and quality-scaled directional background blur, and Ground Assault with a horizontal solid floor, tanks and aimed turrets. Contact with the floor blocks movement without damaging the player. Ground guns enter from outside the viewport and flash before firing. Each new stage has its own boss artwork and attacks. Blur only affects scenery, leaving combat and the HUD sharp. New ImageGen prompts are recorded in Artwork/EQUIPMENT-PROMPTS.json and Artwork/LEVEL-PROMPTS.json.





## Boss forms and warp camera



All twelve bosses now have two larger ImageGen transformation forms with additional attached armour and weapons. The former rapid pose loop has been replaced by smooth blends during transformations; each phase keeps a stable hull, with continuous bank, movement and reactor pulses. Boss health is now 300 + 32 per campaign sector. At two-thirds and one-third health, a protected 55-tick transformation unfolds additional weapon pods and changes the attack cadence. The final form adds more fire from the deployed pods. Damage cannot skip a transformation threshold. Hull scorch points and orange/electrical sparking accumulate with damage. Nova bombs deal one heavy 90-HP hit per boss per detonation instead of deleting bosses.



During sector transit the camera eases toward the ship, zooms to 2.4x, holds, then smoothly returns. Input mapping accounts for zoom, and combat continues through the transition. The additional scene target is reused, and the extra compositing pass runs only during warp. Animation poses and their prompts are recorded in Artwork/BOSS-ANIMATION-PROMPTS.json.





## Longer missions, storms and special attacks



Each sector now builds through twelve formations, with mixed insect flights, specialist squads, streams, fans and V formations. Wave spacing and an earliest 48-second boss gate (54 seconds in the cavern) give the encounter time to build; surviving enemies and hazards still gate boss entry. The HUD shows mission time, current sector time and completed wave count. Timers stop while paused and mission time freezes at game over.



Asteroid Belt, Frost Ridge and Magma Coast include dense destructible asteroid, ice and fire storms. Storms enter from off-screen, leave gaps and use isolated full-alpha ImageGen obstacle sprites. Boss specials alternate a 1.2-second laser aim warning with a brief super-laser, and shootable bomblets that burst into eight ordinary rounds. Armour-piercing bullets remain amber; ordinary, aimed, spiral and bomblet fire have different colours and shapes. Each player weapon also has its own trail and glow colour.



Warp now zooms to an extreme ship close-up, holds, then pulls back. The ship is rendered directly from source artwork in the close-up to avoid magnifying the small combat rendering. Prompt records for the new sprites are in Artwork/BOSS-TRANSFORMATION-PROMPTS.json.





Impact audio now distinguishes metal hulls, energy shields, rock, ice, molten debris and shot interception. Projectile type shifts the transient pitch and character. Short synthesized metallic, resonant, granular and glass-like responses use the existing 48-voice mixer, stereo impact positions and per-material/global event limits. Master mute still applies to the whole audio stream.





The ship computer uses a locally synthesized female Microsoft Zira voice, lightly filtered and modulated into a cybernetic announcer. A compact audio sprite supplies 44 pickup, warning and sector announcements. Urgent warnings interrupt pickup chatter, the music ducks during speech, pause/focus/master mute are respected, and a separate Ship voice button can disable announcements. Voice audio loads after launch and does not block gameplay.





Pacing refinement: waves are spaced 3.4 seconds apart, with fewer overlapping enemies. Boss volleys and special attacks have about 15 percent more recovery time; storms arrive slightly less frequently. Player movement, firing, projectile speed and the rendering refresh rate remain responsive.





Weapon capsules now choose only weapons not yet collected, reserving those already falling so two copies cannot be scheduled together. When every weapon is collected, those drops become salvage. Bronze scrap, silver caches and rare gold tech award 250, 750 and 1500 points. Separate power capsules upgrade owned weapons. Every three weapon-power upgrades evolves the player ship through eight hull marks, with distinct ImageGen hulls in vertical and side flight, additional gun mounts and engines. Auxiliary guns add limited support damage, preserving weapon balance and the small player hitbox. The warp close-up uses the current upgraded hull too.



Caverns scroll four times faster (3072 vs 768) with smooth alternating narrow passages and broad chambers. Eight ship stages add distinct vertical/side ImageGen hulls and up to four pairs of light auxiliary guns. Bomber sorties enter from outside the playfield, leave shootable flashing bomblets and retreat; bombs produce eight interceptable fragments and particle shockwaves. Boss shields absorb 24 base pulse hits; once broken, they instantly refill after a fixed six-second exposure window. Super-laser firing windows are invincible, visibly shielded and labelled in the boss HUD; telegraph windows remain attackable.



Flying enemies and bosses now emit rotation-aligned engine particles with periodic boost plumes, white-hot cores and blue glow; transformed bosses gain more exhaust ports. All modern-view hazards use ImageGen rock/ice/fire, mine and barrier artwork, including shootable enemy bomblets.



Damaged boss hull parts emit ImageGen grey smoke puffs from the same locations as fire and electrical sparks. Smoke uses normal alpha compositing, expands and drifts separately from additive engine/fire light, with a fixed 1024-puff budget.



Engine flames, damage sparks and smoke advect with the same downstream world velocity as the scrolling terrain; faster cavern flight and warp acceleration stretch their wakes. Emitted particles retain their own drift and expand/fade independently of the moving hull, in both flight orientations.



Damaged boss hull parts emit ImageGen grey smoke puffs from the same locations as fire and electrical sparks. Smoke uses normal alpha compositing, expands and drifts separately from additive engine/fire light, with a fixed 1024-puff budget.



Bouncing lasers cycle blue, magenta, gold, lime, cyan and violet on enemy/wall rebounds, with thin white cores, coloured glow and three subtle rainbow afterimages. Regular loot now appears every eight kills, and bosses award one alternating power/repair reward: half the prior drop count.



Weapon power progression now requires 2 collected charges for Level 2, another 3 for Level 3 and another 5 for Level 4. Each weapon stores its own charge progress; first acquisition equips the weapon and banks one charge without immediately levelling up. HUD displays the current charge fraction, while level-up messages only appear at the threshold.



Transition camera zoom reduced to approximately half its previous maximum, retaining the smooth close-in and pull-back motion.



Boss damage smoke is substantially denser: roughly three times as many emissions, larger puffs lasting 3-4.5 seconds, and stronger downstream drift. Several damaged parts form separate long grey trails, with reduced emission on lower effects settings and a fixed 1024-puff limit.



Player hull damage also emits wind-driven smoke: light wisps below 75 armour, two smoking damage points at 50 or less, and three dense smoke plumes at critical armour (25 or less). Repairing the hull stops new emission; existing smoke dissipates naturally.



Bosses now have a separate cyan shield meter and pink hull meter. Shields absorb 24 base pulse hits before hull damage becomes possible. Breaking a shield starts a fixed six-second exposure countdown; at zero the shield snaps to full charge, even while hull shots continue. Super-laser firing remains invincible.



Bouncing-laser impact damage reduced by 20 percent (75 to 60 base damage), retaining five-hit rebounds, splitting and prismatic effects.



Storms now include dense full-width barrages requiring a cleared firing lane, with small fragments and large rocks up to 56 units across. Asteroid Surface replaces the former arena sector immediately before Rocky Cavern, with high-speed cratered terrain. Caverns include forked routes around rock islands, bottlenecks down to approximately 20 units of clearance, and varying speed (slower through pinches, faster in open chambers). Unprotected wall collisions destroy one ship and grant a protected respawn in the nearest safe lane.



Boss super lasers now have an 80-unit damaging width (one quarter of the playfield), broad edge warnings, six colour families and variant-specific braided/scanning energy filaments. Boss special devices include six-hit timed super bombs, eight-hit gravity wells and two-hit guided missile racks, all shootable. Timed bombs telegraph their fuse with a shrinking light ring before bursting into sixteen bullets. Caverns widen gradually during boss warnings to provide room to evade wide beams.

Continuous player lasers now use a single sustained energy voice with ignition hiss, FM buzz, filtered plasma noise and a release tail; ricochet lasers use descending energy zaps. Boss beams have a separate rising charge sound and bass-heavy firing roar, using bounded pooled voices and the existing finite audio limiter.



The 17-sector campaign begins Earth Orbit → Deep Space → Asteroid Belt → Fire Storm. It then visits Ice Asteroids, Metal Asteroids, Diamond Asteroids, Uranium Asteroids, Heavy Radiation, Asteroid Surface and Rocky Cavern, followed by the remaining nebula/planet sectors. Each added asteroid sector has distinct ImageGen ore and a unique three-form boss. Metal is tougher, ice breaks readily, and isotope/uranium cores have visible damaging radiation halos. Bouncing-laser volleys now cost six energy, requiring bursts or switching weapons to recharge.



Mission polish: each sector opens with eight seconds of free flight while the female ship computer explains the area and objective. Regular loot now drops every sixteen defeated enemies (halved again). One wave per sector is a 32-drone kamikaze swarm using small ImageGen bug sprites; direct unshielded ship collisions leave armour and energy near empty. Boss destruction sends an expanding screen-clearing orbital shockwave through the playfield, with a whiteout, impact shake, victory fanfare, score/kill/salvage/bonus debrief, fly-off and black fade into the next mission. All cinematics and briefing timers pause with gameplay.



Warp visuals use up to 3,200 deterministic perspective stars in GPU-batched, linearly filtered glow ribbons. Cyan, violet, white and occasional warm stars accelerate through depth during the camera transition. Quality settings scale the field to 1,800 or 900 stars; simulation, pause and camera changes remain independent of the graphical effect. The old pixel warp streak overlay is disabled in modern rendering.



The browser hides the cursor over the active playfield, restoring it on pause. Ship VO focuses on mission briefings, boss and rear-attack warnings, hazards, critical hull/energy and stage completion; item and ship-upgrade chatter is silent, and low-energy reminders are rate limited to thirty seconds.



Scenery pace varies through smoothly blended eight-second phrases, with slower briefings and boss fights and faster open-flight runs. Smoke and flame share the visible foreground cloud/terrain velocity, accounting for side-flight camera scaling. Each run grants three continues: a pause-aware nine-second particle countdown offers recovery with three lives, full hull/energy and three seconds of protection; score entry waits until the final defeat, timeout or Finish Run.



Earth flight replaces stars with three depth layers of rain and three scrolling cloud layers. Cloud cover blends between clear and heavy weather every eight seconds, with faster foreground cloud banks and matching smoke drift.



Regular parallax stars are approximately 25 percent smaller; warp stars keep their cinematic scale. Earth cloud layers scroll at 1400, 2100 and 3100 logical pixels per scenery-clock second, all faster than the 780-pixel ground layer. Rain is faster still, and smoke follows the nearest clouds.



Planet sections automatically climb and descend through smooth altitude phrases. Ground textures zoom around the screen centre from distant high-altitude views to close low passes, while cloud cover thickens when crossing the cloud deck. Ship scale and collision geometry stay consistent.



Weapon and impact SFX pass through a fixed-buffer stereo reverb with damped reflections and diffusion. Caverns smoothly increase wet level and feedback for longer tails. Continuous player and boss lasers additionally use a six-stage 0.10 Hz phaser with 82 percent resonant feedback; dry attacks remain in the mix. Music and voice stay on their separate browser buses.



Each boss displays separate shield and hull counter-weapons, dealing 3× shield damage and 2.5× hull damage. The first boss shield is weak to Pulse and its hull to Death Laser; the second shield is weak to Triple Death Laser and its hull to EMP Storm. Later counters vary across the nine weapons. Nova/support damage and protected boss phases keep their existing rules. Boss warnings smoothly decelerate scenery to a moderate battle pace across every biome, including caverns.



Bouncing-laser launch and rebound sounds bypass the phaser and use a fractional-delay flanger instead: independent 0.14–0.23 Hz sweeps over 0.5–5 ms, with 38 percent damped feedback and the existing room reverb. All delay storage is bounded and preallocated per voice.



Enemy and wall ricochets produce a flanged metallic ping, with quieter echoes at 70 and 140 milliseconds. At most two new reflection pings start per simulation tick to keep dense volleys clear.



Ricochet pitch follows metal, shield, stone, ice/crystal, organic/heat or energy material, with a repeatable enemy-type tuning offset. Asteroid destruction replaces the generic boom with a layered fracture/crunch and low rubble tail; larger rocks sound heavier, while ice and metal have different tuning.



Single laser audio layers root + minor seventh; triple death laser layers root + perfect fifth + minor seventh. Independently wrapped oscillators preserve tuning through phase wraps, normalized before the shared phaser and room reverb.



Solid sprites cast directional opaque silhouette shadows onto other solid sprites. Receiver alpha is preserved with multiplicative compositing; transparent space, pickups and additive weapon/exhaust effects remain clear. Broad-phase overlap checks keep isolated sprites on the original batch path.



Each bouncing-laser launch and reflection ping has its own flanger delay buffer, feedback and independent LFO phase/rate (0.14–0.23 Hz). Material-tuned pings and their short echoes sweep independently; other sound effects and music are unaffected.



The slow flanger LFO uses 64-sample control blocks with per-sample delay interpolation, reducing LFO sine evaluations by about 64 times. Background tiles are selected in inverse-zoom viewport coordinates and culled using their transformed dimensions so altitude changes cannot expose empty scenery.



Player lasers and ricochet pings now use narrow-band filtered noise instead of sine/FM beeps. Single/triple intervals are preserved as two/three tuned noise bands, with independent per-voice filters and flangers on ricochets; material pitch and short reflection echoes remain.



EMP Storm is weapon 9, available by key, wheel, gamepad and unique weapon pickup. Each 9-energy pulse chains through nearby targets, clears ordinary rounds and damages shootable devices. Upgrades increase range, damage and target count. Bombers and selected bosses deploy cyan EMP charges: shoot them within 2.2 seconds or evade their 110-unit pulse. A hit disables firing for 2.5 seconds, preserving movement/shields/nova; a 4.5-second immunity window prevents stun locking.



Bouncing laser launch and reflection levels are reduced relative to the other weapons. Flanger feedback is 38%, with a 65/35 dry/wet mix and stronger high-frequency damping. Each reflection steps through a small note variation around its material/enemy pitch, with a soft tonal ping over the filtered noise and quieter short echoes.



Boss shields and hulls now have distinct weapon weaknesses: matching shield damage is 3× and matching hull damage is 2.5×. The HUD lists both weapons and highlights the active layer; invulnerable entrance, morph and superweapon phases remain protected.



EMP Storm range increased from 85–115 to 125–170 logical units across its four upgrades. Its sound combines a saturated electrical buzz, 50/100 Hz transformer modulation, tuned arcing noise and random crackles, feeding the existing room reverb.



EMP Storm now chains/branches through 8–14 unique enemies per pulse. The first link reaches 125–170 units; subsequent links can jump 75–105 units from any struck enemy, extending beyond the initial radius. Damage tapers 5% per jump to a 45% floor; no target is hit twice per pulse. Visual arcs remain bounded to 24.



On planet missions the player casts a silhouette onto the composited cloud layers. Altitude controls size, offset, opacity and soft edge passes: low flight gives a compact, darker shadow, high flight a larger, softer, displaced one. Cloud alpha is preserved so the shadow cannot darken clear gaps. The existing scratch render target is reused.



Dense connected packs (at least five enemies, average two nearby neighbours) trigger critical EMP feedback. Every on-screen enemy connected by 52–64-unit jumps can join, up to the existing 64-enemy pool. The 2.3–3.2-second autonomous network freezes ordinary enemies, continually recruits nearby arrivals and ramps damage after a short charge-up. Bosses remain mobile and take reduced feedback damage; protected phases remain immune. New pulses cannot endlessly refresh the critical timer. Enemy EMP disruption breaks the network. CPU searches, 96 visual arcs and sound events are bounded.



Crowded-scene shadow passes clear and composite only each transformed sprite bounding rectangle, with clipped caster geometry, rather than copying the entire framebuffer per receiver. This retains opaque silhouette shadows while reducing fill cost in dense critical swarms.



Enemies and transforming bosses now cast altitude-dependent, correctly rotated cloud silhouettes alongside the player. During fast Earth flight, frontal enemy engine trails are suppressed for projectile readability; rear raiders retain stronger twin booster plumes and a positional entry sound. Sample-based boss reactor/computer ambience grows with boss phase and fades on defeat, sharing mute/pause controls.





## Sample-based browser audio



Browser effects now use decoded audio samples only (Kenney/OpenGameArt CC0; Tesla recording by Why_O under CC BY 4.0): gunfire, lasers, material impacts, shields, explosions, bombs, electric buzz, warning and pickup cues, engines, warp and a victory fanfare. Procedural C voices are bypassed in WebAssembly. Pitch shifts, sampled fifth/seventh laser harmonies, per-reflection flangers and short cavern reflections process the samples. Twenty-four bounded one-shot events and category cooldowns prevent dense hits from flooding the mixer. Original source links and licences are in docs/audio/sfx/SOURCES.txt.



Enemy deaths and collision detonations use four explosion size tiers: tiny drones/bugs, standard ships, heavy specialists and bosses. Each regular tier cycles three distinct recorded bang/firework/cannon samples by rubberduck (CC0). Boss detonations add a 5.5-second sampled low-frequency/rocket-engine rumble and reserve a voice even when combat audio is saturated.



Ballistic weapons use isolated real CC0 firearm recordings by Tabasco: alternating SKS shots for standard rounds, CZ-52 for rapid fire, shotgun for spread, and Mosin Nagant for heavy/ricochet shots. Energy weapons keep their separate sci-fi samples.



Progression tuning: regular drops now arrive every 8 kills (previously 16); 5 of each 16 drops are weapon-power pickups. Upgrade requirements remain incremental at 2, 3 and 4 charges (previously 2, 3 and 5).



Tesla playback is reduced from .72 to .42 gain, with quieter 130 ms chain accents, random per-discharge pitch shifts and a subtle 15% wet four-stage allpass phaser. Tesla voices are capped at four during dense feedback chains.





## Torpedoes and material impacts



A forward torpedo automatically launches while fire is held, with a 150-tick (3-second) reload shown on the HUD. It accelerates forward in both flight modes and detonates on enemies, destructible hazards, cavern walls or timeout, dealing 800 damage units at the centre falling to 300 at the 48-unit blast edge. Boss shields, protected phases and enemy guards still apply. It clears ordinary hostile rounds without damaging the player; weak bullets do not intercept the torpedo. Terrain walls remain intact.



Bomb-dropper charges retain their visual explosions and damage but their own impact/detonation sounds are silent. Enemy destruction still uses normal explosion audio. Metal impacts cycle recorded clinks/rings, rock hits use crunching breakage and ice hits use recorded cracking ice. Original source links and CC0 licences are recorded in docs/audio/sfx/SOURCES.txt.





## Weapon banks and deployables



Each main weapon has a separate 100-unit battery, its own recharge rate and a fire cadence that persists while switching. All banks recharge in the background. Running empty triggers a 150-tick (3-second) overheat lockout; swapping weapons cannot bypass it. The weapon strip shows energy, time to full charge and cooldowns. Spread starts with a five-direction semicircle including left/right shots and gains 7/9/12 projectiles, progressively adding rear diagonals and a full circle at maximum rank while retaining useful forward fire. Other projectile weapons gain extra angled shots as they level; single/triple death lasers become wider, brighter and more damaging, with collision widths matching their visuals.



C deploys a chaff-drone charge: 48 friendly proximity mines scatter into a grid and persist up to seven seconds; carry limit two, with rearm drops. E deploys one EMP bomb per mission, freezing all enemy ships (including bosses) and hostile ordnance for four seconds while electrical shocks play over them. Boss invulnerability rules remain intact. Shield drops grant six seconds of invincibility, with at most four shield drops per mission. HUD counters and touch buttons show the new equipment.



Asteroid Belt and Diamond Asteroids now have dedicated ImageGen cavern artwork (Artwork/CAVERN-PROMPTS.json). Their three four-wave legs alternate vertical/horizontal/vertical flight (reversed for Diamond). Turns wait for all enemy ships, hazards and hostile ordnance to leave or be destroyed; no enemies are cleared to force a turn. Cavern walls, islands and collisions use the visible tunnel geometry; the boss gets a wider cavern arena.





### Cinematic title screen

The opening menu uses bespoke built-in ImageGen key art: a silver interceptor above Earth, an asteroid field and a distant alien flagship. The title, launch button, seventeen-mission selector and render-detail selector are real accessible HTML controls. Artwork drifts gently; reduced-motion preferences disable that animation. Mobile uses a taller menu and readable touch targets. Loading and failure messages remain visible. Artwork and full prompts: `docs/images/title-keyart.jpg`, `Artwork/title-concept.jpg`, `Artwork/TITLE-PROMPTS.json`.





Death laser visuals (weapons 4 and 6) use one coherent glowing blade per lane, with a white-hot filament and level-based corona: level 1 blue, level 2 green, level 3 yellow, level 4 red. Width, damage, battery and recharge rules are unchanged.



### Death lasers, painted action bar and Phoenix Mode
Single and triple death lasers replace the old rainbow bands. Their rounded, coherent blades have a white core and change colour by weapon rank: blue, green, yellow, red. The action bar uses nine ImageGen painted weapon icons, corner key numbers, a clockwise recharge wipe and pulsing red overheat borders. On narrow screens the touch weapon buttons beneath the canvas provide the same controls.

Heatseekers detonate once on enemy, projectile, hazard or cavern-wall contact. Their circular blast radius grows from 40 to 58 logical units, dealing their normal missile damage at the centre with up to 65% radial falloff. Nearby ships and destructible objects are affected; protected boss phases and shields remain effective, with heatseeker weakness applied. The player takes no blast damage. Multiple enemy kills from one heatseeker, torpedo, mine, dropper explosion, Tesla discharge or nova grant an additional 125*N*(N-1) score: 250 for two, 750 for three, 1500 for four. Nova awards the increase as its ring expands, avoiding duplicate bonuses. A short BLAST COMBO popup shows the total.

Combat builds the Phoenix meter: ordinary kills add 3%, bosses 20%, with smaller gains from actual damage to hull/shields. At full charge, X or the Phoenix meter button activates a six-second invincible state. The ship transforms into a sleek white ImageGen interceptor, with no bird features. Contact melts ordinary enemies and destructible hazards; nearby hostile rounds burn away. Holding fire releases a free mega death ray spanning approximately half the playfield. Boss shield/phase protection remains. The meter cannot recharge while Phoenix Mode is active; pausing freezes its timer. X is reserved for Phoenix; arrows remain movement controls. Source art and prompts are in Artwork/PHOENIX-INTERCEPTOR-PROMPT.json and Artwork/WEAPON-ICON-PROMPT.json.


Visual clarity update: regular single/triple lasers now use a stronger multi-layer colour halo and feathered edges around the rounded hot core. Tesla arcs have brighter coronas and thicker branches tapering to sharp points, using the existing bounded ribbon batch. Phoenix Mode renders the white interceptor at the current normal ship's size, including upgrade and flight-mode scaling; its aura and ray origin follow the smaller silhouette. Cavern backdrops are darker and softened by two drifting mist layers behind the solid walls, while the collision edges receive a subtle cool rim light. Damage, beam collision widths and cavern collision geometry are unchanged.

Large player lasers also cast a subtle colour-matched cone and ambient spill onto the scene. Sprite masks intersected by the beam receive an additive material highlight, including enemy hulls, shields, pickups and destructible hazards. The lighting is clipped to the playfield and uses bounded existing geometry batches; the HUD is unaffected.

Steering smoothly aims single, triple and Phoenix laser beams toward lateral movement, up to 45 degrees from the flight direction. Triple beam lanes are individually clamped to that limit. The beam centre, perpendicular collision distance, touched-object highlight and scenery light share the same aiming geometry. Releasing lateral input smoothly returns the beams forward. In side flight, vertical movement supplies the lateral aim.

Weapon battery balance: recharge is now three times slower for every weapon (empty-to-full 12–36 seconds, depending on weapon), and running empty triggers a six-second overheat lockout. Inactive weapons continue recharging independently; damage and firing cadence are unchanged. The action-bar clock wipe uses the same six-second duration.

Phoenix charge now requires five times the prior combat contribution: 5000 charge units rather than 1000. Kill and damage contributions accumulate at their previous precision, and the HUD reads the authoritative maximum from the game, so a formerly full meter now represents 20% progress.

Tesla critical feedback also consumes its own battery while the network remains active, including after switching weapons; exhausting it ends the chain and starts the six-second lockout.

HUD placement: the Phoenix meter now occupies a compact strip at the very top, above the existing status counters. Weapon ability icons sit flush against the bottom edge in desktop, fullscreen and mobile layouts; the sector/status line sits just above them.


High-score restart registration now uses the current mission number, rather than the internal scenery id. New run registrations carry a client version; the service recognizes legacy restarted sessions and maps their scenery id back to the mission route when needed. Failed submissions keep the pending result for retry, and identical pilot/score/sector results are unique in the database. Regression checks cover restart registration, old-session recovery, new-session validation, retry preservation and duplicate saves.


### Browser pacing and supplies
The browser follows every requestAnimationFrame callback, with no fixed 60, 120, or 200 FPS cap. SDL's deferred startup swap setting is consumed before the animation loop starts, and simulated presentation waits remain disabled. Simulation stays fixed at 50 Hz with interpolation. Regular loot cadence increases from 10 to 11 drops per 80 kills (+10%), preserving the item cycle and separate boss/shield rewards. Nova pickups cap at five; continuing restores five nova bombs, while a fresh game still starts with three.

Supply regression checks: compile tests/supplies.c with Emscripten and --use-port=sdl3 -sENVIRONMENT=node, then run the generated JavaScript with Node. The checks cover stock boundaries, continue restoration, actual kill/drop cadence and item order.

Weapon firing and Tesla critical-feedback battery drain are now 10% lower, using separate fractional accounting for each battery. Fire cadence, damage, passive recharge, and six-second overheat cooldown are unchanged. This gives roughly 11% more firing time before recharge effects, subject to whole-shot boundaries.

Browser sample voices now leave the admission budget according to AudioContext time, including when main-thread completion callbacks are delayed. Effect tails and oscillator stops are bounded independently of those callbacks. The obsolete silent SDL audio device is not opened in the web build. Audio regression checks: node tests/audio-bus.cjs (synthetic cadence tests, not a claim of measured high-refresh hardware playback).

### Sustained batteries and mission progression
Weapons 1-3 now sustain approximately 25 / 12 / 10 seconds of uninterrupted firing from a full battery, including passive recharge and no pickups. Game-step tests measured Pulse 24.80-24.82s, Spread 11.78-11.86s, and Rapid 9.92-9.94s across all four ranks. Fractional shot cost follows the upgraded fire interval, retaining damage and rate upgrades without reducing battery duration. Weapons 4-9, recharge periods and overheat cooldown are unchanged.

Missions 1-3 retain their original attack pace. Mission-number progression then reduces formation spacing from 170 to 128 simulation ticks, common enemy firing spacing from 128 to 86 ticks, and boss attack intervals by up to 28%. Late regular/specialist formations gain bounded extra enemies; occasional aimed flanking pairs and at most 14% more hostile projectile speed add pressure. Existing boss tells, scenery/cavern motion and collisions remain intact; no new health inflation is added. Encounter deadlines follow formation spacing to avoid longer empty waits. Drops remain 11 per 80 kills in the same item order. Compile/run tests/balance.c with the same Emscripten Node target to reproduce game-step battery durations and controlled early/middle/late shot counts.


Release 81 strengthens upgraded Triple Death Laser levels: beam half-widths are 3/8/16/25 world units and base enemy damage is 110/155/215/290. Target paths and smaller pod echoes share the upgraded power. Fine-wall beams stop and cut across their visible aperture, including thin grains between former sample rays. All weapon-bank costs, recharge and cooldowns remain unchanged.


Release 82 gives Heatseekers heavier impacts and a larger real blast, with bounded splash against enemies, environmental targets and exposed giant parts. Tiny red brackets show actual acquired targets; short red trails follow travelled guidance paths. Protected or occluded targets hide their cue, target selection and steering stay unchanged, and reduced motion keeps brackets steady. Costs, deliberate firing cadence and boss gates remain intact.
