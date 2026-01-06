//add 3x levels
baseRodX=65;
sliderRodX=35;
jointRodX=35;
rodAngleMax=25+0;//...
rodAngle=rodAngleMax*$t-0;;//...
height=1200;
rod1l=700;
rod2l=680;
supportRodLength=270;
jointY0=-supportRodLength+150;
//jointY1=-height+rod1l;
jointY1=380-supportRodLength-rod2l;
jointY=(1*$t*(jointY1-jointY0)+jointY0)/cos(rodAngle);

//tlink?
zJoints=jointY*cos(rodAngle)-jointRodX*sin(rodAngle);
rJoints=baseRodX-jointY*sin(rodAngle)-jointRodX*cos(rodAngle)-sliderRodX;
supportAngle=180-asin(rJoints/supportRodLength)-rodAngle;//180...
supportAngle0=asin(rJoints/supportRodLength)-180;//180...
hJoints=-sqrt(supportRodLength*supportRodLength-(rJoints)*(rJoints));//-

echo("supportAngle",supportAngle,supportAngle0,rodAngle,$t);
//x0=sliderRodX;
////y0=sliderY;//?
//x1=baseRodX+(jointY)*sin(rodAngle);
//y1=jointY*cos(rodAngle);
//
//
////supportRodLength=sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
//y0=y1-sqrt(supportRodLength*supportRodLength-(x1-x0)*(x1-x0));
//sliderY=y0;

//supportAngle=25;

