#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
struct NODE{
    int id;
    double x; // x �y��
    double y; // y �y��
};

int nodes, links, power, noise;
float parent, child;

double numerator( double x1, double x2, double y1, double y2){ // �p��link�� P/dl [SINR�����l]
    double result, x, y, dl;
    x = pow(fabs(x1-x2),2);
    y = pow(fabs(y1-y2),2);
    dl = pow(sqrt(x+y),3);
    result = power/dl;
    return result;
} // numerator()

int beforeSINR( float flink[][5], int finallink[][4], double t[][nodes+1], int curx, int cury){ // �P�_�O�_�P���e��link�]�ŦXSINR
    float x, y, tmp, result;
    for( int i = 0; i < links && flink[i][0] != -1 ; i++ ){
        x = pow(fabs(curx-flink[i][3]), 2);
        y = pow(fabs(cury-flink[i][4]), 2);
        tmp = pow(sqrt(x+y), 3);
        tmp = flink[i][2] + (power/tmp); // �p��[�J�o��link��P�e��sinr������
        result = flink[i][1]/tmp;
        if( result <= 1 ) // �Y�p�󵥩�@����������return 0
            return 0;
    }
    return 1;
} // beforeSINR()

int SINR( float flink[][5], int finallink[][4], double t[][nodes+1], int send, int get, struct NODE node[] ){
    float denominator = 0.0, x, y, result, tmp;
    float t1 = node[send].x, t2 = node[send].y; // �ثelink��transmitter��X�y�лPY�y��
    if( beforeSINR( flink, finallink, t, t1, t2 ) == 0 ) return 0;
    //���P�_�ثe��link��transmitter�O�_�P���e�Ҧslink��receiver�ŦXSINR > 1
    //�Y���ŦX����return 0 ; �ŦX�N�~�򩹤U��
    float curx = node[get].x, cury = node[get].y; // �ثelink��receiver��X�y�лPY�y��
    for( int i = 0; finallink[i][0] != -1 ; i++ ){ // �p��ثelink��receiver�P���elink��transmitter��SINR����
        x = pow(fabs(node[finallink[i][1]].x-curx), 2);
        y = pow(fabs(node[finallink[i][1]].y-cury), 2);
        tmp = pow(sqrt(x+y),3);
        result = power/tmp;
        denominator+=result;
    } // for
    float sinr = t[send][get]/(denominator+noise);
    child = t[send][get];// sinr�����l
    parent = denominator+noise; // sinr������
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
    int finallink[nodes][4]; // �s��̲׿�ܪ�link
    memset( finallink, -1, sizeof(finallink)); // ��l��-1
    float flink[nodes][5];
    // �s��w���link��receiver;
    // 0�Glink_id  1�GSINR�����l 2�GSINR������ 3�GX�y�� 4�GY�y��
    for( int i = 0; i < nodes; i++)
        for( int j = 0; j < 5; j++)
            flink[i][j] = -1;
    double max = -1.0;
    int cur = -1, i, j;
    int visit[nodes]; // �ΨӧP�_�O�_���X�L
    memset( visit, 0, sizeof(visit));
    int a = 0;
    for( i = 0; i < nodes; i++ ){ //�qnode_id = 0 �}�l��
        if( visit[i] == 1 ) continue; // �p�G���I���X�L�N�������U�@�I
        max = -1;
        for( j =0; j < nodes;j++) // ��ثe�o�I(Transmitter)�P��Receiver�� P/d1 [SINR���l]�̤j��
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
            for( int k = 0; k < a && flink[k][0] != -1 ; k++ ){ // ��sflink��SINR������
                float x = pow(fabs(node[i].x-flink[k][3]), 2);
                float y = pow(fabs(node[i].y-flink[k][4]), 2);
                float tmp = pow(sqrt(x+y), 3);
                tmp = flink[k][2] + (power/tmp);
                flink[k][2] = tmp;
            }
            visit[cur] = visit[i] = 1;
            a++;
        }
        else continue; //SINR���ŦX�������U�@��node
    }
    printf("\n# Accepted Links\n");

    printf("%d\n", a);
    printf("LinkID  Transmitter  Receiver\n");
    for( int i = 0; i < a; i++)
        printf("%d\t%d\t     %d\n", finallink[i][0], finallink[i][1], finallink[i][2]);
    // �L�XOUTPUT
}

int main(){
    printf("#Nodes  #Links  Power   Basic_Noise\n");
    scanf("%d%d%d%d", &nodes, &links, &power, &noise);
    int link[links][3];
    double t[nodes][nodes+1]; //t:�������ǵ���
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
    Select( node, t, link); //��link

    /*
    -1.000000  35.355339   0.754293  -1.000000  -1.000000  -1.000000
    35.355339  -1.000000  -1.000000  -1.000000   3.162278   1.118034
    -1.000000  -1.000000  -1.000000   8.944272  -1.000000  -1.000000
     2.133462  -1.000000  -1.000000  -1.000000  -1.000000  -1.000000
    -1.000000  -1.000000  -1.000000  -1.000000  -1.000000  35.355339
    -1.000000  -1.000000  12.500000  -1.000000  -1.000000  -1.000000
    */
}
