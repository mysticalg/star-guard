#define main star_guard_main
#include "../src/pc.c"
#undef main

int main(void) {
    int checks=0, actualDrops=0;
    static const int loot[16]={10,7,6,11,7,8,10,7,9,12,7,6,2,7,5,4};
    #define VERIFY(x) do { if (!(x)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x); return 1; } checks++; } while(0)
    headless=1;
    game_reset();
    VERIFY(ex.bombs==3);
    for (int stock=0;stock<=5;stock++) {
        game_reset();g.mode=1;g.clock=100;g.groups=SECTOR_WAVES;g.shield=100000;ex.bombs=stock;
        drop(g.px/Q,player_y/Q,5);game_step(0,0);
        VERIFY(ex.bombs==(stock==5?5:stock+1));
        g.mode=3;g.continues=3;ex.bombs=stock;
        game_step(0,13);
        VERIFY(g.mode==1&&g.continues==2&&g.lives==3&&ex.bombs==5);
    }
    game_reset();g.mode=1;ex.shieldDrops=4;
    for (int kill=1;kill<=800;kill++) {
        for(int d=0;d<8;d++)g.drops[d].on=0;
        ex.stageKills=g.kills;ex.shieldDrops=0;ex.chaff=0;
        Obj enemy={.on=1,.type=0,.x=160*Q,.y=100*Q};
        killed(&enemy);
        int count=0,type=-1;
        for(int d=0;d<8;d++)if(g.drops[d].on){count++;type=g.drops[d].type;}
        int expected=(kill*11/80)>((kill-1)*11/80);
        VERIFY(count==expected);
        if(count){VERIFY(type==loot[actualDrops%16]);actualDrops++;}
    }
    VERIFY(actualDrops==110);
    VERIFY(regular_drop_count(800)==110);
    printf("PASS %d checks: initial bombs 3, pickup cap 5, continue refill 5, 110 drops in 800 kills, unchanged 16-item cycle\n",checks);
    return 0;
}