module baseTriangle()
{
//    %cylinder(r=baseRodX,center=true,h=.1,$fn=64);
%color("green",.5) translate([0,0,0]) rotate([0,180,0])  cylinder(d=30,h=700,$fn=16); 
for(i=[0:120:240])
rotate([0,180,i]) 
%color("blue",.5)  translate([baseRodX,0,0]) rotate([0,rodAngle,0])   cylinder(d=25,h=rod1l,$fn=16); 

difference()
{
union()
{
difference()
{
intersection()
{
union()
{
hull() for(i=[0:120:240])
rotate([0,180,i])

hull()
{ 

translate([baseRodX,0,0]) rotate([90,0,0])   cylinder(d=36,h=100,$fn=64,center=true);
translate([baseRodX-7,0,-35]) rotate([90,0,0])   cylinder(d=3,h=100,$fn=64,center=true);
} 
}
translate([0,0,-18]) 

intersection()
{
rotate([0,0,60])  cylinder(d=270,h=47,$fn=3);
rotate([0,0,120])  cylinder(d=342,h=47,$fn=3);    
}
}

for(i=[0:120:240])
rotate([0,180,i])
translate([baseRodX,0,0]) {

rotate([90,0,0]) cylinder(d=8.2,h=200,$fn=32,center=true);
rotate([90,30,0]) translate([0,0,31])  cylinder(d=15,h=50,$fn=6);
rotate([-90,0,0]) translate([0,0,31])  cylinder(d=20,h=50,$fn=32);            


union()
{    
rotate([90,0,0]) 
cylinder(d=36,h=50,$fn=32,center=true);
translate([0,0,25]) cube([35,50,100],center=true);
translate([35/2,0,0]) cube([35,50,100],center=true);    
}

}

translate([0,0,0]) 
intersection()
{
rotate([0,0,0]) cylinder(r=baseRodX+20,h=45,$fn=3,center=true);
rotate([0,0,60]) cylinder(r=baseRodX+60,h=45,$fn=3,center=true);    
}

translate([0,0,0]) rotate([0,0,30]) cylinder(h=99,d=25.5,$fn=64,center=true);
}




hull()
{
translate([0,0,4]) rotate([0,0,30]) cylinder(h=44,d=43,$fn=64,center=true);    
translate([0,0,-43]) rotate([0,0,30]) cylinder(h=50,d=40,$fn=64,center=true);
}

hull()
{
//translate([20,0,-68]) rotate([0,0,30]) cylinder(h=30,d=15,$fn=64);
translate([-20,0,5-65.5]) rotate([90,0,0])  cylinder(h=25,d=15,$fn=64,center=true);
translate([0,0,5-65.5]) rotate([90,0,0]) cylinder(h=25,d=15,$fn=64,center=true);
translate([0,0,100-65.5]) rotate([90,0,0]) cylinder(h=25,d=15,$fn=64,center=true);
translate([-20,0,25-65.5]) rotate([90,0,0])  cylinder(h=25,d=15,$fn=64,center=true);    
}

//hull()
//{
//translate([-24,-10,10-65.5]) rotate([90,0,0])  cylinder(h=5,d1=25,d2=20,$fn=64,center=true);
//translate([-24,10,10-65.5]) rotate([90,0,0])  cylinder(h=5,d2=25,d1=20,$fn=64,center=true);
//}

for(i=[30:60:360])
    rotate([0,0,i])
{
translate([25,0,4]) cube([55,5,44],center=true);
}
}

for(i=[0:120:360])
    rotate([0,0,i])
{
hull()
{    
translate([55,0,0]) cylinder(d=8.5,h=100,center=true,$fn=32);
translate([30,0,0]) cylinder(d=8.5,h=100,center=true,$fn=32);    
}
translate([80,7.5,-10]) rotate([0,90,0]) cylinder(d=4.2,h=100,center=true,$fn=16);
translate([80,-7.5,-10]) rotate([0,90,0]) cylinder(d=4.2,h=100,center=true,$fn=16);
    
translate([80,7.5,10]) rotate([0,90,0]) cylinder(d=4.2,h=100,center=true,$fn=16);
translate([80,-7.5,10]) rotate([0,90,0]) cylinder(d=4.2,h=100,center=true,$fn=16);    
}

for(i=[60:120:360])
    rotate([0,0,i])
{

translate([32,0,0]) cylinder(d=8.5,h=100,center=true,$fn=32);    
}

translate([-20,0,5-65.5]) rotate([90,0,0])  cylinder(h=35,d=5.3,$fn=64,center=true);
translate([-20,0,25-65.5]) rotate([90,0,0])  cylinder(h=35,d=5.3,$fn=64,center=true);
translate([-20,-15.001,5-65.5]) rotate([90,30,0])  cylinder(h=15.001,d=11,$fn=6,center=true);
translate([-20,-15.001,25-65.5]) rotate([90,30,0])  cylinder(h=15.001,d=11,$fn=6,center=true);
translate([-20,20.001,5-65.5]) rotate([90,30,0])  cylinder(h=15.001,d=12,$fn=64,center=true);
translate([-20,20.001,25-65.5]) rotate([90,30,0])  cylinder(h=15.001,d=12,$fn=64,center=true);

translate([0,0,0]) rotate([0,0,30]) cylinder(h=999,d=30.5,$fn=64,center=true);
//translate([0,0,-24]) rotate([0,90,0]) cylinder(h=99,d=12,$fn=64,center=true);
translate([-25,0,-43-1]) cube([65,4,50],center=true);
//translate([19,0,-43-1]) cylinder(h=50,d=4,$fn=64,center=true);
}
}

