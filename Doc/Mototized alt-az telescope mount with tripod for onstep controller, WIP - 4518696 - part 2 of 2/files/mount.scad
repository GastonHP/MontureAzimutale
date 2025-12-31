//TODO: hub clutch
//TODO: double belt reducer.
//25 axle tube is hard attached to big pulley
//or not....
//extend axle tube through pulley
use <reducer.scad> 

//axleTo12tube=26;
axleTo12tube=37/2+6+1+.5;
yawAxleHeight=80.0;
pitchOffsetX=-5-25;
pitchOffsetY=111;
pitchOffsetZ=17.5+17.5-3;
pitchAdapterPos=40;
braceOffset=110;
holderD=12+6;

yawRot=27;
pitchRot=31;



//midBraceOffset=100;
beltTrainAngle=70;
beltTrainZmod=30;

braceMod=14;//center tube to  center tube 
sideXmodTop=25/2+6+2;
sideYmodTop=6+1;

Dbig=194.827;Dsmall=17.9546;
b=4*(768)-6.28*(Dbig+Dsmall);
pulley_distance=(b+sqrt((b*b)-32*((Dbig-Dsmall)*(Dbig-Dsmall))))/16;
pulleyH=20;

//secondary gt2 reducer, 90:16=5.625
Dbig2=60;Dsmall2=9.68;
b2=4*(200)-6.28*(Dbig2+Dsmall2);
pulley_distance2=(b2+sqrt((b2*b2)-32*((Dbig2-Dsmall2)*(Dbig2-Dsmall2))))/16;

echo("pulley_distance",pulley_distance);
echo("pulley_surfaces_distance2",pulley_distance2-Dbig2/2-Dsmall2/2);
echo("steps per angle second",(256*200*(209/20)*5.55)/(360*3600));
//The ESP32's fastest step rate is just shy of 5x faster at 10us/step.


//module dummyBearing6805()
//{
////6805: 25in, 37 out, primaryHeight=7mm
////edge rounding diameter=.6mm
////inner shoulder diameter=28
////outer shoulder diameter=33
//hull(){
//translate([0,0,3.5-.3]) cylinder(d1=37,d2=37-1.2,h=.6,center=true,$fn=128);//+1!
//translate([0,0,-3.5+.3]) cylinder(d2=37,d1=37-1.2,h=.6,center=true,$fn=128);}
//}   

module nema23motor()
{
difference()
{
union()
{
color("dimgray") hull()
{
translate([47.2/2,47.2/2,0]) cylinder(d=9.2,$fn=64,h=60);
translate([47.2/2,-47.2/2,0]) cylinder(d=9.2,$fn=64,h=60); 
translate([-47.2/2,47.2/2,0]) cylinder(d=9.2,$fn=64,h=60); 
translate([-47.2/2,-47.2/2,0]) cylinder(d=9.2,$fn=64,h=60);     
}
color("silver")    hull()
{
translate([47.2/2,47.2/2,60]) cylinder(d=9.2,$fn=64,h=4);
translate([47.2/2,-47.2/2,60]) cylinder(d=9.2,$fn=64,h=4); 
translate([-47.2/2,47.2/2,60]) cylinder(d=9.2,$fn=64,h=4); 
translate([-47.2/2,-47.2/2,60]) cylinder(d=9.2,$fn=64,h=4);     
}
color("darkgrey") translate([0,0,64]) cylinder(d=38.1,$fn=64,h=.25);
color("silver") translate([0,0,64]) cylinder(d=8,$fn=32,h=21);        
}
translate([47.2/2,47.2/2,60]) cylinder(d=4.5,$fn=16,h=10);
translate([47.2/2,-47.2/2,60]) cylinder(d=4.5,$fn=16,h=10); 
translate([-47.2/2,47.2/2,60]) cylinder(d=4.5,$fn=16,h=10); 
translate([-47.2/2,-47.2/2,60]) cylinder(d=4.5,$fn=16,h=10);
color("dimgray") translate([26,26,-1]) cylinder(d=15,$fn=16,h=61);
color("dimgray")translate([26,-26,-1]) cylinder(d=15,$fn=16,h=61); 
color("dimgray")translate([-26,26,-1]) cylinder(d=15,$fn=16,h=61); 
color("dimgray") translate([-26,-26,-1]) cylinder(d=15,$fn=16,h=61); 
}
}
module spCyl(height,D=12,fn=16)
{
hull() { sphere(d=D,$fn=fn); translate([0,0,height]) sphere(d=D,$fn=fn); };
}

module sqCyl(height,D=12,fn=16)
{
intersection()
{
cylinder(d=holderD+2,h=height,center=true);
cube([holderD,holderD,height],center=true);
}    

}

module belt(D1,D2,length,fn=32)
{
difference()
{  
hull()
{
translate([0,0,0]) cylinder(d=D1+4,h=15,$fn=fn,center=true); 
translate([length,0,0]) cylinder(d=D2+4,h=15,$fn=fn,center=true); 
}
hull()
{
translate([0,0,0]) cylinder(d=D1,h=15+1,$fn=fn,center=true); 
translate([length,0,0]) cylinder(d=D2,h=15+1,$fn=fn,center=true); 
}
}
}

