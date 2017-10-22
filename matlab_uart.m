scom=serial('com5'); 
scom.BaudRate = 115200;
scom.Terminator='CR';%设置终止符为CR（回车符）
fopen(scom);
fwrite(scom,255);%写串口，发送握手信号0xFF(等价于十进制下的数值255)
%fclose(scom);%关闭串口设备对象
%delete(scom);%删除内存中的串口设备对象
%clear scom; %清除工作空间中的串口设备对象