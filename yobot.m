clc;  
clear;  
close all;  
L(1) = Link([ 0  0.147  0  0 ],  'modified' );  
L(2) = Link([ 0  0  0.033 pi/2  ],  'modified');   
L(3) = Link([ 0 0 0.155  0  ],  'modified');  
L(4) = Link([ 0  0 0.135 0],  'modified');   
L(5) = Link([ 0  0.218 0 -pi/2],  'modified') ;  
  
  
youbot = SerialLink(L, 'name', 'youbot');  
youbot.display()  
youbot.plot([0 0 0 0 0]);  
  
youbot.teach;  