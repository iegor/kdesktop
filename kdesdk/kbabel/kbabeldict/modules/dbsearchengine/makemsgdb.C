#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <gdbm.h>
#include <string.h>

void removechar (char *s, int c)
{
  int i, l;
  l = strlen (s);
  if ((c >= l) || (c < 0))
    return;
  for (i = c; i < l; i++)
    s[i] = s[i + 1];
}
void removeallc (char *s, char c)
{
  char *pos;
  while ((pos = strchr (s, c)) != 0)
    removechar (s, (long int) pos - (long int) s);

}

void normalizestr (char *s)
{
  char *pos;
  while ((pos = strstr (s, "#~")) != 0) {
    removechar (s, (long int) pos - (long int) s);
    removechar (s, (long int) pos - (long int) s);
    }
  while (strchr (s, ' ') == s)
    removechar (s, 0);
}

void freadline(char *buff,FILE *f)
{
char c;
while ((fread(&c,1,1,f)==1) && (c!='\n'))
	{
	 *buff=c;
	 buff++;
	} 
*buff=0;

}

int makePoDb(const char* sourceName,const char* outputName)
{
static bool open = false;
int m=0,n=0,h=0;
GDBM_FILE db;
datum key,value;
char *s,a[20000],b[2000],k[2000],v[2000];
int i,*np,nmax=0,co=-1,oldref[2000];
long int tim;
FILE *mlf;
bool nextIsFuzzy;
bool isAMsgId=true;
/* char keystring[1000],valuestring[1000]; */

/*Read headers, refnum end other info */
db = gdbm_open((char *)outputName,1024,GDBM_READER,0666,0);
mlf = fopen(sourceName,"r");
if(strrchr(sourceName,'/')!=0)
sourceName=strrchr(sourceName,'/')+1;

if(!(db==0))
  {
   printf("ciao\n");
   key.dptr=a;
   strcpy(a,"__@REFNUM__");
   key.dsize=strlen(a)+1;
   value = gdbm_fetch(db,key);
   np=(int*)value.dptr;        
   nmax=*np;
   for(i=0;i<nmax;i++) {
        sprintf(a,"__@%d__",i);
	key.dsize=strlen(a)+1;
        value = gdbm_fetch(db,key);
  	if(strcmp(value.dptr,sourceName)==0)
	 oldref[++co]=i; 
    //    printf("File %s in Date %s\nRef # %d, oldref[%d]=%d\n",value.dptr,value.dptr+1+strlen(value.dptr),i,co,oldref[co]);  
      }
   gdbm_close(db);
  }


db = gdbm_open((char *)outputName,1024,GDBM_WRCREAT,0666,0);
if(db==0) return 0;

nmax++;
//sourceName=strrchr(sourceName,'/')+1;
 if(open) return 0;
 open=true;
 
 key.dptr=a;
 strcpy(a,"__@REFNUM__");
 key.dsize=strlen(a)+1;
 value.dptr=(char *)&nmax;
 value.dsize=4;
 gdbm_store(db,key,value,GDBM_REPLACE);
 sprintf(a,"__@%d__",nmax-1);
 key.dsize=strlen(a)+1;
 strcpy(v,sourceName);
 value.dptr=v;
 time(&tim);
// ctime(&tim);
  // fprintf(stderr,"%s %s\n",v,ctime(&tim));
 strcpy(v+strlen(v)+1,ctime(&tim));
 value.dsize=strlen(v)+2+strlen(v+strlen(v)+1);
 gdbm_store(db,key,value,GDBM_REPLACE); 
  
    while(!feof(mlf)) 
      {
	freadline(a,mlf);
	normalizestr(a);
//	printf("#%s#\n",a);

//	while (st.find("#~")==0)
//	  st = st.right(st.length()-2); 
//	while (st.find(" ")==0)
//	  st = st.right(st.length()-1);
        
	if(isAMsgId) nextIsFuzzy=false; 
	if(strstr(a,"#,")==a)
	 if(strstr(a,"fuzzy")!=0) nextIsFuzzy=true;
	isAMsgId=(strstr(a,"msgid"));
	if(isAMsgId && !nextIsFuzzy)
	   {
	    *b='\0';
	    clearerr(mlf);    
	    while(!feof(mlf) && !(strstr(a,"msgstr")==a)) 
	      { 
		strcat(b,strchr(a,'"')+1);   
	        *strrchr(b,'\"')= '\0'; //"
		freadline(a,mlf);
		normalizestr(a);


		if(b+strlen(b)==strstr(b,"\\n")+2)
		{
		 b[strlen(b)-2]='\n'; 
		 b[strlen(b)-1]='\0'; 
	        }
	      } 
	      
    	//     fprintf(stderr,"MSGID#%s#\n",b);
	    //} 
	  
	if(b[0]!='\0') {
	key.dptr=k;
        strcpy(k,b);
	key.dsize=strlen(key.dptr)+1;
	int lines=0;
	*b='\0'; 
	while(!feof(mlf) && !(strstr(a,"msgid")==a) && !(strchr(a,'"')==0)) 
	      {
		lines++;
		strcat(b,strchr(a,'"')+1);
	        *strrchr(b,'\"')= '\0'; //"
		freadline(a,mlf);
		normalizestr(a);


		if(b+strlen(b)==strstr(b,"\\n")+2)
		{
		 b[strlen(b)-2]='\n'; 
		 b[strlen(b)-1]='\0'; 
		}
	      }
        value.dptr=v;
	int *t;
	int *nr,*re; //,*md;   //Number of references,ref

	t=(int *)value.dptr;
	*t=1;
	strcpy(value.dptr+4,b);
	nr=(int *)(value.dptr+4+strlen(value.dptr+4)+1);
	*nr=1;
	nr++;
	*nr=nmax-1;
	value.dsize=strlen(value.dptr+4)+1+4+8;
        //  fprintf(stderr,"MSGSTR#%s#nref=%d\n",value.dptr+4,
//  			    *(int*)(value.dptr+4+strlen(value.dptr+4)+1 ));
	    
        if(b[0]!='\0') {
	    if(gdbm_store(db,key,value,GDBM_INSERT))
	     {
              // fprintf(stderr,"*** Key exists ***\n");

	     gdbm_sync(db);
	     gdbm_close(db);
	     db = gdbm_open((char *)outputName,1024,GDBM_READER,0,0);
	     datum oldvalue;
	     oldvalue= gdbm_fetch(db,key);
	     gdbm_sync(db);
	     gdbm_close(db);
	     db = gdbm_open((char *)outputName,1024,GDBM_WRCREAT,0,0);
	     	  
	     t=(int *)oldvalue.dptr;
	     int i,r,j;   //counters
	     int v=*t;   		//Number of strings
	     int *nr,*re,*md;   //Number of references,ref
	     bool exist=false,here,modif=false;
	     char *os=oldvalue.dptr+4;     
  	//     fprintf(stderr,"**Searching string #%s#\n",b);
	     for(i=0;i<v;i++)
	      {

	       exist=false;  //REMOVE THIS LINE!!!
	       here=false;
   	      // fprintf(stderr,"**STRING %d #%s# len=%d    %s\n",i,os,strlen(os),b);
	       if(strcmp(os,b)==0) {
	         exist=true;
	         here=true;
	//	 fprintf(stderr,"That's good\n"); 
		 }
	       os+=strlen(os)+1;   //End of string
	       nr=(int *)os;
	       os+=(*nr+1)*4;
	       re=nr;
	       modif=false;
	//       fprintf(stderr,"refernces %d\n",*nr);
	       for(j=0;j<*nr;j++)
	         {
		  re++;

	          if(here) 
		  {
		 // printf("reference #%d\n",*re);
		  for(r=0;r<=co;r++) 
		  {
		  // fprintf(stderr,"%d==%d ?-->",oldref[co],*re);
		    if(oldref[r]==*re)
		       {
		       modif=true;
	//	       fprintf(stderr,"Yes\n");
		       *re=(nmax-1);
		       } //else  fprintf(stderr,"No\n");
                   }
		//  fprintf(stderr,"qui\n");
		   if(!modif)
		      md=nr;
		//  fprintf(stderr,"modif %s\n",modif ? "true":"false");
		  }
		 }
	      
	      }
	     
	     if(!exist)
	         {
		  int oldlen=(long int)os-(long int)oldvalue.dptr-4; 
	          memcpy(a+4,oldvalue.dptr+4,oldlen); 
//	         fprintf(stderr,"***!exist Old len is %d+4 1st str is %s\n",oldlen,a+4);
		  v++;
		  t=(int *)a;
		  *t=v;
	//	    fprintf(stderr,"b=%s",b);
	          strcpy(a+4+oldlen,b);
		  re=(int *)(a+4+oldlen+strlen(b)+1);
		  *re=1;
		  re++;
		  *re=nmax-1;
		  //fprintf(stderr,"a+4=%s  a+4+oldlen=%s",a+4,a+4+oldlen);
                  value.dptr=a;
		  value.dsize=oldlen+strlen(b)+1+4+8;
		  gdbm_store(db,key,value,GDBM_REPLACE);
		  n++;
		 } else 
		 {
		  if(!modif)
		   {
//		    fprintf(stderr,"grossa crisi %d\n",*md);
//		    fprintf(stderr,"Old num of ref \n");
		    int oldlen1=(long int)(md)-(long int)(oldvalue.dptr)-4; 
		    int oldlen2=(long int)(os)-(long int)(md)-4; //-4 because nr  
	            memcpy(a+4,oldvalue.dptr+4,oldlen1); 
		    memcpy(a+4+oldlen1+8,oldvalue.dptr+4+oldlen1+4,oldlen2);
		    re=(int *)(a+4+oldlen1); 
		    *re=(*md )+1;
		    // *re++;
		    re++;
		    *re=nmax-1;
		    t=(int *)a;
		    *t=v;
                    value.dptr=a;
		    value.dsize=oldlen1+oldlen2+4+8;
		    gdbm_store(db,key,value,GDBM_REPLACE);
		    n++; 
		   }
		    else  //if (modif) 
		    {
		     value.dptr=oldvalue.dptr;
		     value.dsize=oldvalue.dsize;
		     gdbm_store(db,key,value,GDBM_REPLACE);
		    }
		 }
		 
	       h++;
	     } else  {
	     
	              m++;
		     }
	 }
	}
	
      } 
      
      
      }
      
 fclose(mlf); 
 gdbm_close(db);
 open=false;
 printf("new Key in database %d\n old key found %d\n value added %d\n",m,h,n); 
 return m+n;
}



main(int argc,char **argv)
{
int i;
for(i=1;i<argc;i++)
 printf("File %s:\nEntry added to dbase: %d\n",argv[i],makePoDb(argv[i],"messages2.db"));

}