module centerSlider()
{
//    %cylinder(r=sliderRodX,center=true,h=.1,$fn=64);

difference()
    {
union()
{
hull()
{    
translate([0,0,16]) cylinder(d=38,h=50,$fn=64);

intersection()
{    
translate([0,0,16]) cylinder(d1=70,d2=58,h=20,$fn=3);
rotate([0,0,60]) translate([0,0,16]) cylinder(d=96,h=20,$fn=3);             
}
}

for(i=[60:120:300])
rotate([0,0,i])
{
hull()
{    
translate([23,17.5,15]) cube([28,4,2],center=true);
translate([12,17.5,35]) cube([5,4,2],center=true);     
}
hull()
{    
translate([23,-17.5,15]) cube([28,4,2],center=true);
translate([12,-17.5,35]) cube([5,4,2],center=true);     
}
}

for(i=[0:120:300])
rotate([0,0,i])  translate([7.5,-3.5,15]) cube([15,7,21]);  
    

difference()
    {
union()
    {
intersection()
{    
hull()for(i=[0:120:240])
rotate([0,180,i])
{ 
//translate([0,0,0]) rotate([90,0,0])   cylinder(d=35,h=50,$fn=32,center=true);   
translate([sliderRodX,0,0]) rotate([90,0,0])   cylinder(d=35,h=70,$fn=32,center=true);
}
intersection()
{
translate([0,0,0]) rotate([0,0,-60])   cylinder(d=165,h=32,$fn=3,center=true);
translate([0,0,0]) rotate([0,0,0])   cylinder(d=210,h=32,$fn=3,center=true);    
}
}

}
}

rotate([0,0,0]) hull()
{

translate([25,-7,53.5]) rotate([90,0,0]) cylinder(d1=25,d2=20,h=5,$fn=64,center=true);
translate([0,-7,53.5]) rotate([90,0,0]) cylinder(d1=25,d2=20,h=5,$fn=64,center=true);
translate([25,7,53.5]) rotate([90,0,0]) cylinder(d2=25,d1=20,h=5,$fn=64,center=true);
translate([0,7,53.5]) rotate([90,0,0]) cylinder(d2=25,d1=20,h=5,$fn=64,center=true);    
 
translate([0,0,18]) rotate([90,0,0]) cylinder(d=5,h=5,$fn=64,center=true);    
}
}

rotate([0,0,0]) 
translate([25,-11,53.5]) rotate([90,30,0]) cylinder(d=15,h=10,$fn=6,center=true);
rotate([0,0,0]) 
translate([25,0,53.5]) rotate([90,30,0]) cylinder(d=8.3,h=30,$fn=64,center=true);

hull()
{
translate([0,0,20]) rotate([90,0,90]) cylinder(d=3,h=90,$fn=64);
translate([0,0,75]) rotate([90,0,90]) cylinder(d=3,h=90,$fn=64);    
}


for(i=[0:120:240])
rotate([0,180,i])
{
//translate([0,0,0]) rotate([90,0,0])   cylinder(d=35,h=50,$fn=32,center=true);   
//translate([sliderRodX,0,0]) rotate([90,0,0])   cylinder(d=31,h=30,$fn=32,center=true);
translate([sliderRodX+5,0,0]) cube([40,31,50],center=true);
translate([sliderRodX,0,0]) rotate([0,supportAngle0,0]) translate([0,0,-25]) cube([30,30,50],center=true);    
    
translate([sliderRodX,0,0]) rotate([90,0,0])   cylinder(d=8.4,h=90,$fn=64,center=true);

translate([sliderRodX,29,0]) rotate([90,30,0])   cylinder(d=15,h=15,$fn=6,center=true);
translate([sliderRodX,-29,0]) rotate([90,30,0])   cylinder(d=20,h=15,$fn=64,center=true);        



}

translate([0,0,0]) rotate([0,0,0])   cylinder(d=30.5,h=190,$fn=64,center=true);

//for(i=[60:120:300])
//rotate([0,180,i])
//translate([sliderRodX,0,0]) rotate([0,0,30])   cylinder(d=12,h=40,$fn=64,center=true);

}

}

module topTJoint()
{
    difference()
    {
    union()
    {    
    hull()
    {
    rotate([90,0,0]) cylinder(d=35,h=48,$fn=64,center=true);
    cylinder(d=35,h=50,$fn=64);
    }
    
    rotate([0,-rodAngleMax,0]) translate([-35/4,0,-35/4]) cube([35/2,48,35/2],center=true);
    
    
}

translate([0,0,32])  rotate([90,0,0]) cylinder(d=5.2,h=99,$fn=64,center=true);

translate([0,27.5,32])  rotate([90,0,0]) cylinder(d=15,h=20,$fn=64,center=true);
translate([0,-27.5,32])  rotate([90,0,0]) cylinder(d=15,h=20,$fn=64,center=true);
translate([0,0,-10]) cylinder(d=25.3,h=90,$fn=64);
    translate([0,0,0])   rotate([90,0,0])  cylinder(d=8.2,h=99,$fn=64,center=true);
}
}

