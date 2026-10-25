(() => {
  const panel = document.createElement('details');
  panel.id = 'frame-diagnostics';
  panel.innerHTML = '<summary>Frame diagnostics</summary><p>Compare browser timing with game rendering in this window. Keep this tab visible.</p><output aria-live="off">Collecting timing…</output><p><button type="button">Test display refresh without game rendering</button></p><p class="frame-help">The display test temporarily freezes gameplay. Return to the game to resume. If this test stays at 60 Hz, the limit persists with game rendering stopped.</p>';
  document.querySelector('header').after(panel);
  const output = panel.querySelector('output');
  const button = panel.querySelector('button');
  const cabinet = document.querySelector('#cabinet');
  let raf = 0, idle = false, resumeLoop = false, oldVisibility = '', samples = [], costs = [], begin = 0, hooked = false, lastPaint = 0;
  function loop() { return typeof MainLoop === 'undefined' ? null : MainLoop; }
  function reset() { samples = []; costs = []; }
  function restore() {
    if (!idle) return;
    cabinet.style.visibility = oldVisibility;
    if (resumeLoop) loop().resume();
    idle = resumeLoop = false;
    button.textContent = 'Test display refresh without game rendering';
    reset();
  }
  button.onclick = () => {
    if (idle) { restore(); return; }
    const main = loop();
    resumeLoop = !!(main && main.func && main.scheduler);
    if (resumeLoop) main.pause();
    oldVisibility = cabinet.style.visibility;
    cabinet.style.visibility = 'hidden';
    idle = true;
    button.textContent = 'Return to game';
    reset();
  };
  function tick(t) {
    if (!panel.open) return;
    const main = loop();
    if (main && !hooked) {
      main.preMainLoop.push(() => { if (panel.open) begin = performance.now(); });
      main.postMainLoop.push(() => {
        if (panel.open) { costs.push(performance.now() - begin); if (costs.length > 240) costs.shift(); }
      });
      hooked = true;
    }
    const frames = typeof Module._web_stat === 'function' && main?.func ? Module._web_stat(13) : 0;
    samples.push({t, frames});
    while (samples.length > 2 && t - samples[0].t > 2000) samples.shift();
    if (samples.length >= 15 && t-lastPaint >= 250) {
      lastPaint = t;
      const first = samples[0], elapsed = t - first.t;
      const hz = (samples.length - 1) * 1000 / elapsed;
      const rendered = (frames - first.frames) * 1000 / elapsed;
      const sorted = costs.slice().sort((a,b) => a-b);
      const mean = costs.length ? costs.reduce((a,b) => a+b,0)/costs.length : 0;
      const p95 = sorted.length ? sorted[Math.min(sorted.length-1,Math.floor(sorted.length*.95))] : 0;
      const vsync = main?.func ? Module._web_stat(155) : -1;
      const scheduler = main?.scheduler ? (main.timingMode === 1 ? 'animation frame / divisor '+main.timingValue : 'timer / '+main.timingValue+' ms') : 'stopped';
      output.textContent = `${idle ? 'DISPLAY TEST — GAME FROZEN' : 'GAME RENDERING'}\nBrowser callbacks: ${hz.toFixed(1)} Hz\nRendered frames: ${rendered.toFixed(1)} FPS\nFrame work: ${mean.toFixed(2)} ms average / ${p95.toFixed(2)} ms p95\nSDL VSync request: ${vsync < 0 ? 'not started' : vsync ? 'on' : 'off'}\nScheduler: ${scheduler}\nTab: ${document.visibilityState}`;
    }
    raf = requestAnimationFrame(tick);
  }
  panel.addEventListener('toggle', () => {
    cancelAnimationFrame(raf);
    if (panel.open) { reset(); raf = requestAnimationFrame(tick); }
    else restore();
  });
  if (new URLSearchParams(location.search).has('diagnostics')) panel.open = true;
})();
