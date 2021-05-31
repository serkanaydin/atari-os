#include <ctype.h>


int nxsc() {
    return program[++cpc];    //cpc points to the syntax_table[]
}
int srcont(){
    skblank();
    char temp=cix;
    if(temp != svontx){
        svontx= temp;
        if(search(OPNTAB_STRING,0)){                     //STENUMU AYARLIYOR
            svontc ='\0';
            return 1;
        }
        svontl = bufferIndex;
        temp = OPNTAB[stenum].val.str;                   //
        svontc = temp;
    }
    temp = program[cpc];
    fprintf(stderr,"--SRCONT-- cix=%d temp=%d svontc=%d\n",cix,temp,svontc);
    if(svontc == temp){
        fprintf(stderr,"SRCONT - SETCODE-- cix=%d SVONTC=%d temp=%d\n",cix,svontc,temp);
        setcode(&temp);
        cix = svontl;
        return 0;
    }
    else if(temp >= 0x44) {
        setcode(svontc);
        cix = svontl;
        return 0; }
    else{
        //svontc='\0';              //comment out
        return 1;
    }
}

int erem(){
    return 0;
}

int edata(){
    return 0;
}

int eif(){
    return 0;
}

int echng(){
    outbuff[cox-1] = (char)(program[cpc++]&0x00ff); //outbuff is a char arr but program short arr
    return(0);
}

int fail(){
    while(1){                                           //??
        code=nxsc();
        while (code<2 || code >3 ){
            if (code == 0 ){
                cpc++;
            }
            code=nxsc();
            fprintf(stderr,"--Fail-- code=%d\n",code);
        }
        if(code==3){
            fprintf(stderr,"--Fail-- code=3 stklvl=%d\n",stklvl);

            if(stklvl==0){
                return 1;
            }
            cpc=stack[stklvl-1];                        //??
            stklvl-=4;
            fprintf(stderr,"--Fail-- cpc=%d stklvl=%d\n",cpc,stklvl);
        }
        else{
            if(cix>maxcix){
                maxcix = cix;
            }
            cix = stack[stklvl-3];                  //??
            cox = stack[stklvl-2];                  //??
        }
        return 0;
    }
}

int tncon(){

    skblank();
    tvscix=cix;
    int test=0;
    int i=0;
    while(inbuff[cix+i]<='9' || inbuff[cix+i]>='0'){
        i++;
        test=1;
    }
    if(test==0)
        return 1;
    char token=0x0e;
    printf("TNCON - SETCODE \n");
    setcode(&token);
    int outbuffIndex=0;
    while(outbuffIndex<=i){
        outbuff[cox+outbuffIndex]=inbuff[cix+i];
        outbuffIndex++;
    }
    cox=cox+outbuffIndex+1;
    return 0;
}

int tscon(){
    skblank();
    if(inbuff[cix]!='"')
        return 1;
    char token=0x0f;
    setcode(&token);
    tscox=cox;
    setcode(NULL);
    char ch;
    do{
        cix++;
        ch = inbuff[cix];
        fprintf(stderr,"TSCON - SETCODE \n");
        setcode(&ch);
    }while (!((ch=='\n')||(ch == '"')));
    outbuff[tscox]=cox-tscox;
    return 0;
}
int tvar(int tvtype){
    skblank();
    tvscix=cix;
    if(isalpha(inbuff[cix])) {
        srcont();                               //which table?
       if (svontc == 0 || (svontc == 1) && (inbuff[svontl] >= 0x30)) {  //not reserved word, or it is
           do                                                  //a non-reserved word whose prefix
               cix++;                                //is a reserved word..
           while (isalpha(inbuff[cix]) || isdigit(inbuff[cix]));
       }
      if (tvtype == 0x80 ) {
           if (inbuff[cix] == '$')
               cix++;                          //skip over $
           else
               return 1;
       }
       else if ( tvtype == 0x00 ) {
           if (inbuff[cix] == '(') {
               cix++;      //skip over (
               tvtype += 0x40;
       }
       }
       char temp=cix;
       cix=tvscix;    //search expects the string to be searched pointed by cix.
       tvscix=temp;  //variable string'in sonunu tvscix'e attık.. search
       int result=search(VNTP_HEAD,0);
       while(!result){
           if(bufferIndex==tvscix){
               break;
           }
           else{
               result=search(VNTP_HEAD,1);
           }
       }
       if (result == 1){
           cix=tvscix-cix-1;
           addToTABLE(&VNTP_HEAD,&VNTP_TAIL);
           char tempVname[200];
           memcpy(tempVname,(inbuff+cix-1),cix);
           VNTP_TAIL->name=tempVname;
           addToTABLE(&STMTAB_HEAD,&STMTAB_TAIL);
           svvvte++;
           char* stmtabTemp= (char*)(calloc(8,1));
           STMTAB_TAIL->entry=stmtabTemp;

           if(tvtype==0x40)
               --tvscix;
           cix=tvscix;
           if (stenum>0x7f)
               return 1;
           fprintf(stderr,"TVAR- SETCODE \n");
           setcode(&stenum);
       }
    }
   else{
        return 1;                         //error
                                    //comment silinecek
    }



    /* return 1;*/
    return 0;
}

int tnvar(){
    return tvar(0x00);
}

int tsvar(){
    return tvar(0x80);
}

