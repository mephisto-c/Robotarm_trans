function [rou,theta] = cootran(x,y)
rou = sqrt(x*x+y*y);
theta = atan(x/y);
