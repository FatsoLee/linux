#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/pid.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");

#define BUF_SIZE	4096

static char global_buffer[BUF_SIZE];
static struct proc_dir_entry *process_dir,*pid_file,*family_file;
static int tpid;

module_param(tpid, int, S_IRUGO);

static int exam_show(struct seq_file *p, void *v)
{
	struct list_head* temp_ptr;
	struct list_head* temp_bro_ptr;
	struct task_struct *p_for_bro;
	struct task_struct *p_for_ch;

	struct task_struct *task_ptr=find_task_by_pid(tpid);
 	seq_printf(p,"\nTask father info :\n\t Father PID =  %d  is executing program %s \n",task_ptr->parent->pid,task_ptr->parent->comm);
	
	list_for_each(temp_ptr,&task_ptr->children){
  		p_for_ch=list_entry(temp_ptr,struct task_struct,sibling);
  		seq_printf(p,"Task children info :\n\t Children PID = %d is executing program %s \n",p_for_ch->pid,p_for_ch->comm);
 	}

	list_for_each(temp_bro_ptr,&task_ptr->sibling){
  		p_for_bro=list_entry(temp_bro_ptr,struct task_struct,sibling);
  		seq_printf(p,"Task Brother info :\n\t Brother PID = %d is executing program %s \n",p_for_bro->pid,p_for_bro->comm);
 	}
	return 0;
 }

static int exam_single_open(struct inode *inode, struct file *file)
{
        return(single_open(file, exam_show, NULL));
}

static struct file_operations exam_single_seq_file_operations = {
        .open = exam_single_open,
        .read = seq_read,
        .llseek = seq_lseek,
        .release = single_release,
};

static int tranttoi(const char* s)
{
	int n = 0;
   while(*s!='\0' && *s >= '0' && *s <= '9')
 	{
     	int c = *s - '0';
    	n = n * 10 + c;
      ++s;
  	}
   return n;
}



static int proc_read_pid(char *page,char **start,off_t off,int count,int *eof,void *data)
{
	//while user read the file named "pid" ,kernel run this function
	int len;
	len=sprintf(page,"%d\n",tpid);
	return len;
}

static int proc_write_pid(struct file *file,const char *buffer,unsigned long count,void *data)
{
	//while user write the file named "pid" ,kernel run this function
	int len;
	if(count>BUF_SIZE-1)
	{
		len=BUF_SIZE-1;
	}
	else
	{
		len=count;
	}
	//get the data from user buffer
	if(copy_from_user(global_buffer,buffer,len))
		return -EFAULT;
	global_buffer[len]='\0';
	//put into pid 
	tpid=tranttoi(global_buffer);
	return len;
}

static int __init  m_pid_init(void)
{
	tpid=3;
	process_dir=proc_mkdir("process_family",NULL);		//create dir /proc/process_family
	process_dir->owner=THIS_MODULE;
	//create read only file /proc/process_family/family
	family_file = create_proc_entry("family", 0, process_dir);
   if (family_file){
   	family_file->proc_fops = &exam_single_seq_file_operations;
	}
	//create read and write file /proc/process_family/pid
	pid_file=create_proc_entry("pid",0644,process_dir);
	pid_file->read_proc=proc_read_pid;
	pid_file->write_proc=proc_write_pid;
	pid_file->owner=THIS_MODULE;

	return 0;
}

static void __exit m_pid_exit(void)
{
	remove_proc_entry("pid",process_dir);
	remove_proc_entry("family",process_dir);
	remove_proc_entry("process_family",NULL);
}

module_init(m_pid_init);
module_exit(m_pid_exit);
