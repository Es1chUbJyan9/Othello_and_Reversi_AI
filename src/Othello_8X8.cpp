#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <assert.h>
#include <limits.h>

#define Board_Size 8
#define TRUE 1
#define FALSE 0
#define Hashsize 67108864 //(2GB)67108864 (4GB)134217728 (8GB)268435456

//等待ms
void Delay(unsigned int mseconds);
//讀取最新一步
void Read_File( char *c );
//讀檔並開始遊戲
char Load_File( void );
//初始化棋盤
void Init();
//判斷下子合法性
int  Play_a_Move( int x, int y);
//顯示棋盤狀況
void Show_Board_and_Set_Legal_Moves( void );
//下子
int  Put_a_Stone( int x, int y );
//判斷是否在棋盤內
int  In_Board( int x, int y );
//判斷是否可吃棋
int  Check_Cross( int x, int y, int update );
//判斷d方向是否可吃棋，update為執行吃棋訊號
int  Check_Straight_Army( int x, int y, int d, int update );
//更新合法步，返回合法步數
int  Find_Legal_Moves(int color);
//判斷是否終局
int  Check_EndGame( void );
//更新比數
int  Compute_Grades(int flag ,int isL);
//決定下棋點
void Computer_Think( int *x, int *y);
//MinMax
int  Search(int myturn, int mylevel);
//遞迴深度搜索
int  search_next(int x, int y, int myturn, int mylevel, int alpha, int beta);
//排序函數
int comparH(const void* a,const void* b);
//U64 隨機數產生器
long long int rand64(void);
//計算zobristHash
long long int getHash(void);
//計算穩定子
int StableDiscs(int me);

