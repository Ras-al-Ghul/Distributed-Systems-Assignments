#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <rpc/rpc.h>
#include "prog.h"

using namespace std;

const int MAX = 10000;

String *sendstring_1_svc(String *vd, struct svc_req *X)
{
    String *tosend;
    tosend = (String *)malloc(sizeof(String));
    tosend->String_val = (char *)malloc(sizeof(char) * 10000);
    
    string temp = "/usr/bin/";
    temp += vd->String_val;
    FILE *fp = popen(temp.c_str(), "r");
    if(fp == NULL){
        printf("Failed to run command\n" );
        exit(1);
    }

    string op = ""; char path[1035];
    while (fgets(path, sizeof(path)-1, fp) != NULL) {
        op+=path;
    }
    pclose(fp);

    strncpy(tosend->String_val, (char *)(op.c_str()), sizeof(char)*(op.length()+1));
    tosend->String_len = op.length()+1;
    return tosend;
}