module sideJoint()
{
    difference()
    {
    union()
    {
    
translate([0,0,-57.5]) rotate([0,0,0]) cylinder(d2=40,d1=30,h=40,$fn=64);

    hull()
    {    
    translate([22,0,10]) rotate([90,0,0]) cylinder(d=15,h=25,$fn=64,center=true);
    translate([22,0,-10]) rotate([90,0,0]) cylinder(d=15,h=25,$fn=64,center=true);
    translate([4,0,0]) rotate([0,90,0]) cube([35,25,40],center=true);     
    }
    
    hull()
    {
    translate([0,0,-57.5]) rotate([0,0,0]) cylinder(d2=30,d1=30,h=40,$fn=64);    
    translate([18,0,-47.5]) rotate([90,0,0]) cylinder(d=20,h=30,$fn=64,center=true);
    translate([25/8,0,-37.5]) rotate([0,0,0]) cube([25/4,30,40],center=true);     
    }



hull()
    {

        intersection()
        {
        translate([-jointRodX,0,0]) rotate([90,0,0]) cylinder(d=35,h=50,$fn=64,center=true); 
        translate([-jointRodX,0,0]) rotate([0,0,0]) cylinder(d=55,h=35,$fn=64,center=true);     
        }
translate([0,0,0]) rotate([0,0,0]) cylinder(d=40,h=35,$fn=64,center=true); 

        
        }
        


    }
    translate([18,0,-47.5]) rotate([90,0,0]) cylinder(d=8.5,h=35,$fn=64,center=true);
    translate([18,13,-47.5]) rotate([90,30,0]) cylinder(d=15,h=10,$fn=6,center=true);
    
        translate([23,0,10]) rotate([90,0,0]) cylinder(d=5.3,h=30,$fn=64,center=true);
    translate([23,0,-10]) rotate([90,0,0]) cylinder(d=5.3,h=30,$fn=64,center=true);
    
    translate([23,12,10]) rotate([90,30,0]) cylinder(d=11,h=10,$fn=6,center=true);
    translate([23,12,-10]) rotate([90,30,0]) cylinder(d=11,h=10,$fn=6,center=true);

    translate([25,0,-27.5]) rotate([0,0,0]) cube([50,3,90],center=true);
        translate([0,0,-17.5]) cylinder(d=30.3,h=100,$fn=64);
    translate([0,0,-2-17.5]) cylinder(d2=30.3,d1=25.3,h=2,$fn=64);
     translate([0,0,-17.5]) cylinder(d=25.3,h=100,$fn=64,center=true);
    
    
    translate([-15,35,0]) rotate([0,90,0]) cylinder(d=15,h=10,$fn=6,center=true); 
   translate([-jointRodX,0,0]) rotate([90,0,0]) cylinder(d=8.5,h=60,$fn=64,center=true);
   translate([-jointRodX,0,0]) rotate([90,0,0]) cylinder(d=30,h=30,$fn=64,center=true);  
   translate([-jointRodX,0,0]) rotate([0,180-supportAngle0,0])   translate([0,0,25])   cube([30,30,50],center=true);
   translate([-5-jointRodX,0,25]) cube([40,30,50],center=true);
   translate([-5-jointRodX-30/2+10,0,0]) cube([40,30,50],center=true);

}


%color("green",.5) translate([0,0]) rotate([0,0,0])   cylinder(d=30,h=rod2l,$fn=16); 

%color("blue",.5) translate([-jointRodX,0,0]) rotate([180,supportAngle,0]) cylinder(d=25,h=supportRodLength,$fn=16); 
}

