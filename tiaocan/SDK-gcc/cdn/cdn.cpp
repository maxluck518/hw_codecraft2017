#include "deploy.h"
#include "lib_io.h"
#include "lib_time.h"
#include "stdio.h"

int trans(char * a){
    int i = 0;
    int cnt = 0;
    int tmp = 0;
    while(a[i] != '\0'){
        tmp = a[i] - '0';
        cnt = cnt*10 + tmp;
        i ++;
    }
    return cnt;
}

int main(int argc, char *argv[])
{
    print_time("Begin");
    time_t begin,end;
    begin = time(NULL);

    char *topo[MAX_EDGE_NUM];
    int line_num;

    char *topo_file = argv[1];

    line_num = read_file(topo, MAX_EDGE_NUM, topo_file);

    // printf("line num is :%d \n", line_num);
    if (line_num == 0)
    {
        printf("Please input valid topo file.\n");
        return -1;
    }

    char *result_file = argv[2];

    GA_ELITRATE = float(trans(argv[3]))/100;
    GA_MUTATIONRATE = float(trans(argv[4]))/100;
    SET_VAL = trans(argv[5]);
    char *tmp = argv[6];
    string gcase(tmp);


    int cost = deploy_server(topo, line_num, result_file);

    release_buff(topo, line_num);

    print_time("End");
    end = time(NULL);

    cout << "**********************************************************************" << endl;
    cout << "time:  "<< end - begin << endl;
    cout<<"result for case-"<< gcase <<":  "<< "  " << SET_VAL << "  " <<GA_ELITRATE<< "  " << GA_MUTATIONRATE<< "  "<< cost <<endl;
    cout << "**********************************************************************" << endl;
    cout << endl;

	return 0;
}

