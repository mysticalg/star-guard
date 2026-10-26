/* Simulation-owned ambient: smooth across section and weather transitions. */
static float lighting_target(void){
 static const float ambient[19]={.98f,.94f,.37f,.30f,.34f,.42f,.49f,.78f,.35f,.65f,.28f,.28f,.38f,.34f,.76f,.76f,.61f,.44f,.82f};
 static const float phrase[5]={1,.84f,.98f,.74f,1.04f};
 int kind=clamp(journey_kind(),0,18),part=(ex.progressTicks/1500+ex.section*2+g.wave)%5;
 float value=ambient[kind]*phrase[part];
 if(ex.env.weather.kind==WEATHER_STORM&&ex.env.weather.stage>0)value*=ex.env.weather.stage==2?.65f:.83f;
 if(ex.giant.active||ex.bossSpawned){if(value<.42f)value=.42f;}
 return value<.25f?.25f:value>1?1:value;
}
static void lighting_step(void){ex.lightTarget=lighting_target();if(!ex.lightReady){ex.lightAmbient=ex.lightTarget;ex.lightReady=1;}else ex.lightAmbient+=(ex.lightTarget-ex.lightAmbient)*.02f;}
