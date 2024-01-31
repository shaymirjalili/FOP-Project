#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#define invalid printf("Invalid command!")
#define MAX_GIT_PROJECTS 200
#define MAX_LENGTH 40
#define MAX_LINE_LENGTH 1000
char cwd[1024];
/*int tracked_in_commit(char* filename){
    char path[1024];
    strcpy(path,current_config->repopath);
    strcat(path,"/commit/");
    strcat(path,branch);
    DIR* go_to_branch=opendir(path);
    struct dirent* ent;
    int exist=0;
    while((ent=readdir(go_to_branch))!=NULL)
        if(strcmp(ent->d_name,filename)==0){
            exist=1;
            break;
        }
    return exist;
}
int modified(char* filepath,char* filename);
int tracked(char* filename);*/
int is_regular_file(const char *path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
    }
int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}
////////////////////////////////////////////////////////////////////////////
int run_add(char* filepath,char* filename){
    FILE* lastadd=fopen(".neogit/lastadd.txt","a");
    struct dirent* entry;
    int exists=0;
    char line[MAX_LINE_LENGTH];
    char line2[MAX_LINE_LENGTH];
    while((fgets(line,MAX_LINE_LENGTH,lastadd))!=NULL){
        line[strlen(line)-1]='\0';
        if(strcmp(filename,line)==0){
            exists=1;
            break;
        }
    }
    char path[1024]=".neogit/stage/";
    strcat(path,filename);
    if(exists){
        FILE* prev=fopen(path,"r");
        FILE* now=fopen(filepath,"r");
        int ident=1;
        while((fgets(line,MAX_LINE_LENGTH,prev))!=NULL && (fgets(line2,MAX_LINE_LENGTH,now))!=NULL )
            if(strcmp(line,line2)!=0){
                ident=0;
                break;
            }
        if(ident==0){
            char tmppath[1024]=".neogit/stage/";
            strcat(tmppath,"tmp");
            FILE* tmp=fopen(tmppath,"w");
            while((fgets(line,MAX_LINE_LENGTH,now))!=NULL)
                fputs(line,tmp);
            remove(path);
            char new[1024]=".neogit/stage/";
            rename(tmppath,path);
            fclose(prev);
            fclose(now);
            fprintf(lastadd,"%s\n",filename);
        }
    }
    else{
        FILE* stage=fopen(path,"w");
        FILE* to_copy=fopen(filepath,"r");
        while((fgets(line,MAX_LINE_LENGTH,to_copy))!=NULL)
            fputs(line,stage);
        fclose(stage);
        fclose(to_copy);
        fprintf(lastadd,"%s\n",filename);
    }

    fclose(lastadd);
    
}
/*int tracked(char* filepath){
    DIR* stagedir=opendir(".neogit/satge");
    struct dirent* stgentry;
    while((entry=readdir(stagedir))!=NULL){
        if(strcmp(entry->d_name,filename)==0){
            closedir(stagedir);
            return 1;
        }
    }
    closedir(stagedir);
    return 0;
}*/
int modified(char* filepath1,char* filepath2){//should use realpath() to identify filepath
    int changed=0;
    FILE* file1=fopen(filepath1,"r");
    FILE* file2=fopen(filepath2,"r");
    if(file2==NULL)printf("*error*\n");
    char line_1[MAX_LINE_LENGTH];
    char line_2[MAX_LINE_LENGTH];
    while((fgets(line_1,MAX_LINE_LENGTH,file1))!=NULL && (fgets(line_2,MAX_LINE_LENGTH,file2))!=NULL){
        if(strcmp(line_1,line_2)!=0){
            changed=1;
            break;
        }
    }
    if((fgets(line_1,MAX_LINE_LENGTH,file1))!=NULL || (fgets(line_2,MAX_LINE_LENGTH,file2))!=NULL)
        changed=1;
    fclose(file1);
    fclose(file2);
    return changed;
}
char* get_file_name(char* filepath){
    while(strchr(filepath,'/')!=NULL){
        filepath=strchr(filepath,'/');
        int n=strlen(filepath);
        for(int i=1;i<n;i++){
            filepath[i-1]=filepath[i];
        }
        //printf("%s\n",filepath);
        filepath[n-1]='\0';
    }
    return filepath;
}
#ifdef _DEBUG_
int main()
{
    int argc=2;
    char const *argv[]={"neogit","init"};
#else
int main(int argc,char const *argv[]){
#endif
    char globalpath[1024]="/mnt/c/c_test/globals.txt";
    char tmpglobpath[1024]="/mnt/c/c_test/tmpglob.txt";
    char *argvtmp[10];
    for(int i=0;i<10;i++)
        argvtmp[i]=malloc(10*sizeof(char));
    int argctmp=argc;
    for(int i=0;i<argc;i++){
        strcpy(argvtmp[i],argv[i]);
    }
    if(argctmp<2)
        invalid;
    if(strcmp(argvtmp[0],"neogit")!=0)
        invalid;

    //                                  //check if a repository exists?
    char tmp_cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;
    int exists = 0;
    struct dirent *entry;
    do {
        // find .neogit
        DIR *dir = opendir(".");
        if (dir == NULL) 
            perror("Error opening current directory");

        while ((entry = readdir(dir)) != NULL) {
            if ((entry->d_type) == 4 && strcmp(entry->d_name, ".neogit") == 0)
                exists = 1;
        }
        closedir(dir);

        // update current working directory
        if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL) return 1;

        // change cwd to parent
        if (strcmp(tmp_cwd, "/") != 0) {
            if (chdir("..") != 0) return 1;
        }

    } while (strcmp(tmp_cwd, "/") != 0);
    // return to the initial cwd
    if (chdir(cwd) != 0) return 1;
    if (exists==0 && (strcmp(argvtmp[1],"init")!=0 && (strcmp(argvtmp[1],"config")!=0 || strcmp(argvtmp[2],"--global")!=0))){
        perror("neogit repository has not been initialized yet. use \"neogit init\" to do so.\n");
    }
 
    else if(exists==0 && strcmp(argvtmp[1],"init")==0){
        if(argctmp!=2) invalid;
        else{
            mkdir(".neogit",0755);
            FILE* global=fopen(globalpath,"r");
            char line[MAX_LINE_LENGTH];
            int email;
            int username;
            int alias;
            char tmp_str[50];
            char string[50];
            fgets(line,MAX_LINE_LENGTH,global);
            sscanf(line,"%s %d %s",tmp_str,&email,string);
            if(email==0) printf("please enter your email: neogit config (--global) user.email \"your email@gmail.com\"\n");
            fgets(line,MAX_LINE_LENGTH,global);
            sscanf(line,"%s %d %s",tmp_str,&username,string);
            if(username==0)
                printf("please enter your username: neogit config (--global) user.name \"your username\"\n");
            FILE* tempp=fopen(".neogit/config.txt","a");
            fgets(line,MAX_LINE_LENGTH,global);
            sscanf(line,"%s %d",tmp_str,&alias);
            fprintf(tempp,"user.email 0\n");
            fprintf(tempp,"user.name 0\n");
            fprintf(tempp,"alias 0\n");
            fclose(tempp);
            mkdir(".neogit/stage",0755);
            mkdir(".neogit/commit",0755);
            FILE* log=fopen(".neogit/log.txt","w");
            fclose(log);
            mkdir(".neogit/commit/master",0755);
            FILE* lastadd=fopen(".neogit/lastadd.txt","w");
            fclose(lastadd);
            FILE* commitcount=fopen(".neogit/commitcount.txt","w");
            fprintf(commitcount,"1234");
            fclose(commitcount);
            FILE* branch=fopen(".neogit/branch.txt","w");
            fprintf(branch,"master");
            fclose(branch);
            printf("initialized your repository successfully!\n");
        }
    }
    else if(exists==1 && strcmp(argvtmp[1],"init")==0)
        printf("neogit repository already exists!\n");

    //Let's check for alias commands
    FILE* global=fopen(globalpath,"r");
    char line[MAX_LINE_LENGTH];
    if(exists==1){
        FILE* config=fopen(".neogit/config.txt","r");
        while((fgets(line,MAX_LINE_LENGTH-1,config))!=NULL){
            if(strncmp("alias",line,5)==0){
                char alias[30];
                char equiv[30];
                while((fgets(alias,29,config))!=NULL){
                    alias[strlen(alias)-1]='\0';
                    fgets(equiv,29,config);
                    equiv[strlen(equiv)-1]='\0';
                    if(strcmp(alias,argvtmp[1])==0){
                        char* pch=strtok(equiv," \0\n");
                        argctmp--;
                        while(pch){
                            strcpy(argvtmp[argctmp++],pch);
                            pch=strtok(NULL," \0\n");
                        }
                        break;
                    }
                }
                break;
            }
        }
        fclose(config);
    }
    while((fgets(line,MAX_LINE_LENGTH,global))!=NULL){
        if(strncmp("alias",line,5)==0){
            char alias[30];
            char equiv[30];
            while((fgets(alias,29,global))!=NULL){
                alias[strlen(alias)-1]='\0';
                fgets(equiv,29,global);
                equiv[strlen(equiv)-1]='\0';
                if(strcmp(alias,argvtmp[1])==0){
                    char* pch=strtok(equiv," \0\n");
                    argctmp--;
                    while(pch){
                        printf("%s\n",pch);
                        strcpy(argvtmp[argctmp++],pch);
                        pch=strtok(NULL," \0\n");
                    }
                    break;
                }
            }
            break;
        }
    }
    fclose(global);
    //config command:
    if(strcmp(argvtmp[1],"config")==0){
        if(strcmp(argvtmp[2],"--global")==0){
            FILE* global=fopen(globalpath,"r");
            FILE* tmpglob=fopen(tmpglobpath,"w");
            char line[MAX_LINE_LENGTH];
            if(strcmp(argvtmp[3],"user.name")==0){
                if(argctmp!=5) invalid;
                else{
                    while((fgets(line,MAX_LINE_LENGTH,global))!=NULL){
                        if(strncmp("user.name",line,9)==0)
                            fprintf(tmpglob,"user.name 1 %s\n",argvtmp[4]);
                        else fputs(line,tmpglob);
                    }
                    remove(globalpath);
                    rename(tmpglobpath,globalpath);
                    fclose(global);
                    printf("set username \"%s\" globally!\n",argvtmp[4]);
                }
            }
            else if(strcmp(argvtmp[3],"user.email")==0){
                if(argctmp!=5) invalid;
                else{
                    while((fgets(line,MAX_LINE_LENGTH,global))!=NULL){
                        if(strncmp("user.email",line,9)==0)
                            fprintf(tmpglob,"user.email 1 %s\n",argvtmp[4]);
                        else fputs(line,tmpglob);
                    }
                    remove(globalpath);
                    rename(tmpglobpath,globalpath);
                    fclose(global);
                    printf("set email \"%s\" globally!\n",argvtmp[4]);
                }
            }
            else if(strncmp(argvtmp[3],"alias.",6)==0){
                if(argctmp!=5) invalid;
                else{
                    while((fgets(line,MAX_LINE_LENGTH,global))!=NULL){
                        if(strncmp("alias",line,5)==0){
                            int n;
                            sscanf(line,"alias %d",&n);
                            fprintf(tmpglob,"alias %d\n", n+1);
                            char* alias=calloc(20,sizeof(char));
                            alias=strstr(argvtmp[3],".");
                            int len=strlen(alias);
                            for(int i=1;i<len;i++)
                                alias[i-1]=alias[i];
                            alias[len-1]='\0';
                            strcat(alias,"\n");
                            fputs(alias,tmpglob);
                            fputs(argvtmp[4],tmpglob);
                            fprintf(tmpglob,"\n");
                        }
                        else fputs(line,tmpglob);
                    }
                    remove(globalpath);
                    rename(tmpglobpath,globalpath);
                    fclose(global);
                    printf("set alias globally!\n");
                }
            }
        }
        else if(strcmp(argvtmp[2],"--global")!=0){
            if(argctmp!=4) invalid;
            else{
                FILE* local=fopen(".neogit/config.txt","r");
                FILE* tmplocal=fopen(".neogit/configtmp.txt","w");
                char line[MAX_LINE_LENGTH];
            if(strcmp(argvtmp[2],"user.name")==0){
                    while((fgets(line,MAX_LINE_LENGTH,global))!=NULL){
                        if(strncmp("user.name",line,9)==0)
                            fprintf(tmplocal,"user.name 1 %s\n",argvtmp[3]);
                        else fputs(line,tmplocal);
                    }
                    remove(".neogit/config.txt");
                    rename(".neogit/configtmp.txt",".neogit/config.txt");
                    fclose(local);
                    printf("set username \"%s\" for your project!\n",argvtmp[3]);
            }
            else if(strcmp(argvtmp[3],"user.email")==0){
                    while((fgets(line,MAX_LINE_LENGTH,global))!=NULL){
                        if(strncmp("user.email",line,9)==0)
                            fprintf(tmplocal,"user.email 1 %s\n",argvtmp[3]);
                        else fputs(line,tmplocal);
                    }
                    remove(".neogit/config.txt");
                    rename(".neogit/configtmp.txt",".neogit/config.txt");
                    fclose(local);
                    printf("set email \"%s\" for your project!\n",argvtmp[3]);
            }
            else if(strncmp(argvtmp[2],"alias.",6)==0){
                    while((fgets(line,MAX_LINE_LENGTH,global))!=NULL){
                        if(strncmp("alias",line,5)==0){
                            int n;
                            sscanf(line,"alias %d",&n);
                            fprintf(tmplocal,"alias %d\n", n+1);
                            char* alias=calloc(20,sizeof(char));
                            alias=strstr(argvtmp[2],".");
                            int len=strlen(alias);
                            for(int i=1;i<len;i++)
                                alias[i-1]=alias[i];
                            alias[len-1]='\0';
                            strcat(alias,"\n");
                            fputs(alias,tmplocal);
                            fputs(argvtmp[3],tmplocal);
                            fprintf(tmplocal,"\n");
                        }
                        else fputs(line,tmplocal);
                    }
                    remove(".neogit/config.txt");
                    rename(".neogit/configtmp.txt",".neogit/config.txt");
                    fclose(local);
                    printf("set alias!\n");
            }

            }
        }

    }
    else if(strcmp(argvtmp[1],"add")==0){
        FILE* historyofadds=fopen(".neogit/lastadd.txt","w");
        char filename[1024];
        if(argctmp==3){
            if(is_regular_file(argvtmp[2])){
                strcpy(filename,argvtmp[2]);
                strcpy(filename,get_file_name(filename));
                run_add(argvtmp[2],filename);
                printf("staged the file \"%s\"!\n",argvtmp[2]);
            } 
            else if(isDirectory(argvtmp[2])){
                struct dirent* ent;
                DIR* dir=opendir(argvtmp[2]);
                char filepath[1024];
                while((ent=readdir(dir))!=NULL){
                 if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                    strcpy(filepath,argvtmp[2]);
                    strcat(filepath,"/");
                    strcat(filepath,ent->d_name);
                    run_add(filepath,ent->d_name); 
                 }
                }
                printf("staged the directory \"%s\"!\n",argvtmp[2]);
                closedir(dir);
            }
            else{
                printf("couldn't find file/directory \"%s\"!\n",argvtmp[2]);
            }
        }
        else if(strcmp(argvtmp[2],"-f")==0){
            if(argctmp<4) invalid;
            else{
                for(int i=3;i<argctmp;i++){
                    if(is_regular_file(argvtmp[i])){
                    strcpy(filename,argvtmp[i]);
                    strcpy(filename,get_file_name(filename));
                    run_add(argvtmp[i],filename);
                    printf("staged the file \"%s\"!\n",argvtmp[i]);
                    } 
                    else if(isDirectory(argvtmp[i])){
                    struct dirent* ent;
                    DIR* dir=opendir(argvtmp[i]);
                    char filepath[1024];
                    while((ent=readdir(dir))!=NULL){
                    if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                        strcpy(filepath,argvtmp[i]);
                        strcat(filepath,"/");
                        strcat(filepath,ent->d_name);
                        run_add(filepath,ent->d_name); 
                    }
                    }
                    printf("staged the directory \"%s\"!\n",argvtmp[i]);
                    closedir(dir);
                    }
                    else{
                        printf("couldn't find file/directory \"%s\"!\n",argvtmp[i]);
                    }
                }
            }
        }
        else if(strcmp(argvtmp[2],"-f")!=0 && strcmp(argvtmp[2],"-n")!=0 && argctmp>3){
            for(int i=2;i<argctmp;i++){
                    if(is_regular_file(argvtmp[i])){
                    strcpy(filename,argvtmp[i]);
                    strcpy(filename,get_file_name(filename));
                    run_add(argvtmp[i],filename);
                    printf("staged the file \"%s\" successfully!\n",argvtmp[i]);
                    } 
                    else if(isDirectory(argvtmp[i])){
                        struct dirent* ent;
                        DIR* dir=opendir(argvtmp[i]);
                            while((ent=readdir(dir))!=NULL){
                            if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                            strcpy(filename,argvtmp[i]);
                            strcpy(filename,get_file_name(filename));
                            char path[1024];
                            strcpy(path,argvtmp[i]);
                            strcat(path,"/");
                            strcat(path,ent->d_name);
                            run_add(path,ent->d_name); 
                            }
                            } 
                        closedir(dir);
                        printf("staged the directory \"%s\" successfully!\n",argvtmp[i]);
                    }

                }
            
        }
        else if(strcmp(argvtmp[2],"-n")==0 && strcmp(argvtmp[3],"1")==0){
            if(argctmp!=4) invalid;
            else{
                DIR* current=opendir(".");
                struct dirent* read_current_dir;
                while((read_current_dir=readdir(current))!=NULL){
                    int staged=1;
                    if(strcmp(read_current_dir->d_name,".")!=0 && strcmp(read_current_dir->d_name,"..")!=0){
                        if(read_current_dir->d_type==4){
                            DIR* innerdir=opendir(read_current_dir->d_name);
                            struct dirent* read_inner;
                            while((read_inner=readdir(innerdir))!=NULL){
                                if(strcmp(read_inner->d_name,".")!=0 && strcmp(read_inner->d_name,"..")!=0){
                                    if(read_inner->d_type==8){
                                        char getname[1024];
                                        strcpy(getname,read_inner->d_name);
                                        //printf("::%s\n",read_inner->d_name);
                                        char raw[1024]=".neogit/stage/";
                                        strcat(raw,read_inner->d_name);
                                        FILE* f=fopen(raw,"r");
                                        if(f==NULL)
                                            staged=0;
                                        else{
                                            char raw2[1024];
                                            strcpy(raw2,read_current_dir->d_name);
                                            strcat(raw2,"/");
                                            strcat(raw2,read_inner->d_name);
                                            if(modified(raw,raw2))
                                                staged=0;
                                        }

                                    }
                                    if(staged==0) printf("name of file: %s --> status: unstaged\n\n",read_inner->d_name);
                                    if(staged==1) printf("name of file: %s --> status: staged\n\n",read_inner->d_name);
                                }
                            }
                        }
                        else if(read_current_dir->d_type==8){
                            char raw[1024]=".neogit/stage/";
                            strcat(raw,read_current_dir->d_name);
                            FILE* f=fopen(raw,"r");
                            if(f==NULL)
                                staged=0;
                            else{
                                char raw2[1024];
                                strcpy(raw2,read_current_dir->d_name);
                                if(modified(raw,raw2))
                                staged=0;
                                }
                            if(staged==0) printf("name of file: %s --> status: unstaged\n\n",read_current_dir->d_name);
                            if(staged==1) printf("name of file: %s --> status: staged\n\n",read_current_dir->d_name);
                            
                         }
                    }
                }
            }


        }
    }
    else if(strcmp(argvtmp[1],"reset")==0){
        if(argctmp!=3) invalid;
        else{
            if(strcmp(argvtmp[2],"-undo")==0){
                FILE* f=fopen(".neogit/lastadd.txt","r");
                char line[MAX_LINE_LENGTH];
                while((fgets(line,MAX_LINE_LENGTH,f))!=NULL){
                    char filename[100];
                    line[strlen(line)-1]='\0';
                    strcpy(filename,line);
                    char del[1024]=".neogit/stage/";
                    strcat(del,filename);
                    remove(del);
                    printf("unstaged file \"%s\"!\n",line);
                }
                fclose(f);
            }
            else{
                if(isDirectory(argvtmp[2])){
                    struct dirent* ent;
                    DIR* dir=opendir(argvtmp[2]);
                    while((ent=readdir(dir))!=NULL){
                        if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                          char del[1024]=".neogit/stage/";
                          strcat(del,ent->d_name);
                          remove(del);  
                        }
                    }
                    printf("unstaged directory \"%s\"!\n",argvtmp[2]);
                }
                else if(is_regular_file(argvtmp[2])){
                    char del[1024]=".neogit/stage/";
                    strcat(del,argvtmp[2]);
                    remove(del);
                    printf("unstaged file \"%s\"!\n",argvtmp[2]); 
                }
            }
        }
    }
    else if(strcmp(argvtmp[1],"set")==0 && strcmp(argvtmp[2],"-m")==0 && strcmp(argvtmp[4],"-s")==0 && strcmp(argvtmp[5],"shortcut-name")==0){
        if(strlen(argvtmp[3])>72){
            printf("your message shouln't have more than 72 charachters!\n");
        }
        else{
            FILE* shortcut=fopen(".neogit/shortcut","r");
            if(shortcut==NULL){
                FILE* sh=fopen(".neogit/shortcut","w");
                fprintf(sh,argvtmp[3]);
                fclose(sh);
            }
            else{
                fclose(shortcut);
                FILE* sh=fopen(".neogit/shortcut","w");
                fprintf(sh,argvtmp[3]);
                fclose(sh);
            }
            printf("set the shortcut message as \"%s\".\n",argvtmp[3]);
        }
        
    }
    else if(strcmp(argvtmp[1],"replace")==0 && strcmp(argvtmp[2],"-m")==0 && strcmp(argvtmp[4],"-s")==0 && strcmp(argvtmp[5],"shortcut-name")==0){
        if(strlen(argvtmp[3])>72){
            printf("your message shouln't have more than 72 charachters!\n");
        }
        else{
            FILE* shortcut=fopen(".neogit/shortcut","r");
            if(shortcut==NULL){
                printf("there is no shortcut to replace!\n");
            }
            else{
                fclose(shortcut);
                FILE* sh=fopen(".neogit/shortcut","w");
                fprintf(sh,argvtmp[3]);
                fclose(sh);
            }
            printf("changed the shortcut message as \"%s\".\n",argvtmp[3]);
        }
    }
    else if(strcmp(argvtmp[1],"remove")==0 && stcmp(argvtmp[2],"-s")==0 && strcmp(argvtmp[3],"shortcut-name")==0){
        FILE* shortcut=fopen(".neogit/shortcut","r");
            if(shortcut==NULL){
                printf("there is no shortcut to remove!\n");
            }
            else{
                remove(".neogit/shortcut");
            }
            fclose(shortcut);
            printf("changed the shortcut message as \"%s\".\n",argvtmp[3]);
    }
    else if(strcmp(argvtmp[1],"commit")==0){
        char message[72];
        int commit_ID;
        char branch[30];
        char username[50];
        char tmp[20];
        int n=0;
        int user;
        if(strcmp(argvtmp[2],"-s")==0 && strcmp(argvtmp[3],"shortcut-name")==0){

        }
        else{
            if(argctmp<4 || strcmp(argvtmp[2],"-m")!=0 ){
                printf("you have to enter a message to make a commit!\n");
            }
            else if(strlen(argvtmp[3])>72){
                printf("your message shouln't have more than 72 charachters!\n");
            }
            else{
                strcpy(message,argvtmp[3]);
                FILE* commitcount=fopen(".neogit/commitcount.txt","r");
                FILE* commitcounttmp=fopen(".neogit/commitcounttmp.txt","w");
                char line[MAX_LINE_LENGTH];
                fgets(line,MAX_LINE_LENGTH,commitcount);
                sscanf(line,"%d",&commit_ID);
                printf(commitcounttmp,"%d",commit_ID++);
                remove(".neogit/commitcount.txt");
                rename(".neogit/commitcounttmp.txt",".neogit/commitcount.txt");
                fclose(commitcount);
                fclose(commitcounttmp);
                FILE* branchf=fopen(".neogit/branch.txt","r");
                fgets(line,MAX_LINE_LENGTH,branchf);
                strcpy(branch,line);
                fclose(branchf);
                FILE* config=fopen(".neogit/config.txt","r");
                fgets(line,MAX_LINE_LENGTH,config);
                fgets(line,MAX_LINE_LENGTH,config);
                sscanf(line,"%s %d %s",tmp,&user,username);
                if(user==0){
                    FILE* globalconfig=fopen(globalpath,"r");
                    fgets(line,MAX_LINE_LENGTH,config);
                    fgets(line,MAX_LINE_LENGTH,config);
                    sscanf(line,"%s %d %s",tmp,&user,username);
                    fclose(globalconfig);
                }
                fclose(config);
                DIR* stage=opendir(".neogit/stage");
                struct dirent* ent;
                while((ent=readdir(stage))!=NULL){
                    if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                        char filename[50];
                        strcpy(filename,ent->d_name);
                        char filepath[1024]=".neogit/commit/";
                        strcat(filepath,branch);
                        strcat(filepath,"/");
                        strcat(filepath,ent->d_name);
                        DIR* commitfolder=opendir(filepath);
                        if(commitfolder==NULL)
                            mkdir(filepath,0755);
                        strcat(filepath,"/");
                        strcat(filepath,commit_ID);
                        FILE* new_commit=fopen(filepath,"w");
                        char stagedfilepath[1024]=".neogit/stage/";
                        strcat(stagedfilepath,ent->d_name);
                        FILE* stagedfile=fopen(filepath,"r");
                        char line[MAX_LINE_LENGTH];
                        while((fgets(line,MAX_LINE_LENGTH,stagedfile))!=NULL)
                            fputs(line,new_commit);
                        remove(stagedfilepath);
                        n++;
                        fclose(stagedfile);
                        closedir(commitfolder);
                    }
                }
                char timedate[1000];
                time_t t = time(NULL);                          ////time here
                struct tm tm = *localtime(&t);
                sprintf(timedate," %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min+7, tm.tm_sec);  
                FILE* log=fopen(".neogit/log.txt","r");
                FILE* tmplog=fopen(".neogit/logtmp.txt","w");
                fprintf(tmplog,"#commit ID: %d\n time:%s\n message: %s\n author: %s\n branch: %s\n number of files commited: %d\n",commit_ID,timedate,message,username,branch,n);
                printf("#commit ID: %d\n time:%s\n message: %s\n author: %s\n branch: %s\n number of files commited: %d\n",commit_ID,timedate,message,username,branch,n);
                char line[MAX_LINE_LENGTH];
                while((fgets(line,MAX_LINE_LENGTH,log))!=NULL)
                    fputs(line,tmplog);
                remove(log);
                rename(".neogit/logtmp.txt",".neogit/log.txt");
                fclose(log);
                fclose(tmplog);
            }
        }
    }
    else if(strcmp(argvtmp[1],"log")==0){
        if(argctmp==2){
            FILE* log=fopen(".neogit/log.txt","r");
            char line[MAX_LINE_LENGTH];
            while((fgets(line,MAX_LINE_LENGTH,log))!=NULL)
                printf("%s",line);
        }
        else if(strcmp(argvtmp[2],"-n")==0 && argctmp==4){
            int n;
            int n=atoi(argvtmp[3]);
            int i=0;
            FILE* log=fopen(".neogit/log.txt","r");
            char line[MAX_LINE_LENGTH];
            while(1){
                char ch;
                fgets(line,MAX_LINE_LENGTH,log);
                sscanf(line,"%c",&ch);
                if(ch=='#') i++;
                if(i>n) break;
                printf("%s",line);
            }
        }
        else if(strcmp(argvtmp[2],"branch")==0){
            int exist=0;
            DIR* dir=opendir(".neogit/commit");
            struct dirent* ent;
            while((ent=readdir(dir))!=NULL)
                if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0)
                    if(strcmp(ent->d_name,argvtmp[3])==0){
                        exist=1;
                        break;
                    }
            closedir(dir);
            if(argctmp!=4){
                printf("you forgot to type the branch name!\n");
            }
            else if(exist==0){
                printf("branch \"%s\" doesn't exist!\n",argvtmp[3]);
            }
            else{
                char branch[20];
                strcpy(branch,argvtmp[3]);
                FILE* log=fopen(".neogit/log.txt","r");
                char line1[MAX_LINE_LENGTH];
                while((fgets(line1,MAX_LINE_LENGTH,log))!=NULL){
                   char line2[MAX_LINE_LENGTH]; 
                   char line3[MAX_LINE_LENGTH];
                   char line4[MAX_LINE_LENGTH];
                   char line5[MAX_LINE_LENGTH];
                   char line6[MAX_LINE_LENGTH];
                   char line7[MAX_LINE_LENGTH];
                   fgets(line2,MAX_LINE_LENGTH,log);
                   fgets(line3,MAX_LINE_LENGTH,log);
                   fgets(line4,MAX_LINE_LENGTH,log);
                   fgets(line5,MAX_LINE_LENGTH,log);
                   fgets(line6,MAX_LINE_LENGTH,log);
                   fgets(line7,MAX_LINE_LENGTH,log);
                   char chert[10];
                   char branchname[10];
                   sscanf(line5," %s %s\n",chert,branchname);
                   if(strcmp(branchname,branch)==0){
                    puts(line1);
                    puts(line2);
                    puts(line3);
                    puts(line4);
                    puts(line5);
                    puts(line6);
                    puts(line7);
                   }
                }
                fclose(log);
            }
        }
        else if(strcmp(argvtmp[]))
    }
}


    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>       CONFIG CAMMANDS BELOW       >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   
    /*if(strcmp(argvtmp[1],"add")==0){
        printf("hastam");
        if(argctmp<3){
            invalid;
        }
        FILE* tmp=fopen(".neogit/lastadd.txt","w");
        int wildcard=0;
        if(strchr(argvtmp[2],'*')!=NULL)
        {
            char *wildcard_broken[10];
            for(int i=0;i<10;i++)
                wildcard_broken[i]=malloc(10*sizeof(char));
            int wildcard_pieces=0;
            char * pch=strtok(argvtmp[2],"*\0");
            while(pch){
                pch=wildcard_broken[wildcard_pieces++];
                pch=strtok(NULL,"*\0");
            }
            struct dirent* ent;
            DIR* dir=opendir(".");
            while((ent=readdir(dir))!=NULL){
                if(ent->d_type==8){
                    char *name=malloc(1024*sizeof(char));
                    strcpy(name,ent->d_name);
                    int to_add=1;
                    for(int i=0;i<wildcard_pieces;i++)
                        if((name=strstr(name,wildcard_broken[i]))==NULL){
                            to_add=0;
                            break;
                        }
                    if(to_add){
                        strcpy(argvtmp[2],ent->d_name);
                        run_add(argvtmp);  
                    }   
                    
                }
                else if(ent->d_type==4){
                    char* name=malloc(1024*sizeof(char));
                    strcpy(name,ent->d_name);
                    int to_add=1;
                    for(int i=0;i<wildcard_pieces;i++)
                        if((name=strstr(name,wildcard_broken[i]))==NULL){
                            to_add=0;
                            break;
                        }
                    if(to_add){
                        strcpy(argvtmp[2],ent->d_name);
                        run_add(argvtmp[2],);
                    }

                }
                closedir(dir);
            }
        }
        if(strcmp(argvtmp[3],"-f")==0){
            for(int i=3;i<argctmp;i++){
                strcpy(argvtmp[2],argvtmp[i]);
                run_add(argvtmp);
            }
        }
        else if(strcmp("-n",argvtmp[2])==0){
            struct dirent* ent;
            DIR* dir=opendir(".");
            while((ent=readdir(dir))!=NULL){
                if(ent->d_type==4 && strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                    int staged=1;
                    struct dirent* new_entry;
                    DIR* mydir=opendir(ent->d_name);
                    while((new_entry=readdir(mydir))!=NULL){
                        if(tracked(new_entry->d_name)==0){
                            staged=0;
                            break;
                        }
                    }
                    closedir(mydir);
                    if(staged==0)
                        printf("%s -> status: not staged\n",ent->d_name);
                    else if(staged==1)
                        printf("%s -> status: staged\n",ent->d_name);
                }
                else if(ent->d_type==8){
                    if(tracked(ent->d_name))
                        printf("%s -> status: staged\n",ent->d_name);
                    else printf("%s -> status: not staged\n",ent->d_name);
                }
                else
                    perror("unknow file type!\n");
            }
        }
        else{
            printf("i4jrie4");
            run_add(argvtmp);
        }
         fclose(tmp);     
        }
        else if(strcmp(argvtmp[1],"reset")==0){                        //emtiazish khoobe
            if(strcmp(argvtmp[2],"-undo")==0){
                FILE* to_read=fopen(".neogit/lastadd.txt","r");
                char line[MAX_LINE_LENGTH];
                while(fgets(line,MAX_LINE_LENGTH,to_read)!=NULL){
                    line[strlen(line)-1]='\0';
                    char path[]=".neogit/stage/";
                    strcat(path,line);
                    remove(path);
                }
                fclose(to_read);
                printf("unstaged the last satged files/directories successfully.\n");
            }
            else{
                struct dirent* ent;
                DIR* dir=opendir(".");
                while((ent=readdir(dir))!=NULL)
                    if(strcmp(ent->d_name,argvtmp[2])==0) break;


                if(ent->d_type==4){
                    struct dirent* dir_ent;
                    DIR* mydir=opendir(ent->d_name);
                    while((dir_ent=readdir(mydir))!=NULL){
                        struct dirent* stage_ent;
                        DIR* di=opendir(".neogit/stage");
                        while((stage_ent=readdir(di))!=NULL)
                            if(strcmp(stage_ent->d_name,dir_ent->d_name)==0){
                                char filepath[]=".neogit/stage/";
                                strcat(filepath,stage_ent->d_name);
                                FILE* file_to_remove=fopen(filepath,"r");
                                if(file_to_remove==NULL)
                                    perror("failed unstaging the directory:(\n");
                                
                                remove(filepath);
                                printf("unstaged the directory successfully.\n");
                            }
                        closedir(di);
                    }
                    closedir(mydir);
                }
                else if(ent->d_type==8){
                    struct dirent* stage_ent;
                    DIR* mydir=opendir(".neogit/stage");
                    while((stage_ent=readdir(mydir))!=NULL)
                            if(strcmp(stage_ent->d_name,ent->d_name)==0){
                                char filepath[]=".neogit/stage/";
                                strcat(filepath,stage_ent->d_name);
                                FILE* file_to_remove=fopen(filepath,"r");
                                if(file_to_remove==NULL)
                                    perror("failed unstaging the file:(\n");
                                remove(filepath);
                                printf("unstaged the file successfully.\n");
                            }
                    closedir(mydir);
                }
                closedir(dir);
            }     
        }
        else if(strcmp("set",argvtmp[1])==0 && strcmp(argvtmp[2],"-m")==0 && strcmp(argvtmp[4],"-s")==0 && strcmp(argvtmp[5],"shortcut-name")==0 && argctmp==6){
            if(strlen(argvtmp[3])>72){
                perror("you exceeded the maximum length for a commit message!\n");
            }
            else{
                strcpy(shortcut,argvtmp[3]);
                shortcut_available=1;
                printf("set the shortcut message successfully!\n");
            }
        }
        else if(strcmp("replace",argvtmp[1])==0 && strcmp(argvtmp[2],"-m")==0 && strcmp(argvtmp[4],"-s")==0 && strcmp(argvtmp[5],"shortcut-name")==0 && argctmp==6){
            if(shortcut_available==0){
                printf("there is no shortcut mesage to replace\n");
            }
            else if(strlen(argvtmp[3])>72){
                perror("you exceeded the maximum length for a commit message!\n");
            }
            else {
                strcpy(shortcut,argvtmp[3]);
                printf("replaced the shortcut message successfully!\n");
            }
        }
        else if(strcmp(argvtmp[1],"remove")==0 && strcmp(argvtmp[2],"-s")==0 && strcmp(argvtmp[3],"shortcut-name")==0 && argctmp==4){
            shortcut_available=0;
            printf("removed the shortcut message successfully!\n");
        }
        else if(strcmp(argvtmp[1],"commit")==0){
           int n=0;
           char message[72];
           if(argctmp!=4 && strcmp(argvtmp[2],"-m")==0){
                perror("you have to enter a message for you commit!\n");
            }
           else if(strcmp(argvtmp[2],"-m")==0 || strcmp(argvtmp[2],"-s")==0){
                if(strlen(argvtmp[3])>72 && argctmp==4 && strcmp(argvtmp[2],"-m")==0){
                    perror("you exceeded the maximum length for a commit message!\n");
                }
                else{
                    if(strcmp(argvtmp[2],"-m")==0)
                        strcpy(message,argvtmp[3]);
                    else if(strcmp(argvtmp[2],"-s")==0)
                        {
                            if(shortcut_available==0) perror("there is no shortcut message available!\n");
                            else strcpy(message,shortcut);
                        }
                    char commitpath[200];
                    strcpy(commitpath,current_config->repopath);
                    sprintf(commitpath,"%s/commit/%s",current_config->repopath,branch);//ta branch pish raftim
                    char stagepath[200];
                    strcpy(stagepath,current_config->repopath);
                    strcat(stagepath,"/stage");
                    DIR* commitdir=opendir(commitpath);
                    DIR* satgedir=opendir(stagepath);
                    struct dirent* stgent;
                    while((stgent=readdir(satgedir))!=NULL){
                        char mystagepath[2024];
                        sprintf(mystagepath,"%s/%s.txt",stagepath,stgent->d_name);
                        FILE* stagef=fopen(mystagepath,"r");//staged file
                        if(tracked_in_commit(stgent->d_name)){
                            char mycommitpath[1024];
                            sprintf(mycommitpath,"%s/%s/%d.txt",commitpath,stgent->d_name,commit_count);
                            //char fpath[1024];
                            FILE* commitf=fopen(mycommitpath,"w");
                            char line[MAX_LINE_LENGTH];
                            while((fgets(line,MAX_LINE_LENGTH,stagef))!=NULL)
                                fputs(line,commitf);
                            remove(mystagepath);
                            fclose(commitf);
                            fclose(stagef);
                        }
                        else{
                            char mypath[2024];
                            sprintf(mypath,"%s/%s",commitpath,stgent->d_name);
                            mkdir(mypath,0755);//created directory for each file
                            char commitstr[100];
                            sprintf(commitstr,"%d",commit_count);
                            strcat(mypath,"/");
                            strcat(mypath,commitstr);
                            //sprintf(mypath,"%s/%hu",mypath,(unsigned short)commit_count);
                            FILE* commitf=fopen(mypath,"w");//this is our new commit
                            char line[MAX_LINE_LENGTH];
                            while((fgets(line,MAX_LINE_LENGTH,stagef))!=NULL)
                                fputs(line,commitf);
                            remove(mystagepath);
                            fclose(commitf);
                            fclose(stagef);
                        }
                        n++;
                    } 
                    char timedate[1000];
                    time_t t = time(NULL);                          ////time here
                    struct tm tm = *localtime(&t);
                    sprintf(timedate," %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min+7, tm.tm_sec);
                    char logpath[1024];
                    sprintf(logpath,"%s/log.txt",current_config->repopath);
                    FILE* log=fopen(logpath,"r");
                    char tmp_logpath[1024];
                    sprintf(tmp_logpath,"%s/tmp_log.txt",current_config->repopath);
                    FILE* tmp=fopen(tmp_logpath,"w");
                    fprintf(tmp,"#commit id: %d\n username: %s\n message: %s\n time:%s\n number of files commited:%d\n author: %s\n branch: %s\n",commit_count,stgent->d_name,message,timedate,n,current_config->username,branch);
                    char line[MAX_LINE_LENGTH];
                    while((fgets(line,MAX_LINE_LENGTH,log))!=NULL)
                        fputs(line,tmp);
                    remove(logpath);
                    rename(tmp_logpath,logpath);
                    fclose(tmp);
                }
           }
           
            
        }
        else if(strcmp(argvtmp[1],"log")==0){
            if(argctmp<2) invalid;
            char path[1024];
            strcpy(path,current_config->repopath);
            strcat(path,"/log.txt");
            FILE* log=fopen(path,"r");
            if(argctmp==2){
                char line[MAX_LINE_LENGTH];
                while((fgets(line,MAX_LINE_LENGTH,log))!=NULL)
                    puts(line);
                fclose(log);
            }
            else if(argctmp==4 && strcmp(argvtmp[2],"-n")==0){
                int n=atoi(argvtmp[3]);
                int i=0;
                do{
                    char line[MAX_LINE_LENGTH];
                    fgets(line,MAX_LINE_LENGTH,log);
                    puts(line);
                    char ch;
                    sscanf(line,"%c",&ch);
                    if(ch=='#') i++;
                }while(i<=n);
            }
            else if(strcmp(argvtmp[2],"-branch")==0){
                if(argctmp!=4) invalid;
                else{
                    char line1[MAX_LINE_LENGTH];
                    while((fgets(line1,MAX_LINE_LENGTH,log))!=NULL){
                       char line2[MAX_LINE_LENGTH]; 
                       char line3[MAX_LINE_LENGTH];
                       char line4[MAX_LINE_LENGTH];
                       char line5[MAX_LINE_LENGTH];
                       char line6[MAX_LINE_LENGTH];
                       char line7[MAX_LINE_LENGTH];
                       char chert[10];
                       char branchname[10];
                       sscanf(line7," %s %s\n",chert,branchname);
                       if(strcmp(branchname,argvtmp[3])==0){
                        puts(line1);
                        puts(line2);
                        puts(line3);
                        puts(line4);
                        puts(line5);
                        puts(line6);
                        puts(line7);
                       }

                    }
                }
            }
            else if(strcmp(argvtmp[2],"author")==0){
                if(argctmp!=4) invalid;
                else{
                    char line1[MAX_LINE_LENGTH];
                    while((fgets(line1,MAX_LINE_LENGTH,log))!=NULL){
                       char line2[MAX_LINE_LENGTH]; 
                       char line3[MAX_LINE_LENGTH];
                       char line4[MAX_LINE_LENGTH];
                       char line5[MAX_LINE_LENGTH];
                       char line6[MAX_LINE_LENGTH];
                       char line7[MAX_LINE_LENGTH];
                       char chert[100];
                       char author[10];
                       sscanf(line6," %s %s\n",chert,author);
                       if(strcmp(author,argvtmp[3])==0){
                        puts(line1);
                        puts(line2);
                        puts(line3);
                        puts(line4);
                        puts(line5);
                        puts(line6);
                        puts(line7);
                       }
                    }
                }
            }
            else if(strcmp(argvtmp[2],"-search")==0){
                if(argctmp!=4) invalid;
                else{
                    char line1[MAX_LINE_LENGTH];
                    while((fgets(line1,MAX_LINE_LENGTH,log))!=NULL){
                       char line2[MAX_LINE_LENGTH]; 
                       char line3[MAX_LINE_LENGTH];
                       char line4[MAX_LINE_LENGTH];
                       char line5[MAX_LINE_LENGTH];
                       char line6[MAX_LINE_LENGTH];
                       char line7[MAX_LINE_LENGTH];
                       char chert[100];
                       char mes[72];
                       sscanf(line3," %s %s\n",chert,mes);
                       if(strstr(mes,argvtmp[3])!=NULL){
                        puts(line1);
                        puts(line2);
                        puts(line3);
                        puts(line4);
                        puts(line5);
                        puts(line6);
                        puts(line7);
                       }
                    }
                }
            }
        }
}
            /*else if(strcmp(argvtmp[2],"-since")==0){
                int n=0;
                char line1[MAX_LINE_LENGTH];
                while((fgets(line1,MAX_LINE_LENGTH,log))!=NULL){
                char line2[MAX_LINE_LENGTH]; 
                char line3[MAX_LINE_LENGTH];
                char line4[MAX_LINE_LENGTH];
                char line5[MAX_LINE_LENGTH];
                char line6[MAX_LINE_LENGTH];
                char line7[MAX_LINE_LENGTH];
                char chert[30];
                char date[30];
                sscanf(line3," %s %s\n",chert,date);
                int year;
                int month;
                int day;
                int hour;
                int min;
                int sec;
                sscanf(date,"%d-%d-%d %d:%d:%d",&year,&month,&day,&hour,&min,&sec);
                int yearidx;
                int monthidx;
                int dayidx;
                int houridx;
                int minidx;
                int secidx;
                sscanf(argvtmp[3],"%d-%d-%d %d:%d:%d",&yearidx,&monthidx,&dayidx,&houridx,&minidx,&secidx);
                if()
                
                }
        //}
        /*else if(strcmp(argvtmp[1],"status")==0){
            struct dirent* ent;
            struct dirent* stgent;
            DIR* dir=opendir(".");
            char path[1024];
            strcpy(path,current_config->repopath);
            strcat(path,"/stage");
            DIR* stagedir=opendir(path);
            while((ent=readdir(dir))!=NULL){
                while((stgent=readdir(stagedir))!=NULL){
                    if(strcmp(ent->d_name,stgent->d_name)==0){
                        printf("%s +")
                    }
                }
            }
            
        }
        else if(strcmp(argvtmp[1],"commit")==0){

        }
        
    }
    time_t t = time(NULL);                          ////time here
    struct tm tm = *localtime(&t);
    printf("now: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min+7, tm.tm_sec);
*/
    //else if(strcmp)
    //FILE* file=fopen("/mnt/c/c_test/man","w");
    // char cwd[1024];
    // getcwd(cwd,1024);
    // printf("%d\n",is_regular_file(".neogit"));
    // printf("%d\n",isDirectory(".neogit"));
    
    // printf("%s\n",cwd);
    //chert();
    //return 0;

