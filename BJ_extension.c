#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define CARDNUMMAX 52		//トランプの枚数
#define CARDMARKMAX 4		//カードのマーク
#define HANDMAX 11			//11枚が最大なので
#define SHUFFLENUM 100		//シャッフル時の入れ替え回数
#define SHUFFLTIME 5		//シャッフル時間(s)
#define PLAYERNUM 4			//最大プレイ人数
#define INITALMONEY 100		//所持金の初期値
#define sleep(n) Sleep(n * 1000)	//sec msec置き換え

//手持ちカードの構造体
typedef struct{
	int cardnumber[HANDMAX];	//手持ちカードの数値
	int cardposition;			//手持ちカード枚数
	int sum;					//手持ちの合計点数
	int burst;					//バーストフラグ
	int Aflag;					//Aの枚数フラグ
	int betcoin;				//所持金
}Card;

int maindeck[CARDNUMMAX];			//山札の設定
char mark[]="SHCD";					//S:スペード, H:ハート, C:クローバー, D:ダイヤ
char number[]="A23456789XJQK";		//1-13番
int cnt=0;							//山札から引いた回数
int bet[PLAYERNUM];					//掛け金
char HITSTAND[]="A";				//ユーザーのヒットスタンドの選択初期設定

//山札の初期化
void CardInitial(){
	int randnum=rand()%CARDNUMMAX;			//スタートポジション
	for(int i=0; i<CARDNUMMAX; i++)
		maindeck[i]=(i+randnum)%CARDNUMMAX+1;
}

//山札のシャッフル
void CardShuffle(){
	int pos;
	int second = 1;

	printf("カードをシャッフル中"); fflush(0);

	for(int j=0; j<SHUFFLTIME; j++){
		sleep(second);
		pos=rand()%CARDNUMMAX;
		putchar('*'); fflush(0);
		for(int i=0; i<SHUFFLENUM; i++){
			int tmp;
			tmp = maindeck[pos];
			maindeck[pos]=maindeck[(pos*pos)%CARDNUMMAX];
			maindeck[(pos*pos)%CARDNUMMAX]=tmp;
			pos = (pos+1)%CARDNUMMAX;
		}
	}
	putchar('\n');
}

//山札の表示
void PrintMainCard(){
	for(int i=0; i<CARDNUMMAX; i++){
		printf("   [%c %c]\n", mark[(maindeck[i]-1)/13],
				number[(maindeck[i]-1)%13]);
	}
}

//ステータスの初期化
void InitialStatus(Card *hand){
	hand->cardposition=0;		//ヒット回数の初期化（最初の2枚を含む）
	hand->sum=0;				//カードの合計値を初期化
	hand->burst=0;				//バーストフラグOFF
	hand->Aflag=0;				//Aの枚数
	hand->betcoin=INITALMONEY;	//100＄スタート
}

//継続時のステータス初期化
void ContinueInitialStatus(Card *hand){
	hand->cardposition=0;		//ヒット回数の初期化（最初の2枚を含む）
	hand->sum=0;				//カードの合計値を初期化
	hand->burst=0;				//バーストフラグOFF
	hand->Aflag=0;				//Aの枚数
}

//カードを引く
void HitCard(Card *hand){
	hand->cardnumber[hand->cardposition] = maindeck[cnt];
	hand->cardposition+=1;
	cnt++;
}

//手持ちカード内容の表示
void PrintCard(Card *hand){
	for(int i=0; i < hand->cardposition; i++){
		printf("   [%c %c]\n", mark[((hand->cardnumber[i]-1)/13)],
				number[(hand->cardnumber[i]-1)%13]);		//マークと番号の表示
	}
}

//手持ちカードの点数計算
void CalHandCard(Card *hand){
	hand->sum=0;	//合計値の初期化
	hand->Aflag=0;	//Aフラグの初期化
	for(int i=0; i < hand->cardposition; i++){
		if(((hand->cardnumber[i]-1)%13+1)>10){			//JQKの場合
			hand->sum+=10;
		}else if(((hand->cardnumber[i]-1)%13+1)==1){	//Aの場合
			hand->sum+=(hand->cardnumber[i]-1)%13+1;
			hand->Aflag+=1;
		}else{
			hand->sum+=(hand->cardnumber[i]-1)%13+1;
		}
	}
}