module armTop_brace(sideAngle,braceAngle)
{//TODO: remove braceMod?

//primaryLength,primaryHeight,sideAngle,braceAngle,sideLength,braceLength);   
difference()
{
sideBolt1Xmod=7;
sideBolt2Xmod=25;
braceBolt1Xmod=sideBolt1Xmod+4;    
braceBolt2Xmod=sideBolt2Xmod+4;    
sideBolt1Ymod=tan(sideAngle)*(sideBolt1Xmod+5)+1/cos(sideAngle)*(holderD/2);
sideBolt2Ymod=tan(sideAngle)*(sideBolt2Xmod+5)+1/cos(sideAngle)*(holderD/2);
braceBolt1Xmod_cos=(cos(sideAngle)*(braceBolt1Xmod+5));
braceBolt1Zmod=tan(braceAngle)*braceBolt1Xmod_cos+(1/cos(braceAngle)*(holderD/2));
braceBolt2Xmod_cos=(cos(sideAngle)*(braceBolt2Xmod+5));
braceBolt2Zmod=tan(braceAngle)*braceBolt2Xmod_cos+(1/cos(braceAngle)*(holderD/2));//TODO: account sideAngle too
braceClampH=(40/cos(braceAngle)+tan(braceAngle)*(holderD/2));    
union()
{    

translate([-sideXmodTop,-sideYmodTop,braceMod])  rotate([0,braceAngle-90,sideAngle])
intersection()
{
translate([0,0,braceClampH/2]) cube([holderD,holderD,braceClampH],center=true); 
cylinder(h=braceClampH,d=holderD+2,$fn=32);
}
translate([-sideXmodTop,-sideYmodTop,0]) rotate([0,-90,sideAngle])
intersection()
{
translate([0,0,20]) cube([holderD,holderD,40],center=true); 
cylinder(h=40,d=holderD+2,$fn=32);
}
translate([-sideXmodTop,sideYmodTop,0]) rotate([0,-90,-sideAngle])
intersection()
{
translate([0,0,20]) cube([holderD,holderD,40],center=true); 
cylinder(h=40,d=holderD+2,$fn=32);
}

hull()
{
translate([-sideXmodTop,-sideYmodTop,0]) rotate([0,-90,sideAngle])
translate([0,0,20]) cube([8,holderD,40],center=true); 
translate([-sideXmodTop,sideYmodTop,0]) rotate([0,-90,-sideAngle])
translate([0,0,20]) cube([8,holderD,40],center=true); 
}

hull()
{
translate([-sideXmodTop,-sideYmodTop,braceMod])  rotate([0,braceAngle-90,sideAngle])
translate([0,0,braceClampH/2]) cube([holderD,8,braceClampH],center=true); 
translate([-sideXmodTop,-sideYmodTop,0]) rotate([0,-90,sideAngle])
translate([0,0,20]) cube([holderD,8,40],center=true); 
}

hull()
{
translate([-sideXmodTop,-sideYmodTop,braceMod])  rotate([0,0,sideAngle])
intersection()
{
translate([0,0,0]) cube([holderD,holderD,holderD],center=true); 
sphere(d=holderD+2,$fn=32);
}
translate([-sideXmodTop,-sideYmodTop,0]) rotate([0,-90,sideAngle])
intersection()
{
translate([0,0,0]) cube([holderD,holderD,holderD],center=true); 
sphere(d=holderD+2,$fn=32);
}   
translate([-sideXmodTop,sideYmodTop,0]) rotate([0,-90,-sideAngle])
intersection()
{
translate([0,0,0]) cube([holderD,holderD,holderD],center=true); 
sphere(d=holderD+2,$fn=32);
}
translate([0,0,-holderD/2]) cylinder(d=35,h=holderD+braceMod,$fn=128);
}



hull()
{
translate([20,0,-holderD/2+7]) rotate([90,0,0]) cylinder(d=14,h=20,$fn=32,center=true);
translate([20,0,holderD/2+braceMod-7]) rotate([90,0,0]) cylinder(d=14,h=20,$fn=32,center=true);
translate([11,0,(braceMod)/2]) cube([22,20,holderD+braceMod],center=true);
}



translate([-sideXmodTop-sideBolt1Xmod,0,0]) rotate([90,0,0]) 
cylinder(d=10,h=(sideYmodTop*2)+sideBolt1Ymod*2,$fn=32,center=true);


translate([-sideXmodTop-sideBolt2Xmod,0,0]) rotate([90,0,0]) 
cylinder(d=10,h=(sideYmodTop*2)+sideBolt2Ymod*2,$fn=32,center=true);



translate([-sideXmodTop-braceBolt1Xmod,-braceBolt1Xmod*sin(sideAngle)-sideYmodTop,-holderD/2])        
cylinder(d=10,h=braceMod+braceBolt1Zmod+holderD/2,$fn=64);

translate([-sideXmodTop-braceBolt2Xmod,-braceBolt2Xmod*sin(sideAngle)-sideYmodTop,-holderD/2])        
cylinder(d=10,h=braceMod+braceBolt2Zmod+holderD/2,$fn=64);

}
translate([-95-25/2,0,holderD/2]) cube([100,200,100],center=true);
translate([16,0,(braceMod)/2]) cube([32,2,holderD+braceMod+2],center=true);
translate([0,0,-holderD/2-1]) cylinder(d=25.2,h=holderD+braceMod+2,$fn=128);
translate([20,0,-holderD/2+7]) rotate([90,0,0]) cylinder(d=5.2,h=22,$fn=32,center=true);
translate([20,0,holderD/2+braceMod-7]) rotate([90,0,0]) cylinder(d=5.2,h=22,$fn=32,center=true);

translate([-sideXmodTop,-sideYmodTop,braceMod])  rotate([0,braceAngle-90,sideAngle]) spCyl(40,12.2);
translate([-sideXmodTop,-sideYmodTop,0]) rotate([0,-90,sideAngle]) spCyl(40,12.2);    
translate([-sideXmodTop,sideYmodTop,0]) rotate([0,-90,-sideAngle]) spCyl(40,12.2);    

translate([-sideXmodTop-sideBolt1Xmod,0,0]) rotate([90,0,0]) 
cylinder(d=4.2,h=(sideYmodTop*2)+sideBolt1Ymod*2+1,$fn=32,center=true);
translate([-sideXmodTop-sideBolt2Xmod,0,0]) rotate([90,0,0]) 
cylinder(d=4.2,h=(sideYmodTop*2)+sideBolt2Ymod*2+1,$fn=32,center=true);
translate([-sideXmodTop-braceBolt1Xmod,-braceBolt1Xmod*sin(sideAngle)-sideYmodTop,-holderD/2-1])        
cylinder(d=4.2,h=braceMod+braceBolt1Zmod+holderD/2+2,$fn=64);
translate([-sideXmodTop-braceBolt2Xmod,-braceBolt2Xmod*sin(sideAngle)-sideYmodTop,-holderD/2-1])        
cylinder(d=4.2,h=braceMod+braceBolt2Zmod+holderD/2+2,$fn=64);


}
}
//armTop_brace(sideAngle=15,braceAngle=15);

