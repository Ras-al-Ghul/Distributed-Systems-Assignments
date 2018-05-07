#include <iostream>
#include <rpc/rpc.h>
#include <unistd.h>
#include "prog.h"
#include <string>
#include <cstring>

using namespace std;

int main(int argc, char *argv[])
{
    CLIENT *Client;
    char host[256];

    if(argc <= 1) {
        fprintf(stderr, "usage:./client last args_for_last\n");
        exit(1);
    }
    gethostname(host, 256);

    Client = clnt_create(host, MATHPROG, MATHVERS, "tcp"); //Create a handle for Client
    
    if(Client == NULL)
    {
        cout << "Error: Client not formed" << endl;
        exit(-1);
    }

    string input = "";
    for(int i=1; i<argc; ++i){
        input += argv[i];
        input += " ";
    }
    String h;
    h.String_val = (char *)malloc(sizeof(char) * 10000);
    strncpy(h.String_val, (char *)(input.c_str()), sizeof(char)*(input.length()+1)); 
    h.String_len = input.length()+1;
    String * print = sendstring_1(&h, Client);
    cout<< print->String_val;
    return 0;
}
