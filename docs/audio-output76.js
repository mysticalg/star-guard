/* Music, speech, weather and combat share one final output and headroom. */
window.GameAudio76=(()=>{
 let context,input,output;const media=new WeakMap();
 function get(){if(!context){context=new AudioContext();input=context.createDynamicsCompressor();input.threshold.value=-5;input.knee.value=3;input.ratio.value=16;input.attack.value=.001;input.release.value=.12;output=context.createGain();output.gain.value=.8;input.connect(output);output.connect(context.destination);}return{context,input};}
 function attachMedia(element){if(media.has(element))return;const a=get(),source=a.context.createMediaElementSource(element);source.connect(a.input);media.set(element,source);}
 return{get,attachMedia};
})();