module endSlider()
{
    difference()
    {
    union()
    {
     translate([0,0,0])  rotate([0,0,0]) cylinder(d=23,$fn=64,h=40);
     translate([0,0,40])  rotate([0,0,0]) cylinder(d=27.7,$fn=64,h=10);   

    }
    translate([0,0,0])  cylinder(d=5.3,$fn=64,h=61);
    translate([0,0,0])  cylinder(d1=20,d2=5.3,$fn=64,h=20);
for(i=[0:90:360]) rotate([0,0,i]) translate([7,0,20]) cube([10,2,40],center=true);  
    
}
    }

module endSliderCap()
{
    difference()
    {
    //#translate([0,0,0])  cylinder(d1=20,d2=5.3,$fn=64,h=20);
    translate([0,0,0])  cylinder(d1=20,d2=12.65,$fn=64,h=10);    
    translate([0,0,0])  cylinder(d=5.3,$fn=64,h=61);
         translate([0,0,0])  cylinder(d=11,$fn=6,h=5);
        translate([0,0,5])  cylinder(d1=11,$fn=6,h=2);
}
    }    
    
//!endSliderCap();
module sideTjoint()
{
    difference()
    {
    intersection()
    {
     translate([0,0,10]) cube([30,30,50],center=true);     
    hull()
    {
    rotate([90,0,0]) cylinder(d=30,$fn=64,h=35,center=true);
    rotate([0,0,0]) cylinder(d=35,$fn=64,h=35);
    }
    }
    rotate([90,0,0]) cylinder(d=8.3,$fn=64,h=36,center=true);
     translate([0,0,25]) rotate([90,0,0]) cylinder(d=5.2,$fn=64,h=36,center=true);
    translate([0,0,-6])  rotate([0,0,0]) cylinder(d=25.3,$fn=64,h=50);
    }
    }

module endCap()
    {
        
            difference()
    {
    hull()
    {
      rotate([0,0,0]) cylinder(d=40,$fn=64,h=35);
      
      translate([0,0,30])  sphere(d=35,$fn=32);
    }
    
    translate([0,0,-20])    rotate([0,0,0]) cylinder(d=30.3,$fn=64,h=100,center=true);
    translate([0,0,10])    rotate([90,0,0]) cylinder(d=5.3,$fn=64,h=100,center=true);
    
    translate([0,24,10])    rotate([90,0,0]) cylinder(d=12,$fn=64,h=10,center=true);
    translate([0,-24,10])    rotate([90,0,0]) cylinder(d=12,$fn=64,h=10,center=true);
}
        
        }

baseTriangle();
//translate([0,0,hJoints+zJoints]) mirror() rotate([0,180,0])  centerSlider();
translate([0,0,hJoints+zJoints])   rotate([0,180,300])  centerSlider();
for(i=[0:120:240])
rotate([0,180,i]) 
translate([baseRodX,0,0]) rotate([0,rodAngle,0]) translate([0,0,-jointY])  sideJoint();


for(i=[0:120:240])
rotate([0,180,i]) 
translate([baseRodX,0,0]) rotate([0,rodAngle,0]) translate([0,0,rod1l])   

{
    endSlider();
endSliderCap();
}

for(i=[0:120:240])
rotate([0,180,i]) 
translate([baseRodX,0,0]) rotate([0,rodAngle,0]) translate([0,0,rod1l-jointY])   endCap();



translate([0,0,hJoints+zJoints])
for(i=[0:120:240])
rotate([0,180,i]) translate([sliderRodX,0,0]) 
rotate([0,rodAngle+supportAngle,0]) sideTjoint();

for(i=[0:120:240])
rotate([0,180,i]) 
translate([baseRodX,0,0]) rotate([0,rodAngle,0]) translate([-jointRodX,0,-jointY])   rotate([0,supportAngle-180,0]) sideTjoint();


for(i=[0:120:240])
rotate([0,180,i]) translate([baseRodX,0,0]) rotate([0,rodAngle,0]) 
topTJoint();