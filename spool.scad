module spool(r=10, h=20)
{
    end_h_div = 2;
    end_r_mult = 2;
    translate([0,0,h/2*-1])
    {
        difference()
        {
            union()
            {
                translate([0,0,h-0.01])
                {
                    cylinder(h=h/end_h_div, r1=r, r2=r*end_r_mult);
                }
                cylinder(h=h, r=r);
                translate([0,0,(h/end_h_div-0.01)*-1])
                {
                    cylinder(h=h/end_h_div, r2=r, r1=r*end_r_mult);
                }
            }
            translate([0,0,(h/end_h_div-0.01)*-1])
            {
                DPD20_axle_shape();
            }
            translate([0,0,h+(h/end_h_div-0.01)])
            {
                rotate([0,180,0])
                {
                    DPD20_axle_shape();
                }
            }
            /**
             * This might not be a good idea
             *
            translate([0,0,(h/4-0.01)*-1])
            {
                cylinder(h=h+h/2, r=r/2);
            }
            */
        }
    }
}

module DPD20_axle_shape()
{
    scale_factor = 1.2;
    scale([scale_factor,scale_factor,scale_factor])
    {
        difference()
        {
            translate([0,0,7.6-8.5])
            {
                cylinder(h=7.6, r=5.4/2);
            }
            translate([3.6/2,-3,-0.01])
            {
                cube([3,6,8]);
            }
            translate([(3.6/2+3)*-1,-3,-0.01])
            {
                cube([3,6,8]);
            }
        }
    }
}

//spool();
//DPD20_axle_shape();

module spool_pieces(r=10, h=20)
{
    translate([r*2.2,0,0])
    {
        difference()
        {
            spool(r,h);
            translate([-2*r,-2*r,0])
            {
                cube(r*4,r*4,h*1.1*-1);
            }
            translate([-1*r/4,-1*r/4,-h/4+0.01])
            {
                cube(r/2*1.05, r/2*1.05, h/4*1.05);
            }
        }
    }
    translate([r*-2.2,0,0])
    {
        union()
        {
            difference()
            {
                rotate([0,180,0])
                {
                    spool(r,h);
                }
                translate([-2*r,-2*r,0])
                {
                    cube(r*4,r*4,h*1.1*-1);
                }
            }
            translate([-1*r/4*0.9,-1*r/4*0.9,-0.01])
            {
                cube(r/2*0.95, r/2*0.95, h/4*0.95);
            }
        }
    }
}

spool_pieces();
//spool();

