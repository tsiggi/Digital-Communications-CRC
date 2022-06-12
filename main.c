#include <stdio.h>     /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#define k 20
#define BER 0.001
#define max 1000000000

void print_Str(char*, int);                                                 // used for debugging
void read_binary(char*, int*);
int check_HelpingFunction(const char*, int*);
void get_Number(char*, int*);
void get_2nkD(char *,const char*, int);
char* get_R(const char* D_2nk, int n,char* P,int size_p,int* size_of_r);
int subtract_mod2(char* ,const char* ,int);
char* get_T(const char* D_2nk,int size_of_D2nk, const char* FSC, int size_of_FCS);
int CRS(char* T, int size_of_T, char* P, int size_of_p);
int BitErrorRate(char* T,int size_of_T ,double ber);

int i;

int main(void) {
    int known_errors = 0 ;                                                  // Errors that we KNOW  from BitErrorRate function
    int found_errors = 0 ;                                                  // Errors that we FOUND from CRS function

    time_t t;

    //
    char P[6] = {'1','1','0','1','0','1'};
    int size_p = 6;
    //read_binary(P, &size_p);      // Binary Number P that is inserted from user
    //print_Str(P,size_p);

    // initialize random seed: /
    srand (time(&t));

    int n = k + size_p - 1;                                             // n = size of D_2nk and T
    int size_d;
    int size_fcs;                                                       // size of FCS
    char D[k] = {0};
    char *D_2nk = malloc(n * sizeof(char));                        //initialize the 2^(n-k) * D  that has n bits
    char *T;
    char *FCS;

    int temp = 0;


    for(int j=0;j<=max;j++) {

        if(j%5000000 == 0)
            printf("Everything good so far. Done = %.1f%\n",(j/(1.0*max))*100);


        // 1st Stage                            Create a binary message, with the same possibility for 0,1 in every position


        //D is a char array that is randomly created with maximum k bits (maximum because it may return 00011 so k=2 & not 5)
        get_Number(D, &size_d);
        // print_Str(D, size_d);


        //2nd Stage                               Calculate CRC(FCS) for this message
        //char F[]={'1','0','1','0','0','0','1','1','0','1'};               // Array for testing

        // ΠΡΕΠΕΙ ΝΑ ΚΆΝΩ FREE ΤΟΝ ΧΏΡΟ ΠΟΥ ΔΈΣΜΕΥΣΑ
        get_2nkD(D_2nk, D, n);                                              // calls the function that calculates 2^(n-k) * D

        // print_Str(D_2nk, n);



        // FCS = remainder of D_2nk / P  ( modulo-2 calculations !!! )
        FCS = get_R(D_2nk, n, P, size_p, &size_fcs);          // calls the function that calculates and returns the FCS sequence of n-k bits
        // print_Str(FCS, size_fcs);

        T = get_T(D_2nk, n, FCS, size_fcs);  // calls the function that calculates and returns the  T  sequence of  n bits
        // print_Str(T, n);


        known_errors += BitErrorRate(T,n,BER);                              // Returns 1 if this Message was changed because of the transfer

        // print_Str(T, n);
        if (0 == CRS(T, n, P, size_p) ){                                     // if T / P (modulo-2 calc) remainder is not 0 then WE HAVE AT LEAST ONE BIT CHANGE WHILE TRANSFERRING THE DATA
            //printf("There was a problem!!");
            found_errors ++ ;
        }
        free(FCS);
        free(T);
    }

    printf("\nMessages that got an error: %d %.8f% \nMessages found to have an error from CRC: %d %.8f%\n",known_errors ,(known_errors/(double)max)*100,found_errors , (found_errors / (double)max) * 100);
    printf("\nPercentage of messages that have an error and are not traced from CRC : %.8f%\n",((known_errors-found_errors)/(double)known_errors)*100);

    return 0;
}

void print_Str(char* str,int n){
    for(i = 0; i< n;i++)
        printf("%c",str[i]);
    printf("\n");
}

