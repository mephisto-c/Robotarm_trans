scom=serial('com5'); 
scom.BaudRate = 115200;
scom.Terminator='CR';%������ֹ��ΪCR���س�����
fopen(scom);
fwrite(scom,255);%д���ڣ����������ź�0xFF(�ȼ���ʮ�����µ���ֵ255)
%fclose(scom);%�رմ����豸����
%delete(scom);%ɾ���ڴ��еĴ����豸����
%clear scom; %��������ռ��еĴ����豸����