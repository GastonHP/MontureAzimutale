

//nema14: 35.2x35.2
//holes 26.1x26.1
//secondary gt2 reducer, 90:16=5.625
Dbig2=60;Dsmall2=9.68;
b2=4*(200)-6.28*(Dbig2+Dsmall2);
pulley_distance2=(b2+sqrt((b2*b2)-32*((Dbig2-Dsmall2)*(Dbig2-Dsmall2))))/16;

echo("pulley_distance 90 to 16t",pulley_distance2-Dsmall2/2-Dbig2/2);
echo("steps per angle second",(256*200*(209/20)*5.55)/(360*3600));


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

module belt()
{
    difference()
    {
        hull()
        {
            cylinder(d=60,$fn=64,h=7);
            translate([pulley_distance2,0,0]) cylinder(d=12,$fn=64,h=7);
            }
                    hull()
        {
            translate([0,0,-1]) cylinder(d=58,$fn=64,h=12);
            translate([pulley_distance2,0,-1]) cylinder(d=10,$fn=64,h=12);
            }
        }
    }


module reducer1()
 {

difference()
     {
     union()
     {
 
 hull()
         {
 hull()
{
    translate([0,37,-20])  cylinder(d=12,h=7,$fn=128);
    translate([0,-37,-20])  cylinder(d=12,h=7,$fn=128);
    }

             
         
translate([pulley_distance2,0,-20])
       hull()  
         {
         translate([0,0,0])  cylinder(d=12,h=7,$fn=128);
             translate([13,13,0])  cylinder(d=12,h=7,$fn=128);
         translate([13,-13,0])  cylinder(d=12,h=7,$fn=128);
             }



hull()
         {
translate([0,0,-20]) cylinder(d=36.8,h=7,$fn=128);
translate([pulley_distance2,0,-20])  cylinder(d=36.8,h=7,$fn=128);
         }

translate([pulley_distance2,0,-20]) 
{

hull()
    {
translate([1.5,0,0])  rotate([0,0,45]) {    
translate([26.1/2,26.1/2,0]) cylinder(d=9,$fn=32,h=7);
translate([26.1/2,-26.1/2,0]) cylinder(d=9,$fn=32,h=7);
translate([-26.1/2,26.1/2,0]) cylinder(d=9,$fn=32,h=7); 
translate([-26.1/2,-26.1/2,0]) cylinder(d=9,$fn=32,h=7);    
}
translate([-1.5,0,0])  rotate([0,0,45]) {    
translate([26.1/2,26.1/2,0]) cylinder(d=9,$fn=32,h=7);
translate([26.1/2,-26.1/2,0]) cylinder(d=9,$fn=32,h=7);
translate([-26.1/2,26.1/2,0]) cylinder(d=9,$fn=32,h=7); 
translate([-26.1/2,-26.1/2,0]) cylinder(d=9,$fn=32,h=7);    
}
}




}
}
                        translate([pulley_distance2,0,5])
             hull()
             {
                          translate([13,13,0])  cylinder(d=12,h=5,$fn=128);
         translate([13,-13,0])  cylinder(d=12,h=5,$fn=128);
             }

             translate([pulley_distance2,0,-20])
             {
                          translate([13,13,0])  cylinder(d=12,h=30,$fn=128);
         translate([13,-13,0])  cylinder(d=12,h=30,$fn=128);
             }
        translate([0,37,-20])  cylinder(d=12,h=30,$fn=128);
    translate([0,-37,-20])  cylinder(d=12,h=30,$fn=128);
}

translate([pulley_distance2+13,13,-20])  cylinder(d=6.5,h=27,$fn=6);
translate([pulley_distance2+13,-13,-20])  cylinder(d=6.5,h=27,$fn=6);       
translate([pulley_distance2+13,13,7.3])  cylinder(d=3.2,h=50,$fn=128);
translate([pulley_distance2+13,-13,7.3])  cylinder(d=3.2,h=30,$fn=128);


translate([0,37,-20])  cylinder(d=6.5,h=27,$fn=6);
translate([0,-37,-20])  cylinder(d=6.5,h=27,$fn=6);       
translate([0,37,7.3])  cylinder(d=3.2,h=50,$fn=128);
translate([0,-37,7.3])  cylinder(d=3.2,h=30,$fn=128);

hull()
{translate([0,12.5,-20])  cylinder(d=3.2,h=50,$fn=128);
translate([0,-12.5,-20])  cylinder(d=3.2,h=30,$fn=128);       
    }

 translate([pulley_distance2+15,0,5])  cylinder(d=3.2,h=8,$fn=128);
translate([0,0,-20])  cylinder(d=22.2,h=8,$fn=128);

translate([pulley_distance2,0,-20])  union()
{
    hull()
{
translate([1.5,0,0])  cylinder(d=22,$fn=64,h=7);
translate([-1.5,0,0])  cylinder(d=22,$fn=64,h=7);    
}

rotate([0,0,45])  
translate([26.1/2,26.1/2,0])
{
rotate([0,0,-45]) hull()
{
   translate([1.5,0,0])  cylinder(d=3,$fn=32,h=7);
   translate([-1.5,0,0])  cylinder(d=3,$fn=32,h=7);        
    }
rotate([0,0,-45]) hull()
{
   translate([1.5,0,3])  cylinder(d=6.6,$fn=32,h=7);
   translate([-1.5,0,3])  cylinder(d=6.6,$fn=32,h=7);        
    }    
}
rotate([0,0,45])  
translate([-26.1/2,26.1/2,0])
{
rotate([0,0,-45]) hull()
{
   translate([1.5,0,0])  cylinder(d=3,$fn=32,h=7);
   translate([-1.5,0,0])  cylinder(d=3,$fn=32,h=7);        
    }
rotate([0,0,-45]) hull()
{
   translate([1.5,0,3])  cylinder(d=6.6,$fn=32,h=7);
   translate([-1.5,0,3])  cylinder(d=6.6,$fn=32,h=7);        
    }    
}
rotate([0,0,45])  
translate([26.1/2,-26.1/2,0])
{
rotate([0,0,-45]) hull()
{
   translate([1.5,0,0])  cylinder(d=3,$fn=32,h=7);
   translate([-1.5,0,0])  cylinder(d=3,$fn=32,h=7);        
    }
rotate([0,0,-45]) hull()
{
   translate([1.5,0,3])  cylinder(d=6.6,$fn=32,h=7);
   translate([-1.5,0,3])  cylinder(d=6.6,$fn=32,h=7);        
    }    
}
rotate([0,0,45])  
translate([-26.1/2,-26.1/2,0])
{
rotate([0,0,-45]) hull()
{
   translate([1.5,0,0])  cylinder(d=3,$fn=32,h=7);
   translate([-1.5,0,0])  cylinder(d=3,$fn=32,h=7);        
    }
rotate([0,0,-45]) hull()
{
   translate([1.5,0,3])  cylinder(d=6.6,$fn=32,h=7);
   translate([-1.5,0,3])  cylinder(d=6.6,$fn=32,h=7);        
    }    
}
    }

}

%translate([pulley_distance2,0,-20-35/2]) rotate([0,0,45]) cube([35,35,35],center=true);

%translate([0,0,1]) color("black",.7) belt();
%translate([0,0,-10]) rotate([0,0,0])  import("pulleyBigGT2.stl"); 
%translate([0,0,40]) import("pulleySmall.stl"); 
//translate([0,0,10]) cylinder(d=22,h=7+7);

%translate([pulley_distance2,0,0]) import("pulleySmallGT2.stl");
}