module m4Cutout()
{

translate([0,0,-4.5-.3]) cylinder(d=4.2,h=holderD+9,center=true,$fn=32);
translate([0,0,holderD/2])     
hull()
{
translate([0,0,0]) rotate([0,0,15]) cylinder(d=8,h=4.5,$fn=6);
rotate([0,0,45]) translate([0,-33,0]) rotate([0,0,30]) cylinder(d=8,h=4.5,$fn=6);
translate([0,0,4.5]) rotate([0,0,15]) cylinder(d1=8,h=4.5,$fn=6);
rotate([0,0,45]) translate([0,-33,4.5]) rotate([0,0,30]) cylinder(d1=8,h=4.5,$fn=6);
}

}

module axisSide()
{
//Yaw axle is static?


difference()
{
union()
{
//yaw
rotate([0,0,yawRot]) {
intersection()
{
hull()
{
translate([0,axleTo12tube,0]) rotate([0,90,0]) cylinder(d=holderD+2,$fn=32,h=54,center=true);
translate([0,-axleTo12tube,0]) rotate([0,90,0]) cylinder(d=holderD+2,$fn=32,h=54,center=true);
}
hull()
{
translate([0,0,0]) cube([54,50+holderD,holderD],center=true);
}
}
translate([0,0,0]) cylinder(d=30,$fn=64,h=yawAxleHeight);

translate([0,0,yawAxleHeight]) cylinder(d1=50,d2=50,$fn=128,h=holderD,center=true);
translate([0,0,yawAxleHeight+1])  rotate([180,0,0]) bearing6805Clamp();    
translate([0,0,yawAxleHeight-holderD/2-holderD/4]) cylinder(d1=30,d2=50,$fn=128,h=holderD/2,center=true);
%translate([0,0,0])  rotate([0,0,0]) bearing6805Clamp();    

hull()
{
rotate([0,0,-yawRot]) translate([0,25-4,-holderD/2]) cylinder(h=yawAxleHeight+holderD,d=8);
rotate([0,0,-yawRot]) translate([0,4-25,-holderD/2]) cylinder(h=yawAxleHeight+holderD,d=8);
rotate([0,0,-yawRot])   translate([pitchOffsetX,-axleTo12tube-holderD/2, pitchOffsetZ+25-4]) rotate([-90,0,0])
cylinder(d=8,$fn=64,h=pitchOffsetY+axleTo12tube+holderD);
rotate([0,0,-yawRot])   translate([pitchOffsetX,-axleTo12tube-holderD/2, pitchOffsetZ-27+4+2]) rotate([-90,0,0])
cylinder(d=8,$fn=64,h=pitchOffsetY+axleTo12tube+holderD);

rotate([0,0,0])   translate([0,axleTo12tube,0]) 
cylinder(d=8,$fn=64,h=holderD,center=true);

rotate([0,0,-yawRot]) translate([pitchOffsetX,pitchOffsetY, -holderD/2]) 
cylinder(d=8,$fn=64,h=pitchOffsetZ);
}

}
//pitch
translate([pitchOffsetX,-axleTo12tube, pitchOffsetZ]) rotate([-90,0,0])
cylinder(d=30,$fn=128,h=pitchOffsetY+axleTo12tube);
hull()
{
translate([pitchOffsetX,-axleTo12tube, pitchOffsetZ]) rotate([-90,0,0])
cylinder(d2=50,d1=50,$fn=128,h=holderD,center=true);
}
%translate([pitchOffsetX,-axleTo12tube, pitchOffsetZ]) rotate([-90,0,0]) bearing6805Clamp();    
%translate([pitchOffsetX,pitchOffsetY+4, pitchOffsetZ]) rotate([90,0,0]) bearing6805Clamp();    

intersection()
{
hull()
{
translate([pitchOffsetX,pitchOffsetY, pitchOffsetZ]) rotate([-90,0,0])
cylinder(d=50,$fn=64,h=holderD,center=true);
translate([pitchOffsetX,pitchOffsetY, 0]) cylinder(d=holderD*2,$fn=64,h=holderD,center=true);

translate([pitchOffsetX,pitchOffsetY, pitchOffsetZ]) rotate([0,90+pitchRot,0]) translate([axleTo12tube,0,0]) 
cylinder(d=holderD+2,$fn=64,h=54,center=true);
translate([pitchOffsetX,pitchOffsetY, pitchOffsetZ]) rotate([0,90+pitchRot,0]) translate([-axleTo12tube,0,0]) 
cylinder(d=holderD+2,$fn=64,h=54,center=true);
}
translate([pitchOffsetX,pitchOffsetY, pitchOffsetZ]) rotate([0,pitchRot+90,0]) translate([20,0,0]) 
cube([50+holderD+40,holderD,54],center=true);
translate([pitchOffsetX,pitchOffsetY, 50-holderD/2]) rotate([0,0,0]) translate([20,0,0]) 
cube([199,holderD,100],center=true);
}

}


rotate([0,0,yawRot]) {
hull()
{
translate([0,0,3.5-.3]) cylinder(d1=37,d2=37-1.2,h=.6,center=true,$fn=128);//+1!
translate([0,0,-3.5+.3-11]) cylinder(d2=37,d1=37-1.2,h=.6,center=true,$fn=128);
}
cylinder(d=4.2,h=holderD+9,center=true,$fn=32); 
translate([0,0,-holderD/2-0.001]) cylinder(d2=37,d1=50,h=(holderD-7)/2+.001,$fn=128);
translate([0,0,3.5-0.001-0.6]) cylinder(d1=37,d2=26,h=7,$fn=128);
rotate([0,0,45]) translate([22,0,0]) 
m4Cutout();
rotate([0,0,135]) translate([22,0,0]) rotate([0,0,45])
m4Cutout();
rotate([0,0,225]) translate([22,0,0]) rotate([0,0,90])
m4Cutout();
rotate([0,0,315]) translate([22,0,0]) rotate([0,0,90])
m4Cutout();

translate([13,-axleTo12tube,0])  rotate([0,0,45]) m4Cutout();
translate([-13,-axleTo12tube,0])  rotate([0,0,-45]) m4Cutout();
translate([13,axleTo12tube,0])  rotate([0,0,45]) m4Cutout();
translate([-13,axleTo12tube,0])  rotate([0,0,225]) m4Cutout();


translate([0,0,yawAxleHeight])
{
translate([0,0,3.5]) cylinder(d1=37,d2=50,h=(holderD-7)/2+.001,$fn=128);
hull()
{
translate([0,0,3.5-.3+11]) cylinder(d1=37,d2=37-1.2,h=.6,center=true,$fn=128);//+1!
translate([0,0,-3.5+.3]) cylinder(d2=37,d1=37-1.2,h=.6,center=true,$fn=128);
}
rotate([0,0,45]) translate([22,0,0])  rotate([0,180,135])
m4Cutout();
rotate([0,0,135]) translate([22,0,0]) rotate([0,180,150])
m4Cutout();
rotate([0,0,225]) translate([22,0,0]) rotate([0,180,(225+135)/2])
m4Cutout();
rotate([0,0,315]) translate([22,0,0]) rotate([0,180,135])
m4Cutout();
}
translate([0,0,-yawAxleHeight]) cylinder(d=25.8,h=3*yawAxleHeight,$fn=128);           
}
rotate([0,0,yawRot]) translate([-20,axleTo12tube,0]) rotate([0,90,0]) cylinder(h=400,$fn=32,d=12.2,center=true);
rotate([0,0,yawRot]) translate([-20,-axleTo12tube,0]) rotate([0,90,0]) cylinder(h=400,$fn=32,d=12.2,center=true);

translate([pitchOffsetX,pitchOffsetY, pitchOffsetZ])
rotate([90,30,0])
{
translate([0,0,-holderD/2-0.001]) cylinder(d2=37,d1=50,h=(holderD-7)/2+.001,$fn=128);
rotate([0,0,45]) translate([22,0,0])  rotate([0,0,0])
m4Cutout();
rotate([0,0,135]) translate([22,0,0]) rotate([0,0,90])
m4Cutout();
rotate([0,0,225]) translate([22,0,0]) rotate([0,0,90])
m4Cutout();
rotate([0,0,315]) translate([22,0,0]) rotate([0,0,90])
m4Cutout();

translate([13,-axleTo12tube,0])  rotate([0,0,225]) m4Cutout();
translate([-13,-axleTo12tube,0])  rotate([0,0,-45]) m4Cutout();
translate([13,axleTo12tube,0])  rotate([0,0,45]) m4Cutout();
translate([-13,axleTo12tube,0])  rotate([0,0,225]) m4Cutout();    


hull()
{
translate([0,0,3.5-.3]) cylinder(d1=37,d2=37-1.2,h=.6,center=true,$fn=128);//+1!
translate([0,0,-3.5+.3-11]) cylinder(d2=37,d1=37-1.2,h=.6,center=true,$fn=128);
}
translate([0,0,pitchOffsetY+axleTo12tube])
{
hull()
{
translate([0,0,3.5-.3+11]) cylinder(d1=37,d2=37-1.2,h=.6,center=true,$fn=128);
translate([0,0,-3.5+.3]) cylinder(d2=37,d1=37-1.2,h=.6,center=true,$fn=128);
}
translate([0,0,3.5]) cylinder(d1=37,d2=50,h=(holderD-7)/2+.001,$fn=128);

rotate([0,0,45]) translate([22,0,0])  rotate([0,180,135])
m4Cutout();
rotate([0,0,135]) translate([22,0,0]) rotate([0,180,225])
m4Cutout();
rotate([0,0,225]) translate([22,0,0]) rotate([0,180,135])
m4Cutout();
rotate([0,0,315]) translate([22,0,0]) rotate([0,180,90])
m4Cutout();
}
translate([0,0,-pitchOffsetY]) cylinder(d=25.8,h=3*pitchOffsetY,$fn=128)     ;
}
translate([pitchOffsetX,pitchOffsetY,pitchOffsetZ]) rotate([0,pitchRot,0]) translate([-25,0,axleTo12tube])    rotate([0,90,0]) cylinder(h=400,$fn=32,d=12.2,center=true);
translate([pitchOffsetX,pitchOffsetY,pitchOffsetZ]) rotate([0,pitchRot,0]) translate([-25,0,-axleTo12tube])    rotate([0,90,0]) cylinder(h=400,$fn=32,d=12.2,center=true);

}

}

