#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define CARDNUMMAX 52		//トランプの枚数
#define CARDMARKMAX 4		//カードのマーク(ハート,スペード,ダイヤ,クローバー)
#define HANDMAX 12		//どのような引きでも11枚を超えるとバーストするので(1,1,1,1,2,2,2,2,3,3,3⇒21)
#define SHUFFLENUM 100		//シャッフル時の入れ替え回数(任意)
#define SHUFFLTIME 5		//シャッフル待機時間s(演出のため)
#define sleep(n) Sleep(n * 1000)	//sec msec置き換え

//手持ちカードの構造体
typedef struct{
	int cardnumber[HANDMAX];	//手持ちカードの数値
	int cardposition;		//手持ちカード枚数
	int sum;			//手持ちの合計点数
	int burst;			//バーストフラグ
	int Aflag;			//Aの枚数フラグ
}Card;

int maindeck[CARDNUMMAX];			//山札
char mark[]="SHCD";				//S:スペード, H:ハート, C:クローバー, D:ダイヤ(表示用)
char number[]="A23456789XJQK";			//1-13番(表示用)
int cnt=0;					//山札から引いた回数

//山札の初期化
void CardInitial(int randnum){
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
	hand->cardposition=0;			//ヒット回数の初期化（最初の2枚を含む）
	hand->sum=0;				//カードの合計値を初期化
	hand->burst=0;				//バーストフラグ(0:OFF / 1:ON)
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
	hand->sum=0;							//合計値の初期化
	hand->Aflag=0;							//Aフラグの初期化
	for(int i=0; i < hand->cardposition; i++){
		if(((hand->cardnumber[i]-1)%13+1)>10){			//JQKの場合
			hand->sum+=10;
		}else if(((hand->cardnumber[i]-1)%13+1)==1){		//Aの場合
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
void PlayerTurn(Card *hand, char HITSTAND[]){
	do{
		printf("\nHIT or STANDの選択\n【Hit>>>H／Stand>>>S】："); fflush(0); scanf("%s", HITSTAND);
		if(HITSTAND[0]=='H'){
			HitCard(hand);
		}
	}while(!(HITSTAND[0]=='S'||HITSTAND[0]=='H'));
}


//ディーラーの手持ちオープン
void DealerOpen(Card *hand){
	printf("***ディーラーの手持ち***\n   [%c %c]\n", mark[((hand->cardnumber[0]-1)/13)],
			number[(hand->cardnumber[0]-1)%13]);
}

//ディーラーの行動選択 ＊手札合計が17以上21以下であればステイを選択する
void DealerTurn(Card *hand){
	do{
		CalHandCard(hand);					//手持ち計算
		if(hand->Aflag > 0){
			if(hand->Aflag <= 2 || hand->Aflag==4){
				int suma, sumb;
				suma = hand->sum;
				sumb = hand->sum + 10;			//1枚だけ11で扱う
				if(sumb >= 17 && sumb <= 21){
					break;
				}else if(sumb > 21 && suma > 21){
					break;
				}else{
					HitCard(hand);			//ヒット
				}
			}else if(hand->Aflag==3){
				break;					//AAAで21点
			}
		}else if(hand->sum < 17){
			HitCard(hand);					//ヒット
		}
	}while(hand->sum < 17);
}

//勝敗判定
void BattleCheck(Card *u_hand, Card *d_hand){
	if(u_hand->Aflag > 0){
		for(int i=1; i <= u_hand->Aflag; i++){
			if((u_hand->sum+10*i) <= 21){
				u_hand->sum += 10;			//21を越えなければAは10として扱う
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
		puts("\n*******You LOSE*******\n");
	}else if(u_hand->sum > d_hand->sum){
		puts("\n*******You WIN********\n");
	}else if(u_hand->sum == d_hand->sum){
		puts("\n*********DRAW*********\n");
	}else if(u_hand->sum <= 21 && d_hand->sum>21){
		puts("\n*******You WIN********\n");
	}else if(u_hand->sum < d_hand->sum){
		puts("\n*******You LOSE*******\n");
	}else{
		puts("\n*********該当なし*********\n");
	}
}

int main(void){
	system("cls");
	srand((unsigned) time(NULL));						//乱数の初期化（初回のみでOK）
	int randnum=rand()%CARDNUMMAX;						//スタートポジション

	Card user_hand;								//ユーザーの手持ちカード
	Card dealer_hand;							//ディーラーの手持ちカード

	InitialStatus(&user_hand);						//ユーザーのステータス初期設定
	InitialStatus(&dealer_hand);						//ディーラーのステータス初期設定

	CardInitial(randnum);							//山札の初期化
	CardShuffle();								//山札のシャッフル

//	PrintMainCard();							//山札の内容表示
//	printf("\n\n");

	InitialDraw(&user_hand);						//ユーザーの初回の2枚引き
	InitialDraw(&dealer_hand);						//ディーラーの初回の2枚引き

	char HITSTAND[]="A";							//ユーザーのヒットスタンドの選択初期設定
	do{
		system("cls");
		DealerOpen(&dealer_hand);					//ディーラーの持ち手オープン

		puts("***プレイヤーの手持ち***");
		PrintCard(&user_hand);						//手持ちのカード表示
		CalHandCard(&user_hand);					//手持ちカードの計算
		PrintHandCard(&user_hand);					//手持ちカードの合計値の表示
		PlayerTurn(&user_hand, HITSTAND);				//プレイヤーの行動選択
		BurstCheck(&user_hand);						//バーストチェック

	}while((HITSTAND[0]=='H')&&(user_hand.burst==0));

	DealerTurn(&dealer_hand);						//ディーラーの行動選択

	system("cls");
	printf("\nResult"); fflush(0);	rewind(stdin);	(void)getchar();
	for(int i=0; i<3; i++){
		sleep(1);
		putchar('.'); fflush(0);
	}

	BattleCheck(&user_hand, &dealer_hand);					//勝敗確認

	printf("プレイヤーの手持ち合計:%d\n", user_hand.sum);
	PrintCard(&user_hand);

	//エンターキーの入力待ち

	printf("\nディーラーの手持ち合計:%d\n", dealer_hand.sum);
	PrintCard(&dealer_hand);

	printf("\nPress Enter KEY"); fflush(0);	rewind(stdin);	(void)getchar();

	return 0;
}

