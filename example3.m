%��һ��������ģ��  
%6����ת�ؽ�  
%xmfbit  
clear L1 L2 L3 L4 L5 L6 R qz qr t q qdd qd  
close all  
clc  
  
  
L1=Link([0,0,20,0,0]);  
L2=Link([0,0,20,0,0]);  
L3=Link([0,0,20,pi/2,0]);  
L4=Link([0,0,20,0,0]);  
L5=Link([0,0,20,0,0]);  
L6=Link([0,0,20,0,0]);  
disp('ģ�͹������,�������£�');  
R=SerialLink([L1,L2,L3,L4,L5,L6])  
qz=[0,0,0,0,0,0];           %��ʼλ�ùؽڵ����  
  
  
%����е�۳�ʼ״̬��ͼ  
figure  
R.plot(qz)                         
title('��е�۵ĳ�ʼλ��');  
  
  
  
  
%�켣���  
qr=[0,pi/2,0,pi/2,0,0];       %����λ��6���ؽڵ�Ĳ���  
t=[0:0.5:10]';  
disp('ת�������еĹؽڵ������');  
[q,qd,qdd]=jtraj(qz,qr,t);  
disp('λ�Ʋ�����');  
q  
disp('�ٶȲ�����');  
qd  
disp('���ٶȲ�����');  
qdd  
  
  
%����̬ͼ  
figure  
R.plot(q)  
title('��е���˶���̬ͼ');  
  
  
%�Ե��ĸ��ؽڵ�Ϊ����λ�ƣ��ٶȣ����ٶȲ����仯ͼ  
figure  
%��ȡ��������У���Ϊ���ĸ��ؽڵ��λ�ƣ��ٶȣ����ٶȲ���  
q4=q(:,4);  
qd4=qd(:,4);  
qdd4=qdd(:,4);  
subplot(2,2,1)  
plot(t,q4)  
title('λ�Ʊ仯');  
subplot(2,2,2)  
plot(t,qd4)  
title('�ٶȱ仯');  
subplot(2,2,3)  
plot(t,qdd4)  
title('���ٶȱ仯');  