int search_deep = 4;
int Search_Counter;
int Computer_Take;
int Winner;
int Now_Board[ Board_Size ][ Board_Size ];
int Legal_Moves[ Board_Size ][ Board_Size ];
int Legal_Move_Index[50][3];
int HandNumber;
int sequence[Board_Size*Board_Size];
int Black_Count, White_Count;
int Turn = 0;         // 0 is black or 1 is white
int Stones[2]= {1,2}; // 1: black, 2: white
int DirX[8] = {0, 1, 1, 1, 0, -1, -1, -1};
int DirY[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
int LastX, LastY;
int resultX, resultY;
typedef struct location{
    int i;
    int j;
    int g;
} Location;
int B, W;

int Dynamicdeep = 10;                               //動態深度
int DynamicdeepS = 0;                               //加深開始手數
int DynamicdeepE = 0;                               //加深結束手數
int deepStart = 4;                                  //初始深度
int deepEnd = 8;                                    //最終深度
int endTime = 46;                                   //終局搜索 Pefect End
int fS = 4;                                         //穩定子權重 StableDiscs Weight
int fRandomMove = true;                             //初始隨機下子
int RandomMove = 2;                                 //初始隨機手數
int fiterative = true;                              //遞代加深 Iterative Deepening
int fMTD = true;                                    //記憶測試 MTD Memory-enhanced Test Driver
int alpha_beta_option = true;                       //AB剪枝 Alpha-Beta
double historyAttenua = 0.2;                        //歷史衰減 History attenuation coefficient
Location history[2][Board_Size*Board_Size][Board_Size*Board_Size]; //歷史表 Move Ordering
int zhash = true;                                   //置換表 transposition table
struct{
    long long int checksum;
    char depth;
    char entry_type;                                //0=exact, 1=lower_bound, 2=upper_bound
    int g;
} hashtable[2][Hashsize];
long long int zobrist[2][Board_Size][Board_Size];   //Zobrist Hash

//測試資料
long long int Hashnode = 0;
long long int Searchnode = 0;
long long int Grades = 0;

//---------------------------------------------------------------------------

int main(void){
    char compcolor = 'W', c[10];
    int column_input = -1, row_input = -1;
    int rx, ry, m=0, n;
    FILE *fp;
    
    Init();
    printf("Computer take?(B/W/All/File play as first/file play as Second/Load and play): ");
    scanf("%c", &compcolor);
    Show_Board_and_Set_Legal_Moves();
    
    if (compcolor == 'L' || compcolor == 'l')
        compcolor = Load_File();
    
    if ( compcolor == 'B' || compcolor == 'b'){
        Computer_Think( &rx, &ry );
        printf("Computer played %c%d\n", rx+97, ry+1);
        Play_a_Move( rx, ry );
        
        Show_Board_and_Set_Legal_Moves();
    }
    
    if ( compcolor == 'A' || compcolor == 'a')
        while ( m++ < Board_Size * Board_Size ){
            Computer_Think( &rx, &ry );
            if ( !Play_a_Move( rx, ry )){
                printf("Wrong Computer moves %c%d\n", rx+97, ry+1);
                scanf("%d", &n);
                break;
            }
            if ( rx == -1 )
                printf("Computer Pass\n");
            else
                printf("Computer played %c%d\n", rx+97, ry+1);
            
            if ( Check_EndGame() )
                return 0;
            Show_Board_and_Set_Legal_Moves();
        }
    
    if ( compcolor == 'F'){
        Computer_Think( &rx, &ry );
        Play_a_Move( rx, ry );
    }
    
    while ( m++ < Board_Size*Board_Size ){
        for(;;){
            if ( compcolor == 'F' || compcolor == 'S' ){
                fp = fopen( "of.txt", "r" );
                fscanf( fp, "%d", &n );
                fclose( fp );
                c[2]=' ';
                if (compcolor == 'F' ){
                    if ( n % 2 == 0 )
                        Read_File( c );
                    else{
                        Delay(101);
                        continue;
                    }
                }
                else{
                    if ( n % 2 == 1 )
                        Read_File( c );
                    else{
                        Delay(101);
                        continue;
                    }
                }
            }
            
            
            if ( compcolor == 'B' || compcolor == 'b'){
                printf("input White move:(a-h 1-8), or PASS\n");
                scanf("%s", c);
            }
            else if ( compcolor == 'W' || compcolor == 'w'){
                printf("input Black move:(a-h 1-8), or PASS\n");
                scanf("%s", c);
            }
            
            if ( c[0] == 'P' || c[0] == 'p')
                row_input = column_input = -1;
            else if ( c[0] == 'M' || c[0] == 'm' ){
                Computer_Think( &rx, &ry );
                if ( !Play_a_Move( rx, ry )){
                    printf("Wrong Computer moves %c%dŝ\n", rx+97, ry+1);
                    scanf("%d", &n);
                    break;
                }
                if ( rx == -1 )
                    printf("Computer Pass");
                else
                    printf("Computer played %c%d\n", rx+97, ry+1);
                if ( Check_EndGame() )
                    break;
                Show_Board_and_Set_Legal_Moves();
            }
            else{
                row_input= c[0] - 97;
                
                if (c[2] == '0'){
                    column_input =  9;
                }
                else{
                    column_input = c[1] - 49;
                }
            }
            
            if ( !Play_a_Move(row_input, column_input) ){
                printf("%c%d is a Wrong move!\n", c[0], column_input+1);
            }
            
            else
                break;
        }
        if ( Check_EndGame() )
            return 0;
        Show_Board_and_Set_Legal_Moves();
        
        Computer_Think( &rx, &ry );
        printf("Computer played %c%d\n", rx+97, ry+1);
        Play_a_Move( rx, ry );
        if ( Check_EndGame() )
            return 0;;
        Show_Board_and_Set_Legal_Moves();
        
    }
    
    printf("Game is over");
    scanf("%d", &n);
    
    return 0;
}

void Delay(unsigned int mseconds){
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

void Read_File( char *c ){
    FILE *fp;
    char tc[10];
    fp = fopen( "of.txt", "r" );
    
    while ( (fscanf( fp, "%s", tc ) ) != EOF ){
        c[2] = 'a';
        c[0] = tc[0];
        if (tc[2] == 48){
            c[1] = '1';
            c[2] = '0';
        }
        else{
            c[1] = tc[1];
        }
    }
}

char Load_File( void ){
    FILE *fp;
    char tc[10];
    int row_input, column_input, n;
    
    fp = fopen( "of.txt", "r" );
    assert( fp != NULL );
    
    fscanf( fp, "%d", &n );
    printf("n = %d\n",n);
    while ( (fscanf( fp, "%s", tc ) ) != EOF ){
        
        column_input = tc[0] - 97;
        row_input = tc[1] - 49;
        if (tc[2] == 48){
            column_input = 9;
        }
        if ( !Play_a_Move(row_input, column_input) )
            printf("%c%d is a Wrong move\n", tc[0], column_input+1);
        
        Show_Board_and_Set_Legal_Moves();
        tc[2] =' ';
    }
    return ( n%2 == 1 )? 'B' : 'W';
}

void Init(){
    srand( (unsigned)time(NULL) );
    Computer_Take = 0;
    memset(Now_Board, 0, sizeof(int) * Board_Size * Board_Size);
    
    /* 10X10
    Now_Board[4][4] = Now_Board[5][5] = 2;
    Now_Board[4][5] = Now_Board[5][4] = 1;
    */
    
    Now_Board[3][3] = Now_Board[4][4] = 2;
    Now_Board[3][4] = Now_Board[4][3] = 1;
    
    HandNumber = 0;
    memset(sequence, -1, sizeof(int) * Board_Size * Board_Size);
    Turn = 0;
    
    LastX = LastY = -1;
    Black_Count = White_Count = 0;
    Search_Counter = 0;
    Winner = 0;
    
    //初始化歷史表
    Location tempL;
    for( int i = 0,m = 0; i < Board_Size; i++ ){
        for( int j = 0; j < Board_Size; j++,m++){
            tempL.i = i;
            tempL.j = j;
            tempL.g = -1;
            for(int k = 0; k<Board_Size*Board_Size; k++){
                history[0][k][m] = tempL;
                history[1][k][m] = tempL;
            }
        }
    }
    //初始化置換表
    for( int i = 0; i < Board_Size; i++ ){
        for( int j = 0; j < Board_Size; j++){
            zobrist[0][i][j] = rand64();
            zobrist[1][i][j] = rand64();
        }
    }
}

int Play_a_Move( int x, int y){
    FILE *fp;
    
    if ( x == -1 && y == -1){
        fp = fopen( "of.txt", "r+" );
        fprintf( fp, "%2d\n", HandNumber+1 );
        fclose(fp);
        
        fp = fopen("of.txt", "a");
        fprintf( fp, "p9\n" );
        fclose( fp );
        
        sequence[HandNumber] = -1;
        HandNumber ++;
        Turn = 1 - Turn;
        return 1;
    }
    
    if ( !In_Board(x,y))
        return 0;
    Find_Legal_Moves( Stones[Turn] );
    if( Legal_Moves[x][y] == FALSE )
        return 0;
    
    if( Put_a_Stone(x,y) ){
        Check_Cross(x,y,1);
        
        Compute_Grades( TRUE ,0);
        return 1;
    }
    else
        return 0;
    return 1;
}

int Put_a_Stone(int x, int y){
    FILE *fp;
    
    if( Now_Board[x][y] == 0){
        sequence[HandNumber] = Turn;
        if (HandNumber == 0)
            fp = fopen( "of.txt", "w" );
        else
            fp = fopen( "of.txt", "r+" );
        fprintf( fp, "%2d\n", HandNumber+1 );
        HandNumber ++;
        fclose(fp);
        
        Now_Board[x][y] = Stones[Turn];
        fp = fopen("of.txt", "a");
        fprintf( fp, "%c%d\n", x+97, y+1 );;
        fclose( fp );
        
        LastX = x;
        LastY = y;
        
        Turn = 1 - Turn;
        
        return TRUE;
    }
    return FALSE;
}

void Show_Board_and_Set_Legal_Moves( void ){
    int i,j;
    
    Find_Legal_Moves( Stones[Turn] );
    
    printf("a b c d e f g h\n");
    for(i=0 ; i<Board_Size ; i++){
        for(j=0 ; j<Board_Size ; j++){
            if( Now_Board[j][i] > 0 ){
                if ( Now_Board[j][i] == 2 )
                    printf("O ");//white
                else
                    printf("X ");//black
            }
            
            if (Now_Board[j][i] == 0){
                if ( Legal_Moves[j][i] == 1)
                    printf("? ");
                else printf(". ");
            }
        }
        printf(" %d\n", i+1);
    }
    printf("\n");
}

int Check_Cross(int x, int y, int update){
    int k, dx, dy;
    
    if( ! In_Board(x,y) || Now_Board[x][y] == 0)
        return FALSE;
    
    int army = 3 - Now_Board[x][y];
    int army_count = 0;
    
    for( k=0 ; k<8 ; k++ ){
        dx = x + DirX[k];
        dy = y + DirY[k];
        if( In_Board(dx,dy) && Now_Board[dx][dy] == army ){
            army_count += Check_Straight_Army( x, y, k, update ) ;
        }
    }
    
    if(army_count >0)
        return TRUE;
    else
        return FALSE;
}

int Check_Straight_Army(int x, int y, int d, int update){
    int me = Now_Board[x][y];
    int army = 3 - me;
    int army_count=0;
    int found_flag=FALSE;
    int flag[ Board_Size ][ Board_Size ] = {{0}};
    
    int i, j;
    int tx, ty;
    
    tx = x;
    ty = y;
    
    for(i=0 ; i<Board_Size ; i++){
        tx += DirX[d];
        ty += DirY[d];
        
        if(In_Board(tx,ty) ){
            if( Now_Board[tx][ty] == army ){
                army_count ++;
                flag[tx][ty] = TRUE;
            }
            else if( Now_Board[tx][ty] == me){
                found_flag = TRUE;
                break;
            }
            else
                break;
        }
        else
            break;
    }
    
    if( (found_flag == TRUE) && (army_count > 0) && update){
        for(i=0 ; i<Board_Size ; i++)
            for(j=0; j<Board_Size ; j++)
                if(flag[i][j]==TRUE){
                    if(Now_Board[i][j] != 0)
                        Now_Board[i][j]= 3 - Now_Board[i][j];
                }
    }
    if( (found_flag == TRUE) && (army_count > 0))
        return army_count;
    else return 0;
}

int In_Board(int x, int y){
    if( x >= 0 && x < Board_Size && y >= 0 && y < Board_Size )
        return TRUE;
    else
        return FALSE;
}

int Check_EndGame( void ){
    int lc1, lc2;
    
    lc2 = Find_Legal_Moves( Stones[1 - Turn] );
    lc1 = Find_Legal_Moves( Stones[Turn] );
    
    if ( lc1==0 && lc2==0 ){
        if(Black_Count > White_Count){
            printf("Black Win!\n");
            if(Winner == 0)
                Winner = 1;
        }
        else if(Black_Count < White_Count){
            printf("White Win!\n");
            if(Winner == 0)
                Winner = 2;
        }
        else{
            printf("Draw\n");
            Winner = 0;
        }
        Show_Board_and_Set_Legal_Moves();
        printf("Game is over\n");
        printf("Hashnode   = %12.lld \n",Hashnode);
        printf("Searchnode = %12.lld \n",Searchnode);
        printf("Grades     = %12.lld \n",Grades);
        scanf("%d", &lc1);
        return TRUE;
    }
    
    return FALSE;
}

int comparH(const void* a,const void* b){
    Location* aa = (Location*)a;
    Location* bb = (Location*)b;
    return bb->g - aa->g;
}

void Computer_Think( int *x, int *y ){
    if (fRandomMove && HandNumber < RandomMove ) { //初始隨機下子
        int lm = Find_Legal_Moves( Stones[Turn]);
        int RMV = ( rand() % lm ) +1 ;
        *x = Legal_Move_Index[RMV][1];
        *y = Legal_Move_Index[RMV][2];
        return;
    }
    else{
        time_t clockBegin, clockEnd;
        static int think_time=0;
        int flag = 0;
        
        //歷史表衰減
        if(historyAttenua != 0){
            for(int i=0; i<Board_Size*Board_Size; i++){
                for(int j=0; j<Board_Size*Board_Size; j++){
                    history[0][i][j].g *= historyAttenua;
                    history[1][i][j].g *= historyAttenua;
                }
            }
        }
        
        //動態深度
        if(DynamicdeepS <= HandNumber && HandNumber <= DynamicdeepE){
            deepEnd = Dynamicdeep+2;
        }
        //終局搜索
        else if(HandNumber >= endTime){
            deepEnd = (Board_Size*Board_Size) - HandNumber;
        }
        else{
            deepEnd = Dynamicdeep;
        }
        
        clockBegin = clock();
        
        resultX = resultY = -1;
        Search_Counter = 0;
        
        flag = Search( Turn, 0);
        
        clockEnd = clock();
        think_time += (clockEnd - clockBegin)/1000;
        printf("used thinking time= %d min. %d.%d sec.\n", think_time/60000, (think_time%60000)/1000,  (think_time%60000)%1000);
        
        if( flag ){
            *x = resultX;
            *y = resultY;
        }
        else{
            *x = *y = -1;
            return;
        }
    }
    
}

int Search(int myturn, int mylevel){
    if(Find_Legal_Moves(Stones[myturn]) <= 0)
        return FALSE;
    
    int B[ Board_Size ][ Board_Size ];
    int L[ Board_Size ][ Board_Size ];
    memcpy( B, Now_Board, sizeof(int) * Board_Size * Board_Size );
    memcpy( L, Legal_Moves, sizeof(int) * Board_Size * Board_Size );
    
    Location min, max;
    min.i = min.j = -1; min.g = INT_MAX;
    max.i = max.j = -1; max.g = INT_MIN;
    int i, j, k, test, g;
    int alpha = INT_MIN, beta = INT_MAX;
    
    for( k = 0; k < Board_Size*Board_Size; k++ ){
        i = history[myturn][mylevel][k].i;
        j = history[myturn][mylevel][k].j;
        if(L[i][j] == TRUE){
            memcpy(Now_Board, B, sizeof(int) * Board_Size * Board_Size );
            Now_Board[i][j] = Stones[myturn];
            Check_Cross(i,j,TRUE);
            
            //遞代加深
            for(search_deep = deepStart, g=0; search_deep <= deepEnd; search_deep+=2){
                if(fiterative == FALSE)
                    search_deep = deepEnd;
                //MTD
                if(fMTD){
                    beta = INT_MAX;
                    alpha = INT_MIN;
                    test = g;
                    do{
                        g = search_next(i,j, 1-myturn, mylevel+1, test-1, test);
                        if (g < test)
                            test = beta = g;
                        else{
                            alpha = g;
                            test = g + 1;
                        }
                    }while( alpha < beta );
                }
                else
                    g = search_next(i,j, 1-myturn, mylevel+1, alpha, beta);
            }
            
            if( myturn == 0 ){      // max level
                if( g > max.g ){
                    max.g = g;
                    max.i = i;
                    max.j = j;
                }
            }
            else{
                if( g < min.g ){    // min level
                    min.g = g;
                    min.i = i;
                    min.j = j;
                }
            }
            // cutoff
            if( alpha_beta_option )
                if( alpha >= beta){
                    i = Board_Size;
                    j = Board_Size;
                }
        }
    }
    
    memcpy( Now_Board, B, sizeof(int) * Board_Size * Board_Size );
    
    if( myturn == 0 ){
        resultX = max.i;
        resultY = max.j;
        return TRUE;
    }
    else{
        resultX = min.i;
        resultY = min.j;
        return TRUE;
    }
    return FALSE;
}

int search_next( int x, int y, int myturn, int mylevel, int alpha, int beta ){
    long long int hash64;
    long long int hash32;
    if( mylevel >= search_deep ){
        hash64 = getHash();
        hash32 = hash64 & (Hashsize-1);
        hashtable[myturn][hash32].checksum = hash64;
        hashtable[myturn][hash32].depth = 0;
        hashtable[myturn][hash32].entry_type = 0;
        hashtable[myturn][hash32].g = Compute_Grades( FALSE ,0);
        return hashtable[myturn][hash32].g;
    }
    
    if(Find_Legal_Moves( Stones[myturn] ) <= 0){
        return Compute_Grades( FALSE ,1);
    }
    
    Search_Counter++;
    
    int i,j,k,g=0;
    
    int B[ Board_Size ][ Board_Size ];
    int L[ Board_Size ][ Board_Size ];
    memcpy( B, Now_Board, sizeof(int) * Board_Size * Board_Size );
    memcpy( L, Legal_Moves, sizeof(int) * Board_Size * Board_Size );
    
    int fentry_type = 2;
    for( k = 0; k < Board_Size*Board_Size; k++ ){
        i = history[myturn][mylevel][k].i;
        j = history[myturn][mylevel][k].j;
        if(L[i][j] == TRUE){
            memcpy(Now_Board, B, sizeof(int) * Board_Size * Board_Size );
            Now_Board[i][j] = Stones[myturn];
            Check_Cross(i,j, TRUE) ;
            
            hash64 = getHash();
            hash32 = hash64 & (Hashsize-1);
            
            int flag = false;
            if(zhash && hashtable[1-myturn][hash32].checksum == hash64 &&
               hashtable[1-myturn][hash32].depth >= search_deep-(mylevel+1)){
                if(hashtable[1-myturn][hash32].entry_type == 0){
                    Hashnode++;
                    g = hashtable[1-myturn][hash32].g;
                    flag = true;
                }
                else if(hashtable[1-myturn][hash32].entry_type == 1){
                    if((1-myturn) == 0 && hashtable[1-myturn][hash32].g >= beta){
                        Hashnode++;
                        g = hashtable[1-myturn][hash32].g;
                        flag = true;
                    }
                    else if((1-myturn) == 1 && hashtable[1-myturn][hash32].g <= alpha){
                        Hashnode++;
                        g = hashtable[1-myturn][hash32].g;
                        flag = true;
                    }
                }
                else if(hashtable[1-myturn][hash32].entry_type == 2){
                    if ((1-myturn) == 0 && hashtable[1-myturn][hash32].g <= alpha){
                        Hashnode++;
                        g = hashtable[1-myturn][hash32].g;
                        flag = true;
                    }
                    else if((1-myturn) == 1 && hashtable[1-myturn][hash32].g >= beta){
                        Hashnode++;
                        g = hashtable[1-myturn][hash32].g;
                        flag = true;
                    }
                }
                
            }
            if(flag == false){
                Searchnode++;
                g = search_next(i,j, 1-myturn, mylevel+1, alpha, beta);
                
            }
            
            if(myturn==0){      // max ply
                if(g > alpha){
                    alpha = g;
                    history[myturn][mylevel][k].g += 2*mylevel*mylevel;  //普通的節點
                    fentry_type = 0;
                }
                history[myturn][mylevel][k].g -= 2*mylevel*mylevel;     //較差的節點
            }
            else{               // min ply
                if(g < beta){
                    beta = g;
                    history[myturn][mylevel][k].g += 2*mylevel*mylevel;  //普通的節點
                    fentry_type = 0;
                }
                history[myturn][mylevel][k].g -= 2*mylevel*mylevel;     //較差的節點
            }
            // cutoff
            if( alpha_beta_option )
                if( alpha >= beta ){
                    i = Board_Size;
                    j = Board_Size;
                    history[myturn][mylevel][k].g += 16*mylevel*mylevel; //優先的節點
                    fentry_type = 1;
                    break;
                }
        }
    }
    
    memcpy(Now_Board, B, sizeof(int) * Board_Size * Board_Size);
    
    //排序節點優先度
    qsort(history[myturn][mylevel], Board_Size * Board_Size, sizeof(history[myturn][mylevel][0]), comparH);
    
    hash64 = getHash();
    hash32 = hash64 & (Hashsize-1);
    
    if(myturn == 0){   //max level
        if(hashtable[myturn][hash32].depth <= search_deep-mylevel){
            hashtable[myturn][hash32].checksum = hash64;
            hashtable[myturn][hash32].depth = search_deep-mylevel;
            hashtable[myturn][hash32].entry_type = fentry_type;
            hashtable[myturn][hash32].g = alpha;
        }
        return alpha;
    }
    else{               //min level
        if(hashtable[myturn][hash32].depth <= search_deep-mylevel){
            hashtable[myturn][hash32].checksum = hash64;
            hashtable[myturn][hash32].depth = search_deep-mylevel;
            hashtable[myturn][hash32].entry_type = fentry_type;
            hashtable[myturn][hash32].g = beta;
        }
        return beta;
    }
}

long long int rand64(void) {
    return rand() ^ ((long long int)rand() << 15) ^ ((long long int)rand() << 30) ^ ((long long int)rand() << 45) ^ ((long long int)rand() << 60);
}

long long int getHash(void){
    long long int hash=0;
    for(int i=0; i < Board_Size; i++){
        for(int j=0; j < Board_Size; j++){
            if(Now_Board[i][j] == 1){
                hash ^= zobrist[0][i][j];
            }
            else if(Now_Board[i][j] == 2){
                hash ^= zobrist[1][i][j];
            }
        }
    }
    return hash;
}

int Find_Legal_Moves( int color ){
    int i,j;
    int me = color;
    int legal_count = 0;
    Legal_Move_Index[0][0] = 0;
    B = W = 0;
    
    for( i = 0; i < Board_Size; i++ )
        for( j = 0; j < Board_Size; j++ )
            Legal_Moves[i][j] = 0;
    
    for( i = 0; i < Board_Size; i++ )
        for( j = 0; j < Board_Size; j++ ){
            if( Now_Board[i][j] == 0 ){
                //剪枝加速
                if( i>0 && i<Board_Size-1 && j>0 && j<Board_Size-1 ){
                    if((Now_Board[i-1][j-1]  == 0 || Now_Board[i-1][j-1] == me) &&
                       (Now_Board[i-1][j]    == 0 || Now_Board[i-1][j]   == me) &&
                       (Now_Board[i-1][j+1]  == 0 || Now_Board[i-1][j+1] == me) &&
                       (Now_Board[i][j-1]    == 0 || Now_Board[i][j-1]   == me) &&
                       (Now_Board[i][j+1]    == 0 || Now_Board[i][j+1]   == me) &&
                       (Now_Board[i+1][j-1]  == 0 || Now_Board[i+1][j-1] == me) &&
                       (Now_Board[i+1][j]    == 0 || Now_Board[i+1][j]   == me) &&
                       (Now_Board[i+1][j+1]  == 0 || Now_Board[i+1][j+1] == me)){
                        continue;
                    }
                }
                Now_Board[i][j] = me;
                if( Check_Cross(i,j,FALSE) == TRUE ){
                    Legal_Moves[i][j] = TRUE;
                    legal_count++;
                    Legal_Move_Index[0][0] = legal_count;
                    Legal_Move_Index[legal_count][1] = i ;
                    Legal_Move_Index[legal_count][2] = j ;
                }
                Now_Board[i][j] = 0;
            }
            else if( Now_Board[i][j] == 1 ){
                B++;
            }
            else if( Now_Board[i][j] == 2 ){
                W++;
            }
        }
    
    return legal_count;
}

int Compute_Grades(int flag, int isL){
    if(flag){
        Find_Legal_Moves( 1 );
        Black_Count = B;
        White_Count = W;
        printf("Grade: Black %d, White %d\n", B, W);
        return 0;
    }
    Grades++;
    //中局
    if( HandNumber < endTime ){
        int SB, SW, BM;
        SB = StableDiscs(1);
        SW = StableDiscs(2);
        
        if( isL == 0 ){
            BM = Find_Legal_Moves( 1 );
        }
        else{
            BM = Legal_Move_Index[0][0];
        }
        return ( fS*(SB - SW) + BM);
    }
    //尾局
    else{
        if( isL == 0 ){
            B = 0;
            for(int i=0; i<Board_Size; i++)
                for(int j=0; j<Board_Size; j++){
                    if( Now_Board[i][j] == 1 ){
                        B++;
                    }
                }
        }
        return ( B );
    }
}

int StableDiscs(int me){
    int i, j, k, num = 0;
    int a=0,b=0,c=0,d=0;
    //左上角
    i = 0;
    j = 0;
    if(Now_Board[i][j] == me){
        num++;
        for(k=1; k<Board_Size; k++){
            if(Now_Board[i+k][j] != me)
                break;
            num++;
        }
        if(k == Board_Size)
            a++;
        for(k=1; k<Board_Size; k++){
            if(Now_Board[i][j+k] != me)
                break;
            num++;
        }
        if(k == Board_Size)
            d++;
    }
    //右上角
    i = Board_Size-1;
    j = 0;
    if(Now_Board[i][j] == me){
        num++;
        for(k=1; k<Board_Size; k++){
            if(Now_Board[i-k][j] != me)
                break;
            num++;
        }
        if(k == Board_Size)
            a++;
        for(k=1; k<Board_Size; k++){
            if(Now_Board[i][j+k] != me)
                break;
            num++;
        }
        if(k == Board_Size)
            b++;
    }
    //左下角
    i = 0;
    j = Board_Size-1;
    if(Now_Board[i][j] == me){
        num++;
        for(k=1; k<Board_Size; k++){
            if(Now_Board[i+k][j] != me)
                break;
            num++;
        }
        if(k == Board_Size)
            c++;
        for(k=1; k<Board_Size; k++){
            if(Now_Board[i][j-k] != me)
                break;
            num++;
        }
        if(k == Board_Size)
            d++;
    }
    //右下角
    i = Board_Size-1;
    j = Board_Size-1;
    if(Now_Board[i][j] == me){
        num++;
        for(k=1; k<Board_Size; k++){
            if(Now_Board[i-k][j] != me)
                break;
            num++;
        }
        if(k == Board_Size)
            c++;
        for(k=1; k<Board_Size; k++){
            if(Now_Board[i][j-k] != me)
                break;
            num++;
        }
        if(k == Board_Size)
            b++;
    }
    if(a > 1)
        num -= Board_Size;
    if(b > 1)
        num -= Board_Size;
    if(c > 1)
        num -= Board_Size;
    if(d > 1)
        num -= Board_Size;
    return num;
}
