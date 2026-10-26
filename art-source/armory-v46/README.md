# Star Guard armory / hangar art, release 46

All PNGs are original built-in imagegen outputs. Exact prompts, original output paths and SHA-256 receipts are in generation.json and extra-generation.json. Originals were copied intact. Concepts are design references, never embedded UI screenshots.

## Mapping

* ship-0: Vector / Balanced; ship-1: Jade / Energy; ship-2: Bastion / Armour. These are hangar portraits. Actual flight sprite crops are separately mapped in player-preview-mapping.json from the shipping player-ships-v2 atlas and playerArtUV.
* weapon-0 through weapon-8: Pulse, Spread, Rapid, Death Laser, Heatseeker, Triple Death Laser, Ricochet, Bouncing Laser, EMP Storm, in that order. Each family icon is shared by its three variants and three chassis: 9 x 3 x 3 = all 81 named items. Variant name/selection text and chassis/variant colour edging identify each real item; artwork does not imply extra projectiles or different mechanics. Prices, ownership, recipes and equip compatibility come from existing PilotStore/WeaponVariants data.
* armour-0 through armour-3: Standard, Laminate, Bulwark, Adaptive. The earlier armour.png exploration is preserved but not used in the final cards.
* resources.png: one generated strip, split into credits, parts, cores (left to right), inspected against the original. Cosmetics reuse the chosen ship portrait plus the actual skin label and lock/equip state; no skin bonus is implied.
* range-deck and range-target: purely visual workshop practice environment. No collision logic or damage model comes from the painting.

## Packing

Transparent icons: find structural alpha >=16 bounds, crop with 8px source margin, preserve original RGBA within the crop, fit within 224x224 on a transparent 256x256 canvas, WebP quality88. This threshold locates bounds only; it never converts RGB into alpha or removes pixels within the crop. Resource derivatives are 96x96 with 80px object bounds. Ship portrait derivatives retain the full original photo aspect ratio, at most720x480, WebP86. Equal-size CSS frames use object-fit:contain with8px desktop/4-5px phone safe padding and a dim blurred scene extension behind the photo, preserving complete ships without hard black letterbox gaps. No hover/parallax zoom is used. Deck is at most 960x640, WebP86. Player crops retain source RGBA and use lossless WebP. See packed manifests and final web-manifest.json for sizes, alpha, source mapping and hashes.

Rajdhani Bold is bundled locally under its SIL Open Font License (docs/fonts/OFL.txt); original TTF is preserved here, web WOFF2 only is referenced. Source: https://github.com/google/fonts/tree/main/ofl/rajdhani .

## Preview implementation

src/armory_range.c includes the existing pc.c/game.c code in a separate WASM module, uses production game_step at 50Hz and reads real shot coordinates/velocity, production projectile colours, beam geometry/colours, EMP endpoints and blast wave ages/radii. Three durable targets are a sandbox fixture. Preview starts at rank1, selected variant and actual gear quality/armour; unowned equipment is labelled Standard sample. Auxiliary torpedoes/pods/world encounters are excluded so the selected weapon stays clear. It resets after approximately6 seconds. The menu canvas is a compact faithful presentation renderer, not a full second SDL mission. Separate WASM memory, headless silent audio, no pilot/storage/score APIs, no automatic main loop. rAF/observers/listeners are stopped on disposal; visibility, intersection and reduced motion gate playback.

The UI omits the old projectile-speed percentage/description from armory cards: the current production fan_fire path does not apply the legacy metadata speed multiplier to its emitted shots. No balance change was made to compensate; the preview faithfully runs the production behavior. Damage and energy figures retain actual existing quality scaling. Full legacy metadata and loadout descriptions remain untouched.

Rebuild the range through build.py (EMCC environment override supported). Tests: node tests/armory-range.cjs; node tests/armory-ui.cjs; node tests/armory-audit.cjs. Browser tests use isolated browser storage and mock score requests.
