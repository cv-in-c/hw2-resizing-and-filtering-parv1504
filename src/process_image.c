#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    int a,b,d;
    if(x<0){
        a=0;
    }
    else if(x>im.w){
        a=(im.w)-1;
    }
    else{
        a=x;
    }
    if(y<0){
        b=0;
    }
    else if(y>im.h){
        b=(im.h)-1;
    }
    else{
        b=y;
    }
    if(c<0){
        d=0;
    }
    else if(c>im.c){
        d=(im.c)-1;
    }
    else{
        d=c;
    }
    return im.data[a+b*(im.w)+d*(im.w)*(im.h)];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    int a,b,d;
    if(x<0){
        return;
    }
    else if(x>im.w){
        return;
    }
    else{
        a=x;
    }
    if(y<0){
        return;
    }
    else if(y>im.h){
        return;
    }
    else{
        b=y;
    }
    if(c<0){
        return;
    }
    else if(c>im.c){
        return;
    }
    else{
        d=c;
    }
    im.data[a+b*(im.w)+d*(im.w)*(im.h)]=v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    copy.data= (float*) malloc((im.w*im.h*im.c)* sizeof(float));
    for(int i=0;i<(im.w*im.h*im.c);i++){
        copy.data[i]=im.data[i];
    }
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    for(int i=0;i<im.h;i++){
        for(int j=0;j<im.w;j++){
            float a=get_pixel(im,j,i,0);
            float b=get_pixel(im,j,i,1);
            float c=get_pixel(im,j,i,2);
            float sum=(0.299*a+0.587*b+0.114*c);
            set_pixel(im,j,i,0,sum);
        }   
    }      
    return gray;
}

void shift_image(image im, int c, float v)
{
    assert((c>=0)&&(c< im.c));
    for (int i=0;i<im.h;i++){
        for (int j=0;j<im.w;j++){
            float curr=get_pixel(im,j,i,c);
            curr=curr+v;
            set_pixel(im,j,i,c,curr);
        }
    }
}

void clamp_image(image im)
{
    for(int i=0;i<((im.w)*(im.h)*(im.c));i++){
        if(im.data[i]<0){
            im.data[i]=0;
        }
        if(im.data[i]>1){
            im.data[i]=1;
	  }
    }
}

// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}
void rgb_to_hsv(image im)
{
    float h, s, v;
    float min_val, max_val, delta;
    for (int i=0;i<im.h;i++) {
        for (int j=0;j<im.w;j++) {
            float r= get_pixel(im,j,i,0);
            float g= get_pixel(im,j,i,1);
            float b= get_pixel(im,j,i,2);
            float maxm= fmaxf(r,fmaxf(g,b));
            float minm= fminf(r,fminf(g,b));
            float diff= maxm - minm;
            v=maxm;
            if(maxm==0){
                s=0;
            }
            else{
                s=diff/maxm;
            }
            if(diff==0){
                h=0;
            }
            else if(maxm==r){
                h=(g-b)/diff;
            }
            else if(maxm==g) {
                h=((b-r)/diff)+2;
            }
            else if(maxm == b) {
                h=((r-g)/diff)+4;
            }
            if(h<0){
                h=(h/6)+1;
            }
            else{
                h=h/6;
            }
            im.data[j+i*im.w] = h;
            im.data[j+(i*im.w)+(im.h*im.w)] = s;
            im.data[j+(i*im.w)+(2*im.h*im.w)] = v;
        }
    }
}
void hsv_to_rgb(image im)
{
	for(int i = 0; i < im.h; i++){
		for(int j = 0; j < im.w; j++){
            float r, g, b;
			float h=get_pixel(im, j, i, 0);
			float s=get_pixel(im, j, i, 1);
			float v=get_pixel(im, j, i, 2);
			float h1=h*6;
            float prod=s*v;
			float diff=v-prod;
			if(prod== 0){
				r = v;
				g = v;
				b = v;
			}
                else if(h1==1) {
                      r = v;
                      g = v;
                      b = diff;
                  }
                  else if(h1==2){
                      r = diff;
                      g = v;
                      b = diff;
                  }
                  else if (h1==3){
                      r = diff;
                      g = v;
                      b = v;
                  }
                  else if(h1==4){
                      r = diff;
                      g = diff;
                      b = v;
                  }
                  else if(h1==5){
                      r = v;
                      g = diff;
                      b = v;
                  }
                  else if((h1<1)&&(h1>0)){
                      r = v;
                      b = diff;
                      g = (h1*prod)+b;
                  }
                  else if((h1<2)&&(h1>1)){
                      r = (((h1-2)*prod)-b)*(-1);
                      g = v;
                      b = prod;
                  }
                  else if((h1<3)&&(h1>2)){
                      r = diff;
                      g = v;
                      b = ((h1-2)*prod)+r;
                  }
                  else if((h1<4)&&(h1>3)){
                      r = diff;
                      g = (((h1-4)*prod)-r)*(-1);
                      b = v;
                  }
                  else if((h1<5)&&(h1>4)){
                      r = (h1-4)*prod+g;
                      g = diff;
                      b = v;
                  }
                  else if ((h1<6)&&(h1>5)){
                      r = v;
                      g = diff;
                      b = ((((h1/6)-1)*6*prod)-g)*(-1);
                  }
                  else {
                      r = v;
                      g = diff;
                      b = diff;
                 }

			set_pixel(im,j,i,0,r);
			set_pixel(im,j,i,1,g);
			set_pixel(im,j,i,2,b);
		}
	}
}