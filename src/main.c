#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#define invalid printf("Invalid command!")
#define MAX_GIT_PROJECTS 200
#define MAX_LENGTH 40
//#define _DEBUG_
typedef struct config_{
DIR* cwd;
char username[MAX_LENGTH];
char email[MAX_LENGTH];
char alias[10][10];
char alias_equiv[10][10];
int total_alias;
}config;
int is_regular_file(const char *path){
            struct stat path_stat;
            stat(path, &path_stat);
            return S_ISREG(path_stat.st_mode);
            }
// void set_globally(config* conf , char *const argv[] , config* repo_list[]){
    
    // strcpy(conf->username,argv[4]);
    // for(int i=0;i<total_repos;i++)
    //     strcpy(repo_list[i]->username,argv[4]);
// }
#ifdef _DEBUG_
int main()
{
    int argc=2;
    char const *argv[]={"neogit","init"};
#else
int main(int argc,char const *argv[]){
#endif
    // for(int i=0;i<argc;i++){
    //     printf("%s\n",argv[i]);
    // }
    config* global_config;
    config* repo_list[MAX_GIT_PROJECTS];
    int global_email=0;
    int global_us=0;
    int global_alias=0;
    int total_repos=0;
    config* current_config;
    char *argvtmp[10];
    for(int i=0;i<10;i++)
        argvtmp[i]=malloc(10);
    int argctmp=argc;
    for(int i=0;i<argc;i++){
        strcpy(argvtmp[i],argv[i]);
    }
    for(int i=0;i<argctmp;i++){
        printf("%s\n",argvtmp[i]);
    }/*
    //char const chert[2][2]={"salam","chi"};
    //printf("%d",main(2,chert[2]));
    if(argctmp<2){
        invalid;
        return 1;
    }
    if(strcmp(argvtmp[0],"neogit")!=0){
        invalid;
        return 1;
    }
    //                                  //check if a repository exists?
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;

    char tmp_cwd[1024];
    int exists = 0;
    struct dirent *entry;
    do {
        // find .neogit
        DIR *dir = opendir(".");
        if (dir == NULL) {
            perror("Error opening current directory");
            return 1;
        }
        while ((entry = readdir(dir)) != NULL) {
            if ((entry->d_type) == DT_DIR && strcmp(entry->d_name, ".neogit") == 0)
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
    //printf("%d",exists);
    // return to the initial cwd
    if (chdir(cwd) != 0) return 1;
    if (exists==0 && strcmp(argvtmp[1],"init")!=0){
        printf("neogit repository has not been initialized yet\n");
        return 1;
    }
    else if(exists==0 && strcmp(argvtmp[1],"init")==0){
        mkdir(".neogit",0755);
        //chdir(".neogit");
        total_repos++;
        repo_list[total_repos-1]->cwd=opendir(".neogit");
        current_config->cwd=repo_list[total_repos-1]->cwd;
        if(global_us){
            strcpy(repo_list[total_repos-1],global_config->username);
            strcpy(current_config->username,global_config->username);
        }
        else
            printf("enter your username:\ngit config (--global) user.name \"your username\"\n");
        if(global_email){
            strcpy(repo_list[total_repos-1],global_config->email);
            strcpy(current_config->email,global_config->email);
        }
        else
            printf("enter your email:\ngit config (--global) user.email \"your email@gmail.com\"\n");
        if(global_alias)
            for(int i=0;i<global_config->total_alias;i++){
                strcpy(repo_list[total_repos-1]->alias,global_config->alias[i]);
                strcpy(repo_list[total_repos-1]->alias_equiv,global_config->alias_equiv[i]);
                strcpy(current_config->alias,global_config->alias[i]);
                strcpy(current_config->alias_equiv,global_config->alias_equiv[i]);
                repo_list[total_repos-1]->total_alias++;
            }
        //getcwd(repo_list[total_repos]->cwd,1024);
        mkdir("stage",0755);
        mkdir("commit",0755);
        mkdir("log",0755);
    }
    else if(exists==1 && strcmp(argvtmp[1],"init")==0)
        printf("neogit repository already exists!");
    //check for alias commands
    for(int i=0;i<current_config->total_alias;i++){
        if(strcmp(current_config->alias[i],argvtmp[1])==0){
            char *tmp=strtok(current_config->alias_equiv[i]," \0");
            argctmp--;
            while(tmp){
                strcpy(argvtmp[argctmp++],tmp);
                tmp=strtok(NULL," \0");
            }
        }
    }//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>       CONFIG CAMMANDS BELOW       >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    if(strcmp(argvtmp[1],"config")==0){                     //GLOBAL??????????
        if(strcmp(argvtmp[2],"--global")==0){
            if(strcmp(argvtmp[3],"user.name")==0){
                global_us=1;
                strcpy(global_config->username,argvtmp[4]);
                for(int i=0;i<total_repos;i++)
                    strcpy(repo_list[i]->username,argvtmp[4]);
            }
            else if(strcmp(argvtmp[3],"user.email")==0){
                global_email=1;
                strcpy(global_config->email,argv[4]);
                for(int i=0;i<total_repos;i++)
                    strcpy(repo_list[i]->email,argvtmp[4]);
            }
            else if(strncmp(argvtmp[3],"alias.",6)==0){
                global_alias=1;
                strcpy((global_config->alias[0]),(argvtmp[3]+6));
                for(int i=0;i<total_repos;i++){
                    strcpy(repo_list[i]->alias[0],argvtmp[2]+6);
                    strcpy(repo_list[i]->alias_equiv[0],argvtmp[3]);
                    repo_list[i]->total_alias++;
                }
            }
        }                                                    //OR LOCAL??????????
        else{
            if(strcmp(argvtmp[2],"user.name")==0)
                    strcpy(current_config->username,argvtmp[3]);
            else if(strcmp(argvtmp[2],"user.email")==0)
                    strcpy(current_config->email,argvtmp[3]);
            else if(strncmp(argvtmp[2],"alias.",6)==0){
                //if(valid(argvtmp[3]))                     //check for invalidity
                strcpy(current_config->alias[current_config->total_alias],argvtmp[2]+6);
                strcpy(current_config->alias_equiv[current_config->total_alias],argvtmp[3]);
                current_config->total_alias++;
            } 
        }
    }
    if(strcmp(argvtmp[1],"add")==0){
        if(argctmp<3){
            invalid;
            return 1;
        }
        else{
            int is_regular_file(const char *path){
            struct stat path_stat;
            stat(path, &path_stat);
            return S_ISREG(path_stat.st_mode);
            }
        }
    }*/
    char cwd[1024];
    printf("%d",is_regular_file(getcwd(cwd,1024)));

    //chert();
    return 0;
}
