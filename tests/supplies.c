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
    static const int sustained[9]={37,22,56,37,14,22,28,14,9};
    for(int w=0;w<9;w++) {
        game_reset();int consumed=0;
        for(int shot=0;shot<100;shot++) {
            g.weaponEnergy[w]=100;
            VERIFY(weapon_spend(w));
            consumed+=100-g.weaponEnergy[w];
            VERIFY(g.weaponEnergy[w]>=0&&g.weaponEnergy[w]<100);
            VERIFY(g.weaponDrainRemainder[w]>=0&&g.weaponDrainRemainder[w]<10);
        }
        VERIFY(consumed==weapon_cost(w)*90);
        VERIFY(g.weaponDrainRemainder[w]==0);
        weapon_batteries_reset();int shots=0;
        while(weapon_spend(w)){shots++;if(g.weaponHeat[w])break;}
        VERIFY(shots==sustained[w]);
        VERIFY(g.weaponEnergy[w]==0&&g.weaponHeat[w]==300);
        VERIFY(!weapon_spend(w));
    }
    game_reset();int feedbackCost=0;
    for(int pulse=0;pulse<100;pulse++) {
        g.weaponEnergy[8]=100;
        VERIFY(weapon_drain(8,3));feedbackCost+=100-g.weaponEnergy[8];
    }
    VERIFY(feedbackCost==270);
    VERIFY(g.weaponDrainRemainder[8]==0);
    g.weaponDrainRemainder[4]=9;weapon_batteries_reset();
    for(int w=0;w<9;w++)VERIFY(g.weaponDrainRemainder[w]==0&&g.weaponEnergy[w]==100);
    printf("PASS %d checks: bomb stock/continue, loot cadence/order, exact 10%% lower drain for all banks and Tesla feedback, sustained firing and overheat/reset\n",checks);
    return 0;
}
