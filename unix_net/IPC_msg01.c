//���̼�ͨ��-��Ϣ����
#include "unp.h"

int main(int argc, char **argv)
{
    printf("IPC_PRIVATE:%d\n", IPC_PRIVATE); //����IPC_PRIVATE 0
    printf("MSGMAX:%d\n", MSGMAX); //һ����Ϣ����󳤶�
    printf("MSGMNB:%d\n", MSGMNB); //һ�����е���󳤶�
    /*
    if (argc != 3)
    {
        printf("usage:%s <file> <arg>\n", argv[0]);
        exit(0);
    }
    key_t   key;
    key = ftok(argv[1], atoi(argv[2]));//��ͬ��·�����ͱ�����ɵ�keyһ��
    printf("create key:%d\n", key);
    
    int msgid;
    
    //if ((msgid = msgget(IPC_PRIVATE, 0)) == -1)//ÿ�ζ�����һ���µ���Ϣ����
    //if ((msgid = msgget(key, IPC_CREAT)) == -1)//��������򷵻�msgid��������������һ���µ���Ϣ����
    //if ((msgid = msgget(key, IPC_CREAT | IPC_EXCL)) == -1)//������������EEXIST��������������һ���µ���Ϣ����
    //if ((msgid = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL)) == -1)//IPC_PRIVATE��������һ���µ���Ϣ����
    if ((msgid = msgget(key, 0)) == -1)//��������򷵻�msgid,�����������
    {
        printf("msgget error:%s\n", strerror(errno));
        exit(0);
    }
    printf("msgid:%d\n", msgid);
    //msgid = 1;
    ///*
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        printf("msgctl error:%s\n", strerror(errno));
        exit(0);
    }
    printf("remove msgid %d\n", msgid);
    //*/
    exit(0);
}