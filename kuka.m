%kuka youBot   
clear;  
clc;  
close all;  
d1=0.147;  
a1=0.033;  
a2=0.155;  
a3=0.135;  
d5=0.113;  
tool = 0.105;  
  
qlim1=[-169,169]*pi/180;  
qlim2=[-65,90]*pi/180;  
qlim3=[-151,146]*pi/180;  
qlim4=[-102.5,102.5]*pi/180;  
qlim5=[-167.5,167.5]*pi/180;  
  
 L(1) = Link('d', d1, 'a', a1, 'alpha', pi/2, 'qlim',qlim1);  
 L(2) = Link('d', 0, 'a', a2, 'alpha', 0, 'qlim', qlim2, 'offset', 1.57);  
%  L(2) = Link('d', 0, 'a', a2, 'alpha', 0, 'qlim', qlim2);  
 L(3) = Link('d', 0, 'a', a3, 'alpha', 0,'qlim',qlim3);  
 L(4) = Link('d', 0, 'a', 0, 'alpha', -pi/2,'qlim',qlim4, 'offset', -1.57);  
%  L(4) = Link('d', 0, 'a', 0, 'alpha', -pi/2,'qlim',qlim4);  
 L(5) = Link('d', d5, 'a', 0, 'alpha', 0,'qlim',qlim5);  
   
 bot = SerialLink(L,'name','KUKA youBot');  
 bot.display();  
   
%  bot.plot([0 0 0 0 0]);  
%  bot.teach;  
  
 %% forward kinematics.  
 bot.tool= transl(0, 0, tool);% EEF  
   
bot.fkine([0 0 0 0 0]*pi/180);  
  
bot.fkine([0 0 -90 0 0]*pi/180)  
bot.fkine([0 0 90 0 0]*pi/180)  