module middleBrace()
{

difference()
{
intersection()
{    
////
translate([pitchOffsetX,pitchOffsetY,0]) translate([braceOffset,0,0])    rotate([-yawRot,90+pitchRot,0]) 
cylinder(d=300,h=30,center=true);

union()
{    
hull()
{
translate([pitchOffsetX,pitchOffsetY,pitchOffsetZ]) rotate([0,pitchRot,0]) translate([braceOffset/cos(pitchRot),0,axleTo12tube])    rotate([0,90,0]) 
sqCyl(80);
translate([pitchOffsetX,pitchOffsetY,pitchOffsetZ]) rotate([0,pitchRot,0]) translate([braceOffset/cos(pitchRot),0,-axleTo12tube])    rotate([0,90,0]) 
sqCyl(80);
//translate([pitchOffsetX,pitchOffsetY,0]) translate([braceOffset,0,0])    rotate([0,90,0]) 
//sqCyl(60);
translate([pitchOffsetX,pitchOffsetY,pitchOffsetZ]) rotate([0,pitchRot,0]) translate([braceOffset/cos(pitchRot),0,axleTo12tube])    
rotate([0,90,0]) rotate([0,-pitchRot,0])
sqCyl(80);

}

hull()
{
rotate([0,0,yawRot]) translate([braceOffset/cos(yawRot),axleTo12tube,0]) rotate([0,90,0])
sqCyl(70);
rotate([0,0,yawRot]) translate([braceOffset/cos(yawRot),-axleTo12tube,0]) rotate([0,90,0])
sqCyl(70);
translate([pitchOffsetX,pitchOffsetY,0]) translate([braceOffset,0,0])    rotate([0,90,0]) 
sqCyl(60);
}
}

}
translate([pitchOffsetX,pitchOffsetY,pitchOffsetZ]) rotate([0,pitchRot,0]) translate([braceOffset/cos(pitchRot)-15,0,axleTo12tube])  
rotate([90,-135,0]) 
m4Cutout();
translate([pitchOffsetX,pitchOffsetY,pitchOffsetZ]) rotate([0,pitchRot,0]) translate([braceOffset/cos(pitchRot)-15,0,-axleTo12tube])  
rotate([90,180-135,0]) 
m4Cutout();
rotate([0,0,yawRot]) translate([braceOffset/cos(yawRot)+5,axleTo12tube,0]) rotate([0,0,45])
m4Cutout();
rotate([0,0,yawRot]) translate([braceOffset/cos(yawRot)+5,-axleTo12tube,0]) rotate([0,0,45])
m4Cutout();

rotate([0,0,yawRot]) translate([braceOffset/cos(yawRot),axleTo12tube,0]) rotate([0,90,0])
cylinder(d=12.2,h=250,center=true);
rotate([0,0,yawRot]) translate([braceOffset/cos(yawRot),-axleTo12tube,0]) rotate([0,90,0])
cylinder(d=12.2,h=250,center=true);
translate([pitchOffsetX,pitchOffsetY,pitchOffsetZ]) rotate([0,pitchRot,0]) translate([braceOffset/cos(pitchRot),0,axleTo12tube])    rotate([0,90,0]) 
cylinder(d=12.2,h=250,center=true);
translate([pitchOffsetX,pitchOffsetY,pitchOffsetZ]) rotate([0,pitchRot,0]) translate([braceOffset/cos(pitchRot),0,-axleTo12tube])    rotate([0,90,0]) 
cylinder(d=12.2,h=250,center=true);
}

}