module reducer2()
{
    difference()
     {
     union()
     {
    hull()
{         
translate([47.2/2,47.2/2,10]) cylinder(d=9.2+3,$fn=64,h=10);
translate([47.2/2,-47.2/2,10]) cylinder(d=9.2+3,$fn=64,h=10); 
translate([-47.2/2,47.2/2,10]) cylinder(d=9.2+3,$fn=64,h=10); 
translate([-47.2/2,-47.2/2,10]) cylinder(d=9.2+3,$fn=64,h=10);
translate([pulley_distance2+13,13,10])  cylinder(d=12,h=10,$fn=128);
translate([pulley_distance2+13,-13,10])  cylinder(d=12,h=10,$fn=128);
    translate([0,37,10])  cylinder(d=12,h=10,$fn=128);
    translate([0,-37,10])  cylinder(d=12,h=10,$fn=128);    
}

translate([0,0,20]) cylinder(d=37,$fn=64,h=10);
}

translate([pulley_distance2+13,13,17])  cylinder(d=6.5,h=27,$fn=128);
translate([pulley_distance2+13,-13,17])  cylinder(d=6.5,h=27,$fn=128);       
translate([pulley_distance2+13,13,10])  cylinder(d=3.2,h=30,$fn=128);
translate([pulley_distance2+13,-13,10])  cylinder(d=3.2,h=30,$fn=128); 


translate([0,37,17])  cylinder(d=6.5,h=27,$fn=128);
translate([0,37,10])  cylinder(d=3.2,h=30,$fn=128);
translate([0,-37,17])  cylinder(d=6.5,h=27,$fn=128);
translate([0,-37,10])  cylinder(d=3.2,h=30,$fn=128);


translate([0,0,-20]) cylinder(d=22.2,$fn=64,h=99);     
{         
#translate([47.2/2,47.2/2,14.3]) cylinder(d=4.2,$fn=64,h=10);
translate([47.2/2,-47.2/2,14.3]) cylinder(d=4.2,$fn=64,h=10); 
translate([-47.2/2,47.2/2,14.3]) cylinder(d=4.2,$fn=64,h=10); 
translate([-47.2/2,-47.2/2,14.3]) cylinder(d=4.2,$fn=64,h=10);
translate([47.2/2,47.2/2,4]) cylinder(d=10,$fn=64,h=10);
translate([47.2/2,-47.2/2,4]) cylinder(d=10,$fn=64,h=10); 
translate([-47.2/2,47.2/2,4]) cylinder(d=10,$fn=64,h=10); 
translate([-47.2/2,-47.2/2,4]) cylinder(d=10,$fn=64,h=10);         
}
}
    }


reducer2();
reducer1();