//���̼�ͨ��-��Ϣ����
//�ͻ��� ����Ϣ���� ��ȡ����
#include "unp.h"

int main(int argc, char **argv)
{
    //printf("IPC_PRIVATE:%d\n", IPC_PRIVATE); //����IPC_PRIVATE 0
    if (argc != 3)
    {
        printf("usage:%s <file> <arg>\n", argv[0]);
        exit(0);
    }
    key_t   key;
    key = ftok(argv[1], atoi(argv[2]));//��ͬ��·�����ͱ�����ɵ�keyһ��
    printf("create key:%d\n", key);
    
    int msgid;
    if ((msgid = msgget(key, 0)) == -1) //��ȡ��Ϣ���б�ʶ��
    {
        printf("msgget error:%s\n", strerror(errno));//��Ϣ���в����� ������
        exit(0);
    }
    printf("open msgid:%d\n", msgid);
    msgbuf msg;
    long type;
    
    while (1)
    {
        printf("input msg type:");
        scanf("%d", &type);
        
        if (msgrcv(msgid, &msg, MAXLINE, type, 0) == -1)
        {
            printf("msgrcv error:%s\n", strerror(errno));
            break;
        }
        printf("get msg,msg type:%d, buf:%s\n", msg.type, msg.buf);
    }
    
    exit(0);
}