module nema23Mount()
{
difference()
{
union()
{
translate([0,-axleTo12tube,0])
rotate([0,90,0]) sqCyl(60);
translate([0,axleTo12tube,0])
rotate([0,90,0]) sqCyl(60);

hull()
{    
hull()
{
translate([47.2/2,47.2/2,0]) cylinder(d=12.8,$fn=32,h=holderD,center=true);
translate([47.2/2,-47.2/2,0])cylinder(d=12.8,$fn=32,h=holderD,center=true);
translate([-47.2/2,47.2/2,0]) cylinder(d=12.8,$fn=32,h=holderD,center=true);
translate([-47.2/2,-47.2/2,0]) cylinder(d=12.8,$fn=32,h=holderD,center=true);
}    

translate([0,-axleTo12tube-9,0]) 
cylinder(h=holderD,d=12,$fn=32,center=true);
translate([0,axleTo12tube+9,0]) 
cylinder(h=holderD,d=12,$fn=32,center=true);        
}
hull()
{
translate([47.2/2,47.2/2,holderD/2]) cylinder(d1=12.8,d2=12.8,$fn=32,h=7);
translate([47.2/2,-47.2/2,holderD/2]) cylinder(d1=12.8,d2=12.8,$fn=32,h=7); 
translate([-47.2/2,47.2/2,holderD/2]) cylinder(d1=12.8,d2=12.8,$fn=32,h=7); 
translate([-47.2/2,-47.2/2,holderD/2]) cylinder(d1=12.8,d2=12.8,$fn=32,h=7);
}    
}
translate([0,-axleTo12tube,0])     rotate([0,90,0]) 
cylinder(d=12.2,h=100,center=true,$fn=32);
translate([0,axleTo12tube,0])     rotate([0,90,0]) 
cylinder(d=12.2,h=100,center=true,$fn=32);


translate([47.2/2,47.2/2,0]) cylinder(d=8.2,$fn=6,h=holderD/2+4);
translate([47.2/2,-47.2/2,0]) cylinder(d=8.2,$fn=6,h=holderD/2+4); 
translate([-47.2/2,47.2/2,0]) cylinder(d=8.2,$fn=6,h=holderD/2+4); 
translate([-47.2/2,-47.2/2,0]) cylinder(d=8.2,$fn=6,h=holderD/2+4);

translate([47.2/2,47.2/2,0]) cylinder(d=4.2,$fn=16,h=99,center=true);
translate([47.2/2,-47.2/2,0]) cylinder(d=4.2,$fn=16,h=99,center=true); 
translate([-47.2/2,47.2/2,0]) cylinder(d=4.2,$fn=16,h=99,center=true); 
translate([-47.2/2,-47.2/2,0]) cylinder(d=4.2,$fn=16,h=99,center=true);        


translate([0,100-axleTo12tube,0])     rotate([0,90,0]) 
cube([2,100,100],center=true);
translate([0,-100+axleTo12tube,0])     rotate([0,90,0]) 
cube([2,100,100],center=true);

translate([0,-axleTo12tube-9,0]) 
cylinder(h=holderD+2,d=4.2,$fn=32,center=true);
translate([0,axleTo12tube+9,0]) 
cylinder(h=holderD+2,d=4.2,$fn=32,center=true);     

translate([0,-axleTo12tube-9,holderD/2-4]) 
cylinder(h=holderD+2,d=8.2,$fn=6);
translate([0,axleTo12tube+9,holderD/2-4]) 
cylinder(h=holderD+2,d=8.2,$fn=6);
translate([0,-axleTo12tube-9,-holderD/2-0.01]) 
cylinder(h=4,d=8.2,$fn=32);
translate([0,axleTo12tube+9,-holderD/2-0.01]) 
cylinder(h=4,d=8.2,$fn=32); 

//#hull()
//{
//translate([8,0,0]) cylinder(d=37,h=holderD*2,center=true,$fn=128);
//translate([-8,0,0]) cylinder(d=37,h=holderD*2,center=true,$fn=128);    
//}

translate([0,0,0]) cylinder(d=37,h=holderD*2,center=true,$fn=128);


translate([0,0,0])     rotate([0,90,0]) 
cylinder(d=12.2,h=100,center=true,$fn=32);
}

}    

