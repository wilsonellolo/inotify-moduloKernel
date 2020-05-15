#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>

#define MAX_EVENT_MONITOR 2048
#define NAME_LEN 32
#define MONITOR_EVENT_SIZE (sizeof(struct inotify_event))
#define BUFFER_LEN MAX_EVENT_MONITOR*(MONITOR_EVENT_SIZE+NAME_LEN)

int main (int argc, char ** argv){
    FILE* archivo_log;
    
    int fd,watch_desc;
    char buffer[BUFFER_LEN];
    fd = inotify_init();
    if(fd<0){
        perror("notify no se inicio");
    }
    watch_desc= inotify_add_watch(fd,"/",IN_CREATE|IN_MODIFY|IN_DELETE);
    if(watch_desc==-1){
        printf("no se pudo agreagar el visor al forder /home/wilson");
    }else{
        printf("Monitoreando /\n");
    }
    int i=0;
    while(1){
        archivo_log = fopen("logDelMonitoreo.txt", "a");
        i=0;
        int total_read=read(fd,buffer,BUFFER_LEN);
        if(total_read<0){
            perror("error de lectura");
        }
        while(i<total_read){
            struct inotify_event *event=(struct inotify_event*)&buffer[i];
            if(event->len){
                if(event->mask & IN_CREATE){
                    if(event->mask & IN_ISDIR){
                        printf("Directorio %s fue creado\n",event->name);
                         fputs("Directorio ", archivo_log);
                         fputs(event->name , archivo_log);
                         fputs(" fue creado \n" , archivo_log);
                    }else{
                        printf("Archivo %s fue creado\n",event->name);
                        fputs("Archivo ", archivo_log);
                        fputs(event->name , archivo_log);
                        fputs(" fue creado \n" , archivo_log);
                    }
                }
                if(event->mask & IN_MODIFY){
                    if(event->mask & IN_ISDIR){
                        printf("Directorio %s fue modificado\n",event->name);
                        fputs("Directorio ", archivo_log);
                        fputs(event->name , archivo_log);
                        fputs(" fue modificado \n" , archivo_log);
                    }else{
                        printf("Archivo %s fue modificado\n",event->name);
                        fputs("Archivo ", archivo_log);
                        fputs(event->name , archivo_log);
                        fputs(" fue modificado \n" , archivo_log);
                    }
                }
                if(event->mask & IN_DELETE){
                    if(event->mask & IN_ISDIR){
                        printf("Directorio %s fue eliminado\n",event->name);
                        fputs("Directorio ", archivo_log);
                        fputs(event->name , archivo_log);
                        fputs(" fue eliminado \n" , archivo_log);
                    }else{
                        printf("Archivo %s fue eliminado\n",event->name);
                        fputs("Archivo ", archivo_log);
                        fputs(event->name , archivo_log);
                        fputs(" fue eliminado \n" , archivo_log);
                    }
                }
                fclose(archivo_log);
                i+=MONITOR_EVENT_SIZE+event->len;
            }
        }
    }
    inotify_rm_watch(fd,watch_desc);
    close(fd);
    
    return (EXIT_SUCCESS);
}