//点数の読み出し
void PrintHandCard(Card *hand){
	CalHandCard(hand);		//手持ち計算
	if(hand->Aflag>0){
		printf("プレイヤーの合計点は【%2d】または【%2d】です。\n", hand->sum, hand->sum+10);
	}else{
		printf("プレイヤーの合計点は【%2d】です。\n", hand->sum);
	}
}

//初回ドロー
void InitialDraw(Card *hand){
	for(int i=0; i<2; i++){
		HitCard(hand);
	}
}

//バーストチェック
void BurstCheck(Card *hand){
	CalHandCard(hand);			//手持ちカードの計算
	if(hand->sum > 21){
		hand->burst=1;
	}
}

//プレイヤーの行動選択
void PlayerTurn(Card *hand){
	do{
		do{
			printf("\nHIT or STAND or DOUBLEの選択\n【Hit>>>H／Stand>>>S／Double>>>D】："); fflush(0); scanf("%s", HITSTAND);
		}while((HITSTAND[0]=='D')&&((hand->betcoin-bet[0]*2)<0));
		if(HITSTAND[0]=='H'||HITSTAND[0]=='D'){
			HitCard(hand);
		}
	}while(!(HITSTAND[0]=='S'||HITSTAND[0]=='H'||HITSTAND[0]=='D'));
}

//ディーラーの手持ちオープン
void DealerOpen(Card *hand){
	printf("***ディーラーの手持ち***\n   [%c %c]\n", mark[((hand->cardnumber[0]-1)/13)],
			number[(hand->cardnumber[0]-1)%13]);
}

void DealerOpenReal(Card *hand){
	printf("***ディーラーの手持ち***\n");
	if(((hand->cardnumber[0]-1)/13)==0){
		printf(" ----------- "); printf(" ----------- \n");
		printf("|%c          |", number[(hand->cardnumber[0]-1)%13]); printf("|***********|\n");
		printf("|    sSs    |"); printf("|*         *|\n");
		printf("|   sSSSs   |"); printf("|*         *|\n");
		printf("|  sSSSSSs  |"); printf("|*    *    *|\n");
		printf("| sSSSSSSSs |"); printf("|*   ***   *|\n");
		printf("| S  SSS  S |"); printf("|*    *    *|\n");
		printf("|     S     |"); printf("|*         *|\n");
		printf("|   cSSSc   |"); printf("|*         *|\n");
		printf("|          %c|", number[(hand->cardnumber[0]-1)%13]); printf("|***********|\n");
		printf(" ----------- "); printf(" ----------- \n");
	}else if(((hand->cardnumber[0]-1)/13)==1){
		printf(" ----------- "); printf(" ----------- \n");
		printf("|%c          |", number[(hand->cardnumber[0]-1)%13]); printf("|***********|\n");
		printf("|  dQ   Qb  |"); printf("|*         *|\n");
		printf("| dHHQVQHHb |"); printf("|*         *|\n");
		printf("| QHHHHHHHP |"); printf("|*    *    *|\n");
		printf("|  QHHHHHP  |"); printf("|*   ***   *|\n");
		printf("|   QHHHP   |"); printf("|*    *    *|\n");
		printf("|    QHP    |"); printf("|*         *|\n");
		printf("|     V     |"); printf("|*         *|\n");
		printf("|          %c|", number[(hand->cardnumber[0]-1)%13]); printf("|***********|\n");
		printf(" ----------- "); printf(" ----------- \n");
	}else if(((hand->cardnumber[0]-1)/13)==2){
		printf(" ----------- "); printf(" ----------- \n");
		printf("|%c          |", number[(hand->cardnumber[0]-1)%13]); printf("|***********|\n");
		printf("|    cCc    |"); printf("|*         *|\n");
		printf("|    QCP    |"); printf("|*         *|\n");
		printf("| cCc S cCc |"); printf("|*    *    *|\n");
		printf("| CCCcCcCCC |"); printf("|*   ***   *|\n");
		printf("| CCC S CCC |"); printf("|*    *    *|\n");
		printf("|    cCc    |"); printf("|*         *|\n");
		printf("|   CCCCC   |"); printf("|*         *|\n");
		printf("|          %c|", number[(hand->cardnumber[0]-1)%13]); printf("|***********|\n");
		printf(" ----------- "); printf(" ----------- \n");
	}else if(((hand->cardnumber[0]-1)/13)==3){
		printf(" ----------- "); printf(" ----------- \n");
		printf("|%c          |", number[(hand->cardnumber[0]-1)%13]); printf("|***********|\n");
		printf("|    dDb    |"); printf("|*         *|\n");
		printf("|   dDDDb   |"); printf("|*         *|\n");
		printf("|  dDDDDDb  |"); printf("|*    *    *|\n");
		printf("| dDDDDDDDP |"); printf("|*   ***   *|\n");
		printf("|  QDDDDDP  |"); printf("|*    *    *|\n");
		printf("|   QDDDP   |"); printf("|*         *|\n");
		printf("|    TDP    |"); printf("|*         *|\n");
		printf("|          %c|", number[(hand->cardnumber[0]-1)%13]); printf("|***********|\n");
		printf(" ----------- "); printf(" ----------- \n");
	}
	putchar('\n');
}


