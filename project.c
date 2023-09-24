#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
struct NODE{
    int id;
    double x; // x 座標
    double y; // y 座標
};

int nodes, links, power, noise;
float parent, child;

double numerator( double x1, double x2, double y1, double y2){ // 計算link的 P/dl [SINR的分子]
    double result, x, y, dl;
    x = pow(fabs(x1-x2),2);
    y = pow(fabs(y1-y2),2);
    dl = pow(sqrt(x+y),3);
    result = power/dl;
    return result;
} // numerator()

int beforeSINR( float flink[][5], int finallink[][4], double t[][nodes+1], int curx, int cury){ // 判斷是否與先前的link也符合SINR
    float x, y, tmp, result;
    for( int i = 0; i < links && flink[i][0] != -1 ; i++ ){
        x = pow(fabs(curx-flink[i][3]), 2);
        y = pow(fabs(cury-flink[i][4]), 2);
        tmp = pow(sqrt(x+y), 3);
        tmp = flink[i][2] + (power/tmp); // 計算加入這條link後與前面sinr的分母
        result = flink[i][1]/tmp;
        if( result <= 1 ) // 若小於等於一直接結束並return 0
            return 0;
    }
    return 1;
} // beforeSINR()

int SINR( float flink[][5], int finallink[][4], double t[][nodes+1], int send, int get, struct NODE node[] ){
    float denominator = 0.0, x, y, result, tmp;
    float t1 = node[send].x, t2 = node[send].y; // 目前link的transmitter的X座標與Y座標
    if( beforeSINR( flink, finallink, t, t1, t2 ) == 0 ) return 0;
    //先判斷目前該link的transmitter是否與之前所存link的receiver符合SINR > 1
    //若不符合直接return 0 ; 符合就繼續往下做
    float curx = node[get].x, cury = node[get].y; // 目前link的receiver的X座標與Y座標
    for( int i = 0; finallink[i][0] != -1 ; i++ ){ // 計算目前link的receiver與之前link的transmitter的SINR分母
        x = pow(fabs(node[finallink[i][1]].x-curx), 2);
        y = pow(fabs(node[finallink[i][1]].y-cury), 2);
        tmp = pow(sqrt(x+y),3);
        result = power/tmp;
        denominator+=result;
    } // for
    float sinr = t[send][get]/(denominator+noise);
    child = t[send][get];// sinr的分子
    parent = denominator+noise; // sinr的分母
    if( sinr > 1 ) return 1;
    else return 0;
} // SINR()

int findid( int transmit, int recieve, int link[][3]){ // find the id of link and return
    for( int i = 0; i < links; i++)
        if( link[i][1] == transmit && link[i][2] == recieve )
            return link[i][0];
    return -1;
} // findid()

void Select( struct NODE node[], double t[][nodes+1], int link[][3] ){
    int finallink[nodes][4]; // 存放最終選擇的link
    memset( finallink, -1, sizeof(finallink)); // 初始為-1
    float flink[nodes][5];
    // 存放已選擇link的receiver;
    // 0：link_id  1：SINR的分子 2：SINR的分母 3：X座標 4：Y座標
    for( int i = 0; i < nodes; i++)
        for( int j = 0; j < 5; j++)
            flink[i][j] = -1;
    double max = -1.0;
    int cur = -1, i, j;
    int visit[nodes]; // 用來判斷是否拜訪過
    memset( visit, 0, sizeof(visit));
    int a = 0;
    for( i = 0; i < nodes; i++ ){ //從node_id = 0 開始找
        if( visit[i] == 1 ) continue; // 如果此點拜訪過就直接跳下一點
        max = -1;
        for( j =0; j < nodes;j++) // 找目前這點(Transmitter)與其Receiver的 P/d1 [SINR分子]最大者
            if ( t[i][j] != -1 &&  t[i][j] > max && visit[j] == 0  ){
                max = t[i][j];
                cur = j;
            }
        if(SINR( flink, finallink, t, i, cur, node) == 1){
            finallink[a][0] = flink[a][0]= findid( i, cur, link);
            finallink[a][1] =link[finallink[a][0]][1];
            finallink[a][2] = link[finallink[a][0]][2];
            flink[a][1] = child;
            flink[a][2] = parent;
            flink[a][3] = node[finallink[a][2]].x;
            flink[a][4] = node[finallink[a][2]].y;
            for( int k = 0; k < a && flink[k][0] != -1 ; k++ ){ // 更新flink中SINR的分母
                float x = pow(fabs(node[i].x-flink[k][3]), 2);
                float y = pow(fabs(node[i].y-flink[k][4]), 2);
                float tmp = pow(sqrt(x+y), 3);
                tmp = flink[k][2] + (power/tmp);
                flink[k][2] = tmp;
            }
            visit[cur] = visit[i] = 1;
            a++;
        }
        else continue; //SINR不符合直接跳下一個node
    }
    printf("\n# Accepted Links\n");

    printf("%d\n", a);
    printf("LinkID  Transmitter  Receiver\n");
    for( int i = 0; i < a; i++)
        printf("%d\t%d\t     %d\n", finallink[i][0], finallink[i][1], finallink[i][2]);
    // 印出OUTPUT
}

int main(){
    printf("#Nodes  #Links  Power   Basic_Noise\n");
    scanf("%d%d%d%d", &nodes, &links, &power, &noise);
    int link[links][3];
    double t[nodes][nodes+1]; //t:紀錄有傳給誰
    for( int i = 0; i < nodes; i++)
        for( int j = 0; j < nodes+1; j++)
            t[i][j] = -1;
    struct NODE node[nodes];
    printf("node_ID  X_pos  Y_pos\n");
    for( int i = 0; i < nodes; i++)
        scanf("%d%lf%lf", &node[i].id, &node[i].x, &node[i].y);
    printf("LinkID  Transmitter  Receiver\n");
    for( int i = 0; i < links; i++){
        scanf("%d%d%d", &link[i][0], &link[i][1], &link[i][2]);
        t[link[i][1]][link[i][2]] =  numerator( node[link[i][1]].x, node[link[i][2]].x, node[link[i][1]].y, node[link[i][2]].y );
    }
    Select( node, t, link); //找link

    /*
    -1.000000  35.355339   0.754293  -1.000000  -1.000000  -1.000000
    35.355339  -1.000000  -1.000000  -1.000000   3.162278   1.118034
    -1.000000  -1.000000  -1.000000   8.944272  -1.000000  -1.000000
     2.133462  -1.000000  -1.000000  -1.000000  -1.000000  -1.000000
    -1.000000  -1.000000  -1.000000  -1.000000  -1.000000  35.355339
    -1.000000  -1.000000  12.500000  -1.000000  -1.000000  -1.000000
    */
}