module pulleysBeltsMotors()
{
//Yaw
rotate([0,0,yawRot]) {    
%translate([0,0,-yawAxleHeight]) cylinder(d=25,h=2*yawAxleHeight);           
translate([0,0,0 -beltTrainZmod]) rotate([0,0,0])  import("pulleyBig.stl");
translate([0,0,30-beltTrainZmod]) rotate([180,0,0])  import("pulleyHub.stl");

translate([0,0,30-beltTrainZmod]) rotate([180,0,0]) 
{
import("hubTop.stl");
import("clutchPad1.stl");    
import("clutchPad2.stl");    
import("clutchDrum.stl");    
}


%translate([0,0,10 -beltTrainZmod])  color("black",.5)    belt(194.827,17.9546,pulley_distance,fn=32);
translate([0,0, 20-beltTrainZmod])  translate([pulley_distance,0,0]) rotate([0,180,0]) import("pulleySmall.stl");
//%translate([pulley_distance,0,80])    rotate([180,0,0])  nema23motor();
translate([pulley_distance,0,37.5])   color("green") rotate([180,0,180]) reducer1();
translate([pulley_distance,0,37.5])   color("green") rotate([180,0,180]) reducer2();
}
%rotate([0,0,yawRot]) translate([-20,axleTo12tube,0]) rotate([0,90,0]) cylinder(h=250,d=12);
%rotate([0,0,yawRot]) translate([-20,-axleTo12tube,0]) rotate([0,90,0]) cylinder(h=250,d=12);
//Pitch
translate([pitchOffsetX,pitchOffsetY, pitchOffsetZ])
rotate([90,pitchRot,0])
{
%hull()
{
translate([0,0,3.5-.3]) cylinder(d1=37,d2=37-1.2,h=.6,center=true,$fn=128);//+1!
translate([0,0,-3.5+.3]) cylinder(d2=37,d1=37-1.2,h=.6,center=true,$fn=128);
}
%translate([0,0,pitchOffsetY+axleTo12tube]) hull()
{
translate([0,0,3.5-.3]) cylinder(d1=37,d2=37-1.2,h=.6,center=true,$fn=128);//+1!
translate([0,0,-3.5+.3]) cylinder(d2=37,d1=37-1.2,h=.6,center=true,$fn=128);
}        
translate([0,0,-beltTrainZmod]) rotate([0,0,0])  import("pulleyBig.stl");
translate([0,0,30-beltTrainZmod]) rotate([180,0,0])  import("pulleyHub.stl");


translate([0,0,30-beltTrainZmod]) rotate([180,0,0]) 
{
import("hubTop.stl");
import("clutchPad1.stl");    
import("clutchPad2.stl");    
import("clutchDrum.stl");    
}

%translate([0,0,0]) cylinder(d=25,h=2*pitchOffsetY)     ;
%translate([0,0, 10-beltTrainZmod])  color("black",.5)    belt(194.827,17.9546,pulley_distance,fn=32);
translate([0,0, 20-beltTrainZmod])  translate([pulley_distance,0,0])  rotate([0,180,0]) import("pulleySmall.stl");
//%translate([pulley_distance,0,80])    rotate([180,0,0])  nema23motor();

translate([pulley_distance,0,37.5])    color("green")rotate([180,0,180]) reducer1();
translate([pulley_distance,0,37.5])   color("green")  rotate([180,0,180]) reducer2();
}
%translate([pitchOffsetX,pitchOffsetY,pitchOffsetZ]) rotate([0,pitchRot,0]) translate([-25,0,axleTo12tube])    rotate([0,90,0]) cylinder(h=250,d=12);
%translate([pitchOffsetX,pitchOffsetY,pitchOffsetZ]) rotate([0,pitchRot,0]) translate([-25,0,-axleTo12tube])    rotate([0,90,0]) cylinder(h=250,d=12);

translate([pitchOffsetX,pitchOffsetY, pitchOffsetZ])
rotate([90,pitchRot,0])
translate([pulley_distance,0,0])    rotate([0,0,0]) nema23Mount();

rotate([0,0,yawRot])
translate([pulley_distance,0,0])    rotate([0,0,0]) nema23Mount();
}