//ディーラーの行動選択 ＊手札合計が17以上21であればステイを選択する
void DealerTurn(Card *hand){
	do{
		CalHandCard(hand);		//手持ち計算
		if(hand->Aflag > 0){
			if(hand->Aflag <= 2 || hand->Aflag==4){
				int suma, sumb;
				suma = hand->sum;
				sumb = hand->sum + 10;	//1枚だけ11で扱う
				if(sumb >= 17 && sumb <= 21){
					break;
				}else if(sumb > 21 && suma > 21){
					break;
				}else{
					HitCard(hand);		//ヒット
				}
			}else if(hand->Aflag==3){
				break;							//AAAで21点
			}
		}else if(hand->sum < 17){
			HitCard(hand);		//ヒット
		}
	}while(hand->sum < 17);
}

void ChangeCard(int sum, Card *d_hand){
	int findcard=21-sum;
	if(findcard==0){
		return;
	}
	for(int i=cnt+1; i<CARDNUMMAX-cnt; i++){
		if(((maindeck[i]-1)%13+1)==findcard){
			int tmp=maindeck[i];
			maindeck[i]=maindeck[cnt];
			maindeck[cnt]=tmp;
		}
	}

}

//ディーラーの行動選択 ＊21になるように山札を入れ替える
void DirtyDealerTurn(Card *d_hand){
	do{
		CalHandCard(d_hand);		//手持ち計算
		if(d_hand->sum < 11){
			HitCard(d_hand);			//ヒット
		}
		CalHandCard(d_hand);		//手持ち計算
	}while(d_hand->sum < 11);

	do{
		if(d_hand->Aflag > 0){
			if(d_hand->Aflag <= 2 || d_hand->Aflag==4){
				int suma, sumb;
				suma = d_hand->sum;
				sumb = d_hand->sum + 10;	//1枚だけ11で扱う
				if(sumb>21){
					ChangeCard(suma, d_hand);
					HitCard(d_hand);		//ヒット
					CalHandCard(d_hand);		//手持ち計算
				}else if(sumb<21){
					ChangeCard(sumb, d_hand);
					HitCard(d_hand);		//ヒット
					CalHandCard(d_hand);		//手持ち計算
				}else if(sumb==21){
					;
				}
			}else if(d_hand->Aflag==3){
				;							//AAAで21点
			}
		}else if(d_hand->sum < 21){
			ChangeCard(d_hand->sum, d_hand);
			HitCard(d_hand);		//ヒット
			CalHandCard(d_hand);		//手持ち計算
		}
	}while(d_hand->sum < 17);
}

