
#include "os_call.c"
#include "disp.c"
#include "string.c"


#define macrox(pos) ((pos)%6)
#define macroy(pos) ((pos)/6)
#define macropos(x,y) ((x)+(y)*6)
#define U (-6)
#define D 6
#define L (-1)
#define R 1
#define WHITE	0
#define BLACK	1


char hako_ban_dat[7][6]={
	-100, -100, -100, -100, -100,-100,
	-100,   11,   22,   22,   31,-100,
	-100,   11,   22,   22,   31,-100,
	-100,   41,   53,   53,   61,-100,
	-100,   41,   74,   84,   61,-100,
	-100,   94,    0,    0,  104,-100,
	-100, -100, -100, -100, -100,-100,
};

char hako_ban[7][6];	//	aab ... 十進数
//	hako_ban[y][x]
//	a:1～10 通し番号
//	b:1～4  駒番号

char hako_koma_pat[5][2][2]={
	
		0,0,			// dummy data
		0,0,
	
	
		1,0,			// 駒番号１
		1,0,
	
	
		1,1,			// 駒番号２
		1,1,
	
	
		1,1,			// 駒番号３
		0,0,
	
	
		1,0,			// 駒番号４
		0,0,
	
};

//char *hako_banp = (char*)hako_ban;
#define hako_banp ((char*)hako_ban)


//int hako_put_koma_test(char ban[7][6], int x, int y, int koma_numb);
//void hako_put_koma(char ban[7][6], int x, int y, int koma);
//void hako_remove_koma(char ban[7][6], int koma);
//void hako_disp_ban();

/********************************************************************************/
/*		hako_play																*/
/********************************************************************************/
int main()
{
	int cursor, key, x, y, nextx, nexty, koma, koma_numb;
	
	cursor = 1;								// 動かす駒の通し番号
	memcpy(hako_ban, hako_ban_dat, sizeof(hako_ban));
	for(;;){
		scrn_clear();
		printf("F9/F10: select, ﾔｼﾞﾙｼ key: move\n");
		hako_disp_ban();					// 表示
		for(x=1; x<=4; x++){
			for(y=1; y<=5; y++){
				if(cursor==hako_ban[y][x]/10)
					goto next1;
			}
		}
next1:;
		lcd_clearA(80+(x-1)*40+15, 20+(y-1)*40+15, 80+x*40-1-15, 20+y*40-1-15, WHITE);
		key = ut_getc();
		if(key==F10){
			if(++cursor > 10)
				cursor = 1;
			continue;
		}
		else if(key==F9){
			if(--cursor < 1)
				cursor = 10;
			continue;
		}
		
		/***	駒を移動する	***/
		koma = (unsigned char)hako_ban[y][x];
		koma_numb = koma % 10;
		while(koma==(unsigned char)hako_ban[y][x-1]){	// x,y を左上にする
			x--;
		}
		while(koma==(unsigned char)hako_ban[y-1][x]){
			y--;
		}
		
		nextx = x;
		nexty = y;
		if(key==DOWN)
			nexty = y+1;
		else if(key==UP)
			nexty = y-1;
		else if(key==RIGHT)
			nextx = x+1;
		else if(key==LEFT)
			nextx = x-1;
        else
            continue;
		/***	移動できるかテスト	***/
		hako_remove_koma(hako_ban, koma);
		if(hako_put_koma_test(hako_ban, nextx, nexty, koma_numb)){	// 移動できる
			x = nextx;
			y = nexty;
		}
		hako_put_koma(hako_ban, x, y, koma);	// 駒を置く
		hako_disp_ban();
	}
}
/********************************************************************************/
/*		hako_put_koma_test														*/
/*		return 1: 置ける、0:置けない											*/
/********************************************************************************/
int hako_put_koma_test(char ban[7][6], int x, int y, int koma_numb)
{
	int x1, y1;
	
	for(y1=0; y1<2; y1++){
		for(x1=0; x1<2; x1++){
			if(ban[y+y1][x+x1] && hako_koma_pat[koma_numb][y1][x1])
				return 0;				// 置けない
		}
	}
	return 1;							// 置ける
}
/********************************************************************************/
/*		hako_put_koma															*/
/********************************************************************************/
void hako_put_koma(char ban[7][6], int x, int y, int koma)
{
	int koma_numb;
	int x1, y1;
	
	koma_numb = koma % 10;
	for(y1=0; y1<2; y1++){
		for(x1=0; x1<2; x1++){
			if(hako_koma_pat[koma_numb][y1][x1])
				ban[y+y1][x+x1] = koma;	// put koma
		}
	}
}
/********************************************************************************/
/*		hako_remove_koma														*/
/********************************************************************************/
void hako_remove_koma(char ban[7][6], int koma)
{
	int x, y;
	
	for(y=0; y<7; y++){
		for(x=0; x<6; x++){
			if(ban[y][x]==koma)
				ban[y][x] = 0;			// remove
		}
	}
}
/********************************************************************************/
/*		lcd_clearA																*/
/********************************************************************************/
void lcd_clearA(int x1, int y1, int x2, int y2, int color)
{
	int x, y;
	
	for(x=x1; x<=x2; x++){
		for(y=y1; y<=y2; y++){
			Pset(x, y, color);
		}
	}
}
/********************************************************************************/
/*		hako_disp_ban															*/
/********************************************************************************/
void hako_disp_ban()
{
	int i, x, y, k, p[5], pcnt, pos, u, d, r, l, color;
	
	for(k=0; k<=10; k++){					// 駒ループ
		memset(p, 0, sizeof(p));
		pcnt = 0;
		for(pos=0; pos<7*6; pos++){
			if(k==hako_banp[pos]/10){
				p[pcnt++] = pos;
				if(pcnt>=5){
					printf("hako_disp_ban err k=%d\n", k);
					for(;;) ;
				}
			}
		}
		color = k==0 ? WHITE : BLACK;
//		color = k==0 ? WHITE : hako_color[k];
		for(i=0; i<pcnt; i++){
			x = macrox(p[i]);
			y = macroy(p[i]);
			lcd_clearA(80+(x-1)*40, 20+(y-1)*40, 80+x*40-1, 20+y*40-1, color);
			if(k){
				u = d = r = l = 1;
				if(hako_banp[p[i]+U]/10==k)		// 同じ駒が上に続く
					u = 0;
				if(hako_banp[p[i]+D]/10==k)
					d = 0;
				if(hako_banp[p[i]+R]/10==k)
					r = 0;
				if(hako_banp[p[i]+L]/10==k)
					l = 0;
				if(u)
					lcd_clearA(80+(x-1)*40, 20+(y-1)*40, 80+x*40-1, 20+(y-1)*40, WHITE);
				if(d)
					lcd_clearA(80+(x-1)*40, 20+y*40-1, 80+x*40-1, 20+y*40-1, WHITE);
				if(l)
					lcd_clearA(80+(x-1)*40, 20+(y-1)*40, 80+(x-1)*40, 20+y*40-1, WHITE);
				if(r)
					lcd_clearA(80+x*40-1, 20+(y-1)*40, 80+x*40-1, 20+y*40-1, WHITE);
			}
		}
	}
	lcd_clearA(80-1, 20-1, 80+160, 20-1, WHITE);
	lcd_clearA(80-1, 20+200, 80+160, 20+200, WHITE);
	lcd_clearA(80-1, 20-1, 80-1, 20+200, WHITE);
	lcd_clearA(80+160, 20-1, 80+160, 20+200, WHITE);
}
