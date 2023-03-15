#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
void rodzaj(struct stat statbuf) //wyswietla rodzaj podanego pliku
{
	printf("Typ pliku: ");
	if(S_ISREG(statbuf.st_mode))//uzycie makra S_ISREG (st_mode)
	{
		printf("zwykly plik\n");
	}
	else if(S_ISLNK(statbuf.st_mode))
	{
		printf("link symboliczny\n");
	}
	else if(S_ISDIR(statbuf.st_mode))
	{
		printf("katalog\n");
	}
}
void rozmiar(struct stat statbuf, char* plik) //wyswietla rozmiar pliku a jesli katalog to ilosc podkatalogow
{
	if(S_ISDIR(statbuf.st_mode)) 
	{ 
		DIR *dir;
		int ile=0; //zmienna na sume podkatalogow
		struct dirent *entry; 
		if((dir = opendir(plik))==NULL)
		{
			perror("opendir");
			exit(0);
		}
	    while((entry=readdir(dir))!=NULL)
	    {
			if(entry -> d_type == DT_DIR)
			{
				ile++;	
			}
		}
	    printf("Liczba podkatalogow: %d\n",ile-2);//-2 bo odejmuje . i ..
	}
	else printf("Rozmiar: %ld B\n",statbuf.st_size);	
}
void sciezka(char* plik, struct stat statbuf) //wyswitla sciezke i jesli to link to do czego prowadzi
{
	printf("Sciezka: ");
	char *sciezka=realpath(plik,NULL);
	if(sciezka==NULL)
	{
		perror("blad znajdowanie sciezki");
	}
	if(S_ISLNK(statbuf.st_mode))
	{
		char *sciezka1=realpath(".",NULL);
		if(sciezka1==NULL)
		{
			perror("blad znajdowanie sciezki");
		}
		printf("%s/%s\nWskazuje na: ",sciezka1,plik);
	}
	printf("%s\n",sciezka);
}
void uprawnienia(struct stat statbuf) //wyswietla uprawnienia pliku
{
	printf("Uprawnienia: uzytkownik: ");
	if((statbuf.st_mode & S_IRUSR))
		printf("r"); /* dla ownera czytanie*/
	if((statbuf.st_mode & S_IWUSR))
		printf("w"); /* dla ownera pisanie*/
	if((statbuf.st_mode & S_IXUSR))
		printf("x"); /* dla ownera wykonywanie*/
	printf(", grupa: ");
	if((statbuf.st_mode & S_IRGRP))
		printf("r"); /* dla grupy czytanie*/
	if((statbuf.st_mode & S_IWGRP))
		printf("w"); /* dla grupy pisanie*/
	if((statbuf.st_mode & S_IXGRP))
		printf("x"); /* dla grupy wykonywanie*/
	printf(", inni: ");
	if((statbuf.st_mode & S_IROTH))
		printf("r"); /* dla innych czytanie*/
	if((statbuf.st_mode & S_IWOTH))
		printf("w"); /* dla innych pisanie*/
	if((statbuf.st_mode & S_IXOTH))
		printf("x"); /* dla innych wykonywanie*/
}
void znaki(char* plik, struct stat statbuf) //wyswietla pierwsze 77 znakow
{                                                  /*plik nie ma prawa do wykonywania czyli jest zwyklym plikiem*/
	if(S_ISREG(statbuf.st_mode) && !(statbuf.st_mode & S_IXUSR) && !(statbuf.st_mode & S_IXGRP) && !(statbuf.st_mode & S_IXOTH))//uzycie makra S_ISREG (st_mode)
	{
		int file,n;
		if((file=open(plik, O_RDONLY))==-1)
		{
			perror("blad otwarcia pliku");
		}
		else
		{
			printf("Pierwsze 77 znakow:\n");
			char tekst[77];
			n=read(file,tekst,77);
			write(fileno(stdout),tekst,n);
			close(file);
		}
	}
}
void miesiac( int buf) //funkcja do wypisywania nazw miesiecy po polsku
{
	if(buf==1)
		printf(" stycznia ");
	else if(buf==2)
		printf(" lutego ");
	else if(buf==3)
		printf(" marca ");	
	else if(buf==4)
		printf(" kwietnia ");			
	else if(buf==5)
		printf(" maja ");
	else if(buf==6)
		printf(" czerwca ");
	else if(buf==7)
		printf(" lipca ");
	else if(buf==8)
		printf(" sierpnia ");
	else if(buf==9)
		printf(" wrzesnia ");
	else if(buf==10)
		printf(" pazdziernika ");
	else if(buf==11)
		printf(" listopada ");
	else if(buf==12)
		printf(" grudnia ");				
}
void czasy(struct stat statbuf) //wyswietla czasy uzywania modyfikacji i zmieniania stanu
{
	//zmienne
	time_t tm;
	char buf[200];
	struct tm t;
	int x;
	//uzywany
	tm=statbuf.st_atime;
	t=*localtime(&tm); // pobranie struktury tm ze zmiennej time_tm
	strftime(buf,sizeof(buf),"%d",&t);
	printf("\nOstatnio uzywany: %s",buf);
	strftime(buf,sizeof(buf),"%m",&t);//m
	x=atoi(buf); //rzutowanie 
	miesiac(x);//funkcja od miesiecy po polsku
	strftime(buf,sizeof(buf),"%Y",&t);
	printf("%s ",buf);
	strftime(buf,sizeof(buf),"%H:%M:%S",&t);
	printf("roku o %s ",buf);
	//modyfikowany
	tm=statbuf.st_mtime;
	t=*localtime(&tm);
	strftime(buf,sizeof(buf),"%d",&t);
	printf("\nOstatnio modyfikowany: %s",buf);
	strftime(buf,sizeof(buf),"%m",&t);
	x=atoi(buf);
	miesiac(x);
	strftime(buf,sizeof(buf),"%Y",&t);
	printf("%s ",buf);
	strftime(buf,sizeof(buf),"%H:%M:%S",&t);
	printf("roku o %s ",buf);
	//zmieniany stan
	tm=statbuf.st_ctime;
	t=*localtime(&tm);
	strftime(buf,sizeof(buf),"%d",&t);
	printf("\nOstatnio zmieniany stan: %s",buf);
	strftime(buf,sizeof(buf),"%m",&t);
	x=atoi(buf);
	miesiac(x);
	strftime(buf,sizeof(buf),"%Y",&t);
	printf("%s ",buf);
	strftime(buf,sizeof(buf),"%H:%M:%S",&t);
	printf("roku o %s \n",buf);
}
void info(char* plik)
{
	struct stat statbuf;
	if(lstat(plik,&statbuf)==-1) //potrzebne do kilku funkcji
	{
		perror("lstat");
		exit(0);
	}; 
	rodzaj(statbuf); //wyswietlanie czy katalog czy link czy plik
	sciezka(plik,statbuf); //wyswietlanie sciezki do pliku
	rozmiar(statbuf,plik); //wyswietla rozmiar a jesli katalog to ilosc plikow w katalogu
	uprawnienia(statbuf); //wyspwietla uprawnienia pliku rwx
	czasy(statbuf);//wypisuje info o czasach
	znaki(plik,statbuf); //wypisuje pierwsze 77 znakow
}
char * rodz_upr(struct stat statbuf) //zwraca wskaznik na tablice char zawiezajaca rodzja pliku i uprawnieniach
{
	char* x;
	x=(char*) malloc(10*sizeof(char));
	if(S_ISREG(statbuf.st_mode))
	{
		x[0]='-';
	}
	else if(S_ISLNK(statbuf.st_mode))
	{
		x[0]='l';
	}
	else if(S_ISDIR(statbuf.st_mode))
	{
		x[0]='d';
	}
	if((statbuf.st_mode & S_IRUSR)) x[1]='r'; 
	else x[1]='-';
	
	if((statbuf.st_mode & S_IWUSR)) x[2]='w'; 
	else x[2]='-';
	
	if((statbuf.st_mode & S_IXUSR)) x[3]='x'; 
	else x[3]='-';
	
	if((statbuf.st_mode & S_IRGRP)) x[4]='r'; 
	else x[4]='-';
	
	if((statbuf.st_mode & S_IWGRP)) x[5]='w'; 
	else x[5]='-';
	
	if((statbuf.st_mode & S_IWGRP)) x[6]='x'; 
	else x[6]='-';
	
	if((statbuf.st_mode & S_IROTH)) x[7]='r'; 
	else x[7]='-';
	
	if((statbuf.st_mode & S_IWOTH)) x[8]='w'; 
	else x[8]='-';
	
	if((statbuf.st_mode & S_IWOTH)) x[9]='x'; 
	else x[9]='-';
	return x;
	free(x);
}
void ls()
{
	time_t tm;//zmienna do pobrania czasu
	char buf[100]; //zmienna na date modyfikacji
	struct tm t; //zeby uzyc strftime
	struct dirent **namelist; //przechowuje wszystkie pliki katalogu
	struct stat statbuf;
	int i,n;
	n=scandir(".",&namelist,0,alphasort); 
    if (n < 0)
        perror("scandir");
    else 
	{
        for (i = 0; i < n; i++) 
		{
			if(lstat(namelist[i]->d_name,&statbuf)==-1) 
			{
				perror("lstat");
				exit(0);
			}; 
			printf("%s ", rodz_upr(statbuf));//rodzaj pliku i uprawnienia (nie trzeba formatowac bo jest zawsze 10)
				
			printf("%ld ", statbuf.st_nlink);//liczba linkow
				
			struct passwd* pw;
			pw=getpwuid(statbuf.st_uid);
			printf("%s ", pw->pw_name);// nazwa wlasciciela
				
			struct group* gr;
			gr=getgrgid(statbuf.st_gid);
			printf("%s ",gr->gr_name);//nazwa grupy
				
			printf("%ld ", statbuf.st_size);//rozmiar w bajtach
			
			tm=statbuf.st_mtime;
			t=*localtime(&tm);
			strftime(buf,sizeof(buf),"%b %d %H.%M",&t);
			printf("%s ",buf);//data ostatniej modyfikacji
			if(S_ISLNK(statbuf.st_mode))
			{
				char *buff; //zmienna na nazwe linku
				ssize_t bufsiz, nbytes;
				bufsiz = statbuf.st_size + 1; //jej wielkosc
				buff=malloc(bufsiz);
				if (buff == NULL) 
				{
		            perror("malloc");
		            exit(0);
	            }
	            nbytes = readlink(namelist[i]->d_name, buff, bufsiz);
	            if (nbytes == -1) 
				{
	               perror("readlink");
	               exit(EXIT_FAILURE);
	           }
				printf("%s -> ",namelist[i]->d_name); //wypisanie plik -> link
				printf("%s\n",buff);
				free(buff);
			}
			else printf(" %s\n", namelist[i]->d_name);//nazwa
		    free(namelist[i]);
        }
    }
    free(namelist);
}
int main(int argc, char** argv)
{
	if(argc==2)
	{
		printf("Informacje o %s: \n",argv[1]);//wypisanie info o pliku
		info(argv[1]);
	}	
	else if(argc==1)
	{
		ls();
	}
	else
	{
		printf("Zla liczba argumntow \nPoprawne uzycie: \n %s lub %s plik",argv[0],argv[0]);
	}
	return 0;
}