//勝敗判定
void BattleCheck(Card *u_hand, Card *d_hand){
	if(u_hand->Aflag > 0){
		for(int i=1; i <= u_hand->Aflag; i++){
			if((u_hand->sum+10*i) <= 21){
				u_hand->sum += 10;	//21を越えなければAは10として扱う
			}
		}
	}

	if(d_hand->Aflag > 0){
		for(int i=1; i <= d_hand->Aflag; i++){
			if((d_hand->sum+10*i) <= 21){
				d_hand->sum += 10;
			}
		}
	}

	if(u_hand->sum > 21){
		puts("\n\n  *   *   ***   *   *    *      ***   ***  *****");
		puts("   * *   *   *  *   *    *     *   * *     *    ");
		puts("    *    *   *  *   *    *     *   *  ***  *****");
		puts("    *    *   *  *   *    *     *   *     * *    ");
		puts("    *     ***    ***     *****  ***   ***  *****");
		if(HITSTAND[0]=='D'){
			puts("ダブルダウン失敗");
			u_hand->betcoin-=(bet[0]*2);
		}else{
			u_hand->betcoin-=bet[0];
		}
	}else if(u_hand->sum > d_hand->sum){
		puts("\n\n  *   *   ***   *   *    *   *  ***  *   *");
		puts("   * *   *   *  *   *    * * *   *   **  *");
		puts("    *    *   *  *   *    * * *   *   * * *");
		puts("    *    *   *  *   *     * *    *   *  **");
		puts("    *     ***    ***      * *   ***  *   *");
		if(HITSTAND[0]=='D'){
			puts("ダブルダウン成功");
			u_hand->betcoin+=(bet[0]*2);
		}else{
			if(u_hand->cardposition==2&&u_hand->sum==21){
				puts("\n*****Natural BJ!!!!*****");
				u_hand->betcoin+=(bet[0]*15)/10;
			}else{
				u_hand->betcoin+=bet[0];
			}
		}
	}else if(u_hand->sum == d_hand->sum){
		puts("\n\n ***  ****    *   *   *");
		puts(" *  * *   *  * *  * * *");
		puts(" *  * * **  *   * * * *");
		puts(" *  * *  *  *****  * * ");
		puts(" ***  *   * *   *  * * ");
	}else if(u_hand->sum <= 21 && d_hand->sum>21){
		puts("\n\n  *   *   ***   *   *    *   *  ***  *   *");
		puts("   * *   *   *  *   *    * * *   *   **  *");
		puts("    *    *   *  *   *    * * *   *   * * *");
		puts("    *    *   *  *   *     * *    *   *  **");
		puts("    *     ***    ***      * *   ***  *   *");
		if(HITSTAND[0]=='D'){
			puts("ダブルダウン成功");
			u_hand->betcoin+=(bet[0]*2);
		}else{
			if(u_hand->cardposition==2&&u_hand->sum==21){
				puts("\n*****Natural BJ!!!!*****");
				u_hand->betcoin+=(bet[0]*15)/10;
			}else{
				u_hand->betcoin+=bet[0];
			}
		}
	}else if(u_hand->sum < d_hand->sum){
		puts("\n\n  *   *   ***   *   *    *      ***   ***  *****");
		puts("   * *   *   *  *   *    *     *   * *     *    ");
		puts("    *    *   *  *   *    *     *   *  ***  *****");
		puts("    *    *   *  *   *    *     *   *     * *    ");
		puts("    *     ***    ***     *****  ***   ***  *****");
		if(HITSTAND[0]=='D'){
			puts("ダブルダウン失敗");
			u_hand->betcoin-=(bet[0]*2);
		}else{
			u_hand->betcoin-=bet[0];
		}
	}else{
		puts("\n\n*********該当なし*********\n担当者に連絡してください。\n");
	}
}

