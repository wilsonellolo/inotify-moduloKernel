
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/hugetlb.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/mmzone.h>
//#include <linux/quicklist.h>
#include <linux/swap.h>
#include <linux/vmstat.h>
#include <linux/atomic.h>
#include <asm/page.h>
#include <asm/pgtable.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched/signal.h>//candidato
#include <linux/sched.h>

 
struct task_struct *task;      
struct task_struct *task_child;
struct list_head *list;        
 
#define PROCFS_NAME "cpu201314158"

char* get_state_name(long state){
	char* string="";
	switch(state){
		case 0:
        string="TASK_RUNNING";
        break;
        case 1:
        string="TASK_INTERRUPTIBLE";
        break;
        case 2:
        string="TASK_UNINTERRUPTIBLE";
        break;
        case 4:
        string="__TASK_STOPPED";
        break; 
        case 8:
        string="__TASK_TRACED";
        break;
        case 16:
        string="EXIT_DEAD";
        break;
        case 32:
        string="EXIT_ZOMBIE";
        break;
        case 64:
        string="TASK_PARKED";
        break;
         case 128:
        string="TASK_DEAD";
        break;
         case 256:
        string="TASK_WAKEKILL";
        break;
         case 512:
        string="TASK_WAKING";
        break;
        case 1024:
        string="TASK_NOLOAD";
        break;
        case 2048:
        string="TASK_NEW";
        break;
        case 4096:
        string="TASK_STATE_MAX";
        break;
        default:
        	string=""; 
        	break; 
	}
	return string;
}     

//Escribiendo a los hijos
void escribeHijos(struct seq_file *m, struct task_struct *task)
{   
    struct task_struct *child_tasks; 
    struct list_head *list;  

    list_for_each(list, &task->children) {
        child_tasks = list_entry(list, struct task_struct, sibling);
        seq_printf(m,"\n<ul>");					
	for_each_process( child_tasks ){   
        	//seq_printf(m,"\nid: %d, Nombre: %s",task->pid,task->comm);			    	
		seq_printf(m,"\n<li><a>id: %d, Nombre: %s</a></li>",child_tasks->pid,child_tasks->comm);
		//escribeHijos(m,child_tasks);		
	}   
	seq_printf(m,"\n</ul>\n");
    }
}


int iterate_init(struct seq_file *m, void *v)       
{
     	int numeroDeProcesos=0;
	int procesosRunning=0;
	int procesosSleeping=0;
	int procesosStoped=0;
	int procesosZombie=0;
	int utilizacionDelCpu=0;
	
    for_each_process( task ){   
        numeroDeProcesos++;
	utilizacionDelCpu+=task->cpu;
	
	switch(task->state){
	case 0:
        	procesosRunning++;
        	break;
        case 4:
        	procesosStoped++;
        	break; 
        case 32:
        	procesosZombie++;
        	break;
	case 2:
        	procesosSleeping++;
        default:
        	break; 
	}
	
    }
	seq_printf(m,"\n<h1>Numero de procesos: %d </h1><hr>",numeroDeProcesos);
	seq_printf(m,"\n<h3>Numero de procesos Running: %d</h3><hr>",procesosRunning);
	seq_printf(m,"\n<h3>Numero de procesos Stoped: %d</h3><hr>",procesosStoped);
	seq_printf(m,"\n<h3>Numero de procesos Zombie: %d</h3><hr>",procesosZombie);
	seq_printf(m,"\n<h3>Numero de procesos Sleeping: %d</h3><hr>",procesosSleeping);
	seq_printf(m,"\n<h2>task->Cpu: </h2><h3> %d  </h3><hr>",(utilizacionDelCpu));


	seq_printf(m,"<div id=\"header\">\n");
    for_each_process( task ){   

	seq_printf(m,"\n<ul class=\"nav\"><li><a><form  method=\"POST\">"); 
	seq_printf(m,"<input name=\"pid\" type=\"text\" value=\"%d\" style=\"background-color: #000; border-color: #000; color: #fff;\">" 
	"<a>Nombre: %s,&nbsp&nbsp Usuario: %u,&nbsp&nbsp Estado id: %ld,&nbsp&nbsp Nombre estado: %s ,&nbsp&nbsp Porcentaje de Ram:</a>"
	"</form></a>"
	,task->pid,task->comm,from_kuid(&init_user_ns, task_uid(task)),task->state,get_state_name(task->state));	
	escribeHijos(m,task);
	seq_printf(m,"</li></ul><hr>\n"); 	
	}   
	
	seq_printf(m,"</div>\n"); 
 
    return 0;
}                


     
static int OS2_open(struct inode *inode, struct file *file){
return single_open(file, iterate_init, NULL);
}

static const struct proc_ops OS2_fops = {

.proc_open = OS2_open,
.proc_read = seq_read,
.proc_lseek = seq_lseek,
.proc_release = single_release,
};

static int __init OS2_init(void){
printk(KERN_INFO "Wilson Palma\r\n");
proc_create(PROCFS_NAME, 0, NULL, &OS2_fops);
return 0;
}
      

static void __exit OS2_exit(void){
remove_proc_entry(PROCFS_NAME, NULL);
printk(KERN_INFO "Wilson Palma\r\n");
}      
 
module_init(OS2_init);   
module_exit(OS2_exit);   
 
MODULE_LICENSE("GPL");

