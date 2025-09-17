// Sprite demo for Super Famicom
// by Takuya Matsubara
#include <snes.h>

extern char gfxpsrite,gfxpsrite_end;
extern char palsprite,palsprite_end;
extern char tilfont, palfont;
extern char tilfont_end, palfont_end;

// スプライト
#define SPRITE_MAX 128 // スプライト最大数
#define SP_PRIO 2      // 優先度

// スプライトの座標
s16 spr_x[SPRITE_MAX];
s16 spr_y[SPRITE_MAX];
s16 spr_dx[SPRITE_MAX];
s16 spr_dy[SPRITE_MAX];

char tempstr[6];
u16 tick_old;
u16 tick_now;
u8 framecnt;

// 移動量テーブル
const s16 x1[8]={ 0, 1, 1, 1, 0,-1,-1,-1};
const s16 y1[8]={-1,-1, 0, 1, 1, 1, 0,-1};

// パレット番号
#define PAL_TXT    1 // テキスト
#define PAL_SP     0 // スプライト

// BG番号
#define BGN_TXT    0 // テキスト

#define VRAM_TXTOFS  0x0000 // TEXTタイル番号オフセット

// VRAM 0x0000-7FFF
#define VRAM_TXTMAP  0x2800 // テキストのタイルマップ
#define VRAM_BGTPTN  0x2000 // テキストのパターン
#define VRAM_TXTPTN  0x2000 // テキストのパターン+オフセット
#define VRAM_SPPTN   0x0000 // スプライトのパターン

// 10進数3ケタ変換(格納先ポインタ,数値0～255)
void u8_to_str(char *str,unsigned char num)
{
    char i=3;
    str += 3;
    *str = 0; // NULL
    str--;
    while(i--){
        *str = (num % 10)+'0';
        num /= 10;
        str--;
    }
}

#define TIMEOUT 600  // 10秒×60フレーム

// fps測定
void sprite_test( u8 sp_size, u8 sp_max ) {
    u8 i;
    u16 tick_start;

     // スプライトを非表示
    for(i=0; i<SPRITE_MAX; i++) {
        oamSetEx(i*4, sp_size , OBJ_HIDE);
    }
    WaitForVBlank();

    for(i=0; i<sp_max; i++) {
        int direction;
        u8 v_flip,h_flip;
        u16 gfxofs;

        spr_x[i] = (rand() % (240-32))+8;
        spr_y[i] = (rand() % (208-32))+8;

        direction = rand() % 8;
        spr_dx[i] = x1[direction];
        spr_dy[i] = y1[direction];

        v_flip = rand() & 1;
        h_flip = rand() & 1;

        if(sp_size == OBJ_SMALL){
            gfxofs = (rand() % 8)*2;
            gfxofs += (rand() % 16)*32;
        }else{
            gfxofs = (rand() % 4)*4;
            gfxofs += (rand() % 8)*64;
        }

        oamSet(
            i*4,
            spr_x[i],
            spr_y[i],
            SP_PRIO,  // 優先度
            v_flip,  // 上下反転
            h_flip,  // 左右反転
            gfxofs,  // タイル番号
            PAL_SP   // パレット番号
        );
        oamSetEx(i*4, sp_size, OBJ_SHOW); // 表示
    }

    consoleDrawText(7, 4, "SPRITE DEMO");
    consoleDrawText(7, 6, " for SUPER FAMICOM");
    if(sp_size == OBJ_SMALL){
        consoleDrawText(10, 10, "SIZE:16x16PIXEL");
    }else{
        consoleDrawText(10, 10, "SIZE:32x32PIXEL");
    }
    consoleDrawText(10, 13, "OBJECTS:");
    u8_to_str(tempstr,sp_max);
    consoleDrawText(18, 13, tempstr);
    consoleDrawText(10, 16, "FRAME/SEC:");
    WaitForVBlank();

    // fps測定
    framecnt = 0;
    tick_start = snes_vblank_count;
    tick_old = tick_start;
    while(1) {
        tick_now = snes_vblank_count;
        if(tick_now < tick_old){
            tick_old = tick_now;
            framecnt = 0;
            continue;
        }
        if((tick_now - tick_old) >= 60){ // 1秒経過
            tick_old = tick_now;
            u8_to_str(tempstr,framecnt);
            consoleDrawText(20, 16, tempstr);
            framecnt = 0;
        }
        for(i=0; i<sp_max; i++) {
            // 座標更新
            spr_x[i] += spr_dx[i];
            spr_y[i] += spr_dy[i];

            // バウンド
            if(spr_x[i] <= 0 || spr_x[i] >= 240) spr_dx[i] *= -1;
            if(spr_y[i] <= 0 || spr_y[i] >= 208) spr_dy[i] *= -1;

            oamSetXY(i*4, spr_x[i], spr_y[i]);
        }
        if((tick_now - tick_start) >= TIMEOUT)break;
        framecnt++;
        WaitForVBlank();
    }
}

// ----メイン
int main(void) {
    u8 i;

    // テキスト初期化
    consoleInit();
    consoleSetTextMapPtr(VRAM_TXTMAP); // タイルマップ
    consoleSetTextGfxPtr(VRAM_TXTPTN); // パターン
    consoleSetTextOffset(VRAM_TXTOFS); // TEXTタイル番号オフセット
    // テキスト用パターン＆パレット設定
    consoleInitText(
        PAL_TXT,  // パレット番号
        (&palfont_end - &palfont),  // パレットのサイズ
        &tilfont, // パターンのデータ
        &palfont  // パレットのデータ
    );

    // テキスト用 BG初期化
    bgSetGfxPtr(BGN_TXT, VRAM_BGTPTN);
    bgSetMapPtr(BGN_TXT, VRAM_TXTMAP, SC_32x32);

    // スプライト初期化
    oamInitGfxSet(
        &gfxpsrite,      // スプライトタイルデータ
        (&gfxpsrite_end - &gfxpsrite),    // パターンのサイズ
        &palsprite,      // パレットのデータ
        (&palsprite_end - &palsprite),    // パレットのサイズ
        PAL_SP,              // パレット番号
        VRAM_SPPTN,          // パターンのアドレス
        OBJ_SIZE16_L32       // スプライトのサイズ
    );
    setMode(BG_MODE1,0);
    bgSetDisable(1);
    bgSetDisable(2);
    WaitForVBlank();
    setScreenOn();

    while(1){
        sprite_test(OBJ_SMALL, 32);
        sprite_test(OBJ_SMALL, 48);
        sprite_test(OBJ_SMALL, 64);
//        sprite_test(OBJ_SMALL, 128);
        sprite_test(OBJ_LARGE, 32);
        sprite_test(OBJ_LARGE, 48);
        sprite_test(OBJ_LARGE, 64);
//        sprite_test(OBJ_LARGE, 128);
    }
    return 0;
}