void read_binary(char *str,int *size){
    // str = store the input (BINARY) as an array of char (0,1)
    // size = size of STR

    // read INPUT & check if it is BINARY
    printf("Give a binary number :");
    scanf("%s",str);

    int flag = check_HelpingFunction(str , size);

    while(flag){
        printf("\n\nTRY AGAIN!!!\nGive a BINARY number (starts with 1 & smaller than 30 bits) [e.g 110001] :  ");
        scanf("%s",str);

        flag = check_HelpingFunction(str , size);
    }
}

int check_HelpingFunction(const char* p,int *size){
    if(p[0]=='0')
        return 1;

    for(i=0;p[i]!='\0';i++){
        if(p[i]!='1' && p[i]!='0'){
            *size = i;
            return 1;
        }
    }
    *size = i;
    return 0;
}

void get_Number(char* D, int* size){
    for(i = 0; i < k; i++ ){
        double foo = (double)rand()/(double)RAND_MAX;
        D[i] = foo>0.5 ? '1' : '0';
    }
    for(i=0; i < k;i++){
        if(D[i]=='1'){
            *size = k - i;
            i = k;
        }
    }
}

void get_2nkD(char *D_2nk, const char* D , int n) {
    for (i = 0; i < n; i++) {
        if (i < k)
            D_2nk[i] = D[i];
        else
            D_2nk[i] = '0';
    }
}

char* get_R(const char* D_2nk, int n,char* P,int size_p,int* size_of_r){
    int count = 0 , c1 = 0;
    char *temp = malloc(size_p* sizeof(char));
    for(i=0;i<size_p;i++)
        temp[i]=D_2nk[i];
    //print_Str(P,size_p);
    //print_Str(temp,size_p);
    while(n-count>=size_p){
        c1 = subtract_mod2(temp,P,size_p);
        count += c1;
        //print_Str(P,size_p);
        //print_Str(temp,size_p);
        //printf(" %d \n",c1);

        i = - c1 ;
        while(1){
            if(i == 0 || count+size_p+i == n)
                break;
            //printf("%d %d %d %d\n",i,count,size_p+i , count+size_p+i);
            temp[size_p+i] = D_2nk[count+size_p+i];
            //print_Str(temp,size_p);
            i++;
        }
    }
    count = 0;
    for(i=size_p-1;i>=0;i--){
        if(temp[i]=='-')
            count ++;
        else
            break;
    }

    *size_of_r = size_p-count;
    return temp;
}

int subtract_mod2(char* temp,const char* p,int size_of_p)
{
    int flag=0,count=0,j=0;
    for(i=0;i<size_of_p;i++){
        if(flag==0){
            if(temp[i]==p[i])
                count++;
            else{
                flag = 1;
                temp[j] = '1';
                j++;
            }
        }else {
            if(temp[i]==p[i])
                temp[j] = '0';
            else
                temp[j] = '1';
            j++;
        }
    }

    for(;j<size_of_p;j++)
        temp[j]='-';

    return count;
}

char* get_T(const char* D_2nk,int size_of_D2nk, const char* FSC, int size_of_FCS){
    char * T = malloc(size_of_D2nk * sizeof(char));


    for(i = 0 ; i < size_of_D2nk ; i++ )
        T[i] = D_2nk[i];

    for(i = 1; i <= size_of_FCS; i++){
        T[size_of_D2nk - i] = FSC[size_of_FCS -i];
    }

    return T ;
}

int CRS(char* T, int size_of_T, char* P, int size_of_p){
    int int_temp;
    char *temp = get_R(T, size_of_T, P,size_of_p,&int_temp);

    for (i=0;i<int_temp;i++){
        if(temp[i] != '0' && temp[i] !='-'){
            free(temp);
            return 0;                           // Something is Wrong
        }
    }
    free(temp);
    return 1;                               // Everything is Good

}

int BitErrorRate(char* T,int size_of_T ,double ber){
    int flag = 0;

    for(i=0;i<size_of_T;i++){
        double foo = (double)rand()/(double)RAND_MAX;
        if( foo < ber ){
            T[i] = T[i]=='0' ? '1' : '0';
            flag++ ;
        }
    }
    return flag>0 ? 1 : 0;
}