void PrintMarkCenter(Card *hand){
	for(int i=0; i<hand->cardposition; i++){
		if(((hand->cardnumber[i]-1)/13)==0){
			printf("|    sSs    |");
		}else if(((hand->cardnumber[i]-1)/13)==1){
			printf("|  dQ   Qb  |");
		}else if(((hand->cardnumber[i]-1)/13)==2){
			printf("|    cCc    |");
		}else if(((hand->cardnumber[i]-1)/13)==3){
			printf("|    dDb    |");
		}
	}
	putchar('\n');
	for(int i=0; i<hand->cardposition; i++){
		if(((hand->cardnumber[i]-1)/13)==0){
			printf("|   sSSSs   |");
		}else if(((hand->cardnumber[i]-1)/13)==1){
			printf("| dHHQVQHHb |");
		}else if(((hand->cardnumber[i]-1)/13)==2){
			printf("|    QCP    |");
		}else if(((hand->cardnumber[i]-1)/13)==3){
			printf("|   dDDDb   |");
		}
	}
	putchar('\n');
	for(int i=0; i<hand->cardposition; i++){
		if(((hand->cardnumber[i]-1)/13)==0){
			printf("|  sSSSSSs  |");
		}else if(((hand->cardnumber[i]-1)/13)==1){
			printf("| QHHHHHHHP |");
		}else if(((hand->cardnumber[i]-1)/13)==2){
			printf("| cCc S cCc |");
		}else if(((hand->cardnumber[i]-1)/13)==3){
			printf("|  dDDDDDb  |");
		}
	}
	putchar('\n');
	for(int i=0; i<hand->cardposition; i++){
		if(((hand->cardnumber[i]-1)/13)==0){
			printf("| sSSSSSSSs |");
		}else if(((hand->cardnumber[i]-1)/13)==1){
			printf("|  QHHHHHP  |");
		}else if(((hand->cardnumber[i]-1)/13)==2){
			printf("| CCCcCcCCC |");
		}else if(((hand->cardnumber[i]-1)/13)==3){
			printf("| dDDDDDDDP |");
		}
	}
	putchar('\n');
	for(int i=0; i<hand->cardposition; i++){
		if(((hand->cardnumber[i]-1)/13)==0){
			printf("| S  SSS  S |");
		}else if(((hand->cardnumber[i]-1)/13)==1){
			printf("|   QHHHP   |");
		}else if(((hand->cardnumber[i]-1)/13)==2){
			printf("| CCC S CCC |");
		}else if(((hand->cardnumber[i]-1)/13)==3){
			printf("|  QDDDDDP  |");
		}
	}
	putchar('\n');
	for(int i=0; i<hand->cardposition; i++){
		if(((hand->cardnumber[i]-1)/13)==0){
			printf("|     S     |");
		}else if(((hand->cardnumber[i]-1)/13)==1){
			printf("|    QHP    |");
		}else if(((hand->cardnumber[i]-1)/13)==2){
			printf("|    cCc    |");
		}else if(((hand->cardnumber[i]-1)/13)==3){
			printf("|   QDDDP   |");
		}
	}
	putchar('\n');
	for(int i=0; i<hand->cardposition; i++){
		if(((hand->cardnumber[i]-1)/13)==0){
			printf("|   cSSSc   |");
		}else if(((hand->cardnumber[i]-1)/13)==1){
			printf("|     V     |");
		}else if(((hand->cardnumber[i]-1)/13)==2){
			printf("|   CCCCC   |");
		}else if(((hand->cardnumber[i]-1)/13)==3){
			printf("|    TDP    |");
		}
	}
	putchar('\n');
}

void PrintMark(Card *hand){
	for(int i=0; i<hand->cardposition; i++){
			printf(" ----------- ");
	}
	putchar('\n');
	for(int i=0; i<hand->cardposition; i++){
		printf("|%c          |", number[(hand->cardnumber[i]-1)%13]);
	}
	putchar('\n');

	PrintMarkCenter(hand);

	for(int i=0; i<hand->cardposition; i++){
		printf("|          %c|", number[(hand->cardnumber[i]-1)%13]);
	}
	putchar('\n');
		for(int i=0; i<hand->cardposition; i++){
			printf(" ----------- ");
	}
	putchar('\n');
}