module bearing6805Clamp()
{

difference(){
union()
{    
translate([0,0,-holderD/2]) cylinder(d2=36.8,d1=49.8,h=(holderD-7)/2,$fn=128);
hull()
{    
translate([0,0,-holderD/2-2]) cylinder(d=50,h=2,$fn=128);
for(i=[45:90:315])
rotate([0,0,i]) translate([22,0,-1-holderD/2]) 
cylinder(d=10,h=2,$fn=32,center=true); 
}
}
translate([0,0,-holderD/2-0.001]) cylinder(d=33,h=holderD*2,$fn=128,center=true);

rotate([0,0,45]) translate([22,0,0]) 
cylinder(d=4.2,h=110,$fn=32,center=true);
rotate([0,0,135]) translate([22,0,0]) rotate([0,0,45])
cylinder(d=4.2,h=110,$fn=32,center=true);
rotate([0,0,225]) translate([22,0,0]) rotate([0,0,90])
cylinder(d=4.2,h=110,$fn=32,center=true);
rotate([0,0,315]) translate([22,0,0]) rotate([0,0,90])
cylinder(d=4.2,h=110,$fn=32,center=true);
}

echo("(holderD-7)/2",(holderD-7)/2,"+2 +4  ,22");

}
//TODO: expansible ring