void PlayerAction(Card *u_hand, Card *d_hand){

	do{
		system("cls");
		// DealerOpen(&dealer_hand);				//ディーラーの持ち手オープン
		DealerOpenReal(d_hand);				//ディーラーの持ち手オープン

		puts("***プレイヤーの手持ち***");
		// PrintCard(u_hand);				//手持ちのカード表示
		PrintMark(u_hand);
		CalHandCard(u_hand);			//手持ちカードの計算
		PrintHandCard(u_hand);			//手持ちカードの合計値の表示

		PlayerTurn(u_hand);	//プレイヤーの行動選択

		BurstCheck(u_hand);			//バーストチェック
	}while((HITSTAND[0]=='H')&&(u_hand->burst==0));
}

void PrintCoin(Card *hand){
	printf("現在の所持金は%dドルです。\n", hand->betcoin);
}

int main(void){
	system("cls");
	srand((unsigned) time(NULL));			//乱数の初期化（初回のみでOK）

	Card user_hand;							//ユーザーの手持ちカード
	Card dealer_hand;						//ディーラーの手持ちカード

	InitialStatus(&user_hand);				//ユーザーのステータス初期設定
	InitialStatus(&dealer_hand);			//ディーラーのステータス初期設定

	char TRY[]="Y";
	do{
		ContinueInitialStatus(&user_hand);		//ユーザーのステータス初期設定
		ContinueInitialStatus(&dealer_hand);	//ユーザーのステータス初期設定

		PrintCoin(&user_hand);				//所持金の表示
		do{
			printf("いくら賭けますか？\n（最大%dドルまで）:", user_hand.betcoin);
			fflush(0); scanf("%d", &bet[0]);		//プレイヤーの掛け金の格納
		}while(bet[0]>user_hand.betcoin);

		CardInitial();							//山札の初期化
		CardShuffle();							//山札のシャッフル

	//	PrintMainCard();						//山札の内容表示
	//	printf("\n\n");

		InitialDraw(&user_hand);				//ユーザーの初回の2枚引き
		InitialDraw(&dealer_hand);				//ディーラーの初回の2枚引き

		PlayerAction(&user_hand, &dealer_hand);	//プレイヤーの行動選択

		DirtyDealerTurn(&dealer_hand);			//ディーラーの行動選択

		system("cls");
		printf("\nResult (Press Enter KEY)"); fflush(0);	rewind(stdin);	(void)getchar();
		for(int i=0; i<3; i++){
			sleep(1);
			putchar('.'); fflush(0);
		}

		BattleCheck(&user_hand, &dealer_hand);		//勝敗確認

		printf("\nディーラーの手持ち合計:%d\n", dealer_hand.sum);
		// PrintCard(&dealer_hand);
		PrintMark(&dealer_hand);

		printf("プレイヤーの手持ち合計:%d\n", user_hand.sum);
		// PrintCard(&user_hand);
		PrintMark(&user_hand);

		PrintCoin(&user_hand);				//所持金の表示
		do{
			printf("継続しますか？【yes→Y／no→N】:");
			fflush(0); scanf("%s", TRY);		//継続の選択
		}while(!(TRY[0]=='Y'||TRY[0]=='N'));

	}while((TRY[0]=='Y')&&(user_hand.betcoin>0));

	if(user_hand.betcoin>INITALMONEY){
		printf("所持金を%dドル増やしました。", user_hand.betcoin-INITALMONEY);
	}else if(user_hand.betcoin==INITALMONEY){
		puts("所持金の増減はありませんでした。");
	}else if(user_hand.betcoin<INITALMONEY){
		printf("所持金を%dドル失いました。\n賭け事はほどほどに！\n", INITALMONEY-user_hand.betcoin);
	}

	printf("\nPress Enter KEY"); fflush(0);	rewind(stdin);	(void)getchar();

	return 0;
}