module pitchAdapter()
{
translate([pitchOffsetX,0, pitchOffsetZ]) rotate([90,pitchRot,0]) translate([0,0,pitchAdapterPos]) rotate([0,0,pitchRot]) 
difference()
{
union()
{
hull()
{
//translate([0,0,35/2])  cube([35,20,35],center=true);

translate([20,0,10]) rotate([90,0,0]) cylinder(d=20,h=15,$fn=64,center=true)  ;
translate([20,0,30]) rotate([90,0,0]) cylinder(d=20,h=15,$fn=64,center=true)  ;
translate([20,0,50]) rotate([90,0,0]) cylinder(d=20,h=15,$fn=64,center=true)  ;
translate([-20,0,10]) rotate([90,0,0]) cylinder(d=20,h=15,$fn=64,center=true)  ;
translate([-20,0,30]) rotate([90,0,0]) cylinder(d=20,h=15,$fn=64,center=true)  ;
translate([-20,0,50]) rotate([90,0,0]) cylinder(d=20,h=15,$fn=64,center=true)  ;
translate([0,0,70-10/2])  cube([60,15,10],center=true);
}

hull()
{
translate([0,15,0])  cylinder(d=5,h=70,$fn=64);
translate([0,-15,0])  cylinder(d=5,h=70,$fn=64);
translate([0,32.5,60])  cylinder(d=5,h=10,$fn=64);
translate([0,-32.5,60])  cylinder(d=5,h=10,$fn=64);                        
}     

translate([0,0,0])  cylinder(d=35,h=70,$fn=64);
translate([0,0,60])  cylinder(d=70,h=10,$fn=128);
}
translate([20,-15,50])  cylinder(d=8.5,h=30,$fn=64);
translate([-20,-15,50])  cylinder(d=8.5,h=30,$fn=64);
translate([20,15,50])  cylinder(d=8.5,h=30,$fn=64);
translate([-20,15,50])  cylinder(d=8.5,h=30,$fn=64);                    


translate([22,0,10]) rotate([90,0,0]) cylinder(d=5.2,h=16,$fn=64,center=true)  ;
translate([22,0,30]) rotate([90,0,0]) cylinder(d=5.2,h=16,$fn=64,center=true)  ;
translate([22,0,50]) rotate([90,0,0]) cylinder(d=5.2,h=16,$fn=64,center=true)  ;
translate([-22,0,10]) rotate([90,0,0]) cylinder(d=5.2,h=16,$fn=64,center=true)  ;
translate([-22,0,30]) rotate([90,0,0]) cylinder(d=5.2,h=16,$fn=64,center=true)  ;
translate([-22,0,50]) rotate([90,0,0]) cylinder(d=5.2,h=16,$fn=64,center=true)  ;

translate([0,0,0])  cylinder(d=25.3,h=80,$fn=64);
translate([0,0,40])  cube([80,2,80],center=true);
}
}

module yawAdapter()
{
    translate([0,0,-100])
    difference()
    {
     union()
     {   
    cylinder(d=50,h=30,$fn=128);
    //cylinder(d=90,h=5,$fn=128);
   
   intersection()
   {      
   hull()
   {      
    translate([-4,22,7.5]) rotate([0,90,0]) cylinder(d=20,h=60,$fn=64,center=true);
  translate([-4,22,22.5]) rotate([0,90,0]) cylinder(d=20,h=60,$fn=64,center=true);
           translate([-4,-22,7.5]) rotate([0,90,0]) cylinder(d=20,h=60,$fn=64,center=true);
  translate([-4,-22,22.5]) rotate([0,90,0]) cylinder(d=20,h=60,$fn=64,center=true);
   
       }
       cylinder(d=500,h=30,$fn=128);
   }
         
      for(i=[0:120:360]) rotate([0,0,i])
      hull()
 {
     translate([45,0,0]) cylinder(d=30,h=30,$fn=32);
     translate([0,0,0]) cylinder(d=30,h=30,$fn=32);
      }
  }
  
  translate([0,22,7.5]) rotate([0,90,0]) cylinder(d=8.5,h=165,$fn=64,center=true);
  translate([0,22,22.5]) rotate([0,90,0]) cylinder(d=8.5,h=165,$fn=64,center=true);
  translate([0,-22,7.5]) rotate([0,90,0]) cylinder(d=8.5,h=165,$fn=64,center=true);
  translate([0,-22,22.5]) rotate([0,90,0]) cylinder(d=8.5,h=165,$fn=64,center=true);
  
  hull()
  {
  translate([-38.5,22,7.5]) rotate([0,90,0]) cylinder(d=20,h=5,$fn=64);
  translate([-38.5,22,22.5]) rotate([0,90,0]) cylinder(d=20,h=5,$fn=64);
  translate([-38.5,-22,7.5]) rotate([0,90,0]) cylinder(d=20,h=5,$fn=64);
  translate([-38.5,-22,22.5]) rotate([0,90,0]) cylinder(d=20,h=5,$fn=64);
  }
  translate([0,0,0]) cube([2,99,99],center=true); 
 
       for(i=[0:120:360]) rotate([0,0,i])
     
 {
//    
     translate([45,0,22]) cylinder(d=15,h=10,$fn=6);
     translate([45,0,8.3]) cylinder(d=8.5,h=40,$fn=64);
     translate([45,0,-2]) cylinder(d=20,h=10,$fn=64);
      }
  
     translate([0,0,-2]) cylinder(d=25.3,h=60,$fn=128);
    
    }
}
    
yawAdapter();
pitchAdapter();
pulleysBeltsMotors();
axisSide();
middleBrace();
translate([0,0,-130]) import("tripod.stl");
%translate([0,-100-190,0]) rotate([0,-45,0])  cylinder(d=150,h=1000,center=true);
%translate([0,-100-190,0]) rotate([0,45-45,0])  cylinder(d=150,h=1000,center=true);

//%for(i=[0:120:240]) rotate([0,0,i])
//translate([-50,0,-100]) rotate([0,-150,0])   
//cylinder(d=25,h=700,$fn=32);

//rotate([0,0,-20])  {
//%translate([0,0,0]) cylinder(d=25,h=mountHeight,$fn=32);
//%import("pulleyBig98.stl");
//%translate([0,0,10])    rotate([0,0,90]) belt(194.827,17.9546,pulley_distance,fn=32);
//%translate([0,pulley_distance,0]) rotate([0,0,0])  import("pulleySmall.stl");
//}