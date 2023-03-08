
/*



 $ gcc jpegtest.c -std=c11 -ljpeg

 *********************************/

#include <stdio.h>
#include<math.h>
#include <stdlib.h>

#include <stdbool.h>

#include <stdint.h>

#include <jpeglib.h>
#include<time.h>

#include <unistd.h>

// JPEG image struct

typedef struct {
	uint8_t *data;   // raw data
	uint32_t width;
	uint32_t height;
	uint32_t ch;     // color channels
} JpegData;



// allocate memory for raw data

void alloc_jpeg(JpegData *jpegData)

{
		int i;
    jpegData->data = NULL;

   jpegData->data = (uint8_t*) malloc(sizeof(uint8_t)  *

                                       jpegData->width  *

                                       jpegData->height *

                                       jpegData->ch);

	  //jpegData->data=(JSAMPARRAY)malloc(sizeof(JSAMPROW) * jpegData->height*3*jpegData->width*sizeof(JSAMPLE));
/*		for(i=0;i<jpegData->height;i++){
			jpegData->data[i]=(JSAMPROW) malloc(sizeof(JSAMPLE) * 3 * jpegData->width);
		}*/
}



// free memory for raw data

void free_jpeg(JpegData *jpegData)

{

    if (jpegData->data != NULL) {

        free(jpegData->data);

        jpegData->data = NULL;

    }

}



// read JPEG image

// 1. create JPEG decompression object

// 2. specify source data

// 3. read JPEG header

// 4. set parameters

// 5. start decompression

// 6. scan lines

// 7. finish decompression

bool read_jpeg(JpegData *jpegData,

              const char *srcfile,

              struct jpeg_error_mgr *jerr)

{

    // 1.

    struct jpeg_decompress_struct cinfo;

    jpeg_create_decompress(&cinfo);

    cinfo.err = jpeg_std_error(jerr);



    FILE *fp = fopen(srcfile, "rb");

    if (fp == NULL) {

        printf("Error: failed to open %s\n", srcfile);

        return false;

    }

    // 2.

    jpeg_stdio_src(&cinfo, fp);



    // 3.

    jpeg_read_header(&cinfo, TRUE);



    // 4. omitted

    // 5.

    jpeg_start_decompress(&cinfo);



    jpegData->width  = cinfo.image_width;

    jpegData->height = cinfo.image_height;

    jpegData->ch     = cinfo.num_components;



    alloc_jpeg(jpegData);



    // 6. read line by line

    uint8_t *row = jpegData->data;

    const uint32_t stride = jpegData->width * jpegData->ch;

    for (int y = 0; y < jpegData->height; y++) {

        jpeg_read_scanlines(&cinfo, &row, 1);

        row += stride;

    }
    // 7.
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);
    return true;
}



// write JPEG image

// 1. create JPEG compression object

// 2. specify destination data

// 3. set parameters

// 4. start compression

// 5. scan lines

// 6. finish compression

bool write_jpeg(const JpegData *jpegData,

                const char *dstfile,

                struct jpeg_error_mgr *jerr)

{

    // 1.

    struct jpeg_compress_struct cinfo;

    jpeg_create_compress(&cinfo);

    cinfo.err = jpeg_std_error(jerr);



    FILE *fp = fopen(dstfile, "wb");

    if (fp == NULL) {

        printf("Error: failed to open %s\n", dstfile);

        return false;

    }

    // 2.

    jpeg_stdio_dest(&cinfo, fp);



    // 3.

    cinfo.image_width      = jpegData->width;

    cinfo.image_height     = jpegData->height;

    cinfo.input_components = jpegData->ch;

    cinfo.in_color_space   = JCS_RGB;

    jpeg_set_defaults(&cinfo);
		//jpeg_set_quality(&cinfo, 50, TRUE);




    // 4.

    jpeg_start_compress(&cinfo, TRUE);



    // 5.

    uint8_t *row = jpegData->data;

    const uint32_t stride = jpegData->width * jpegData->ch;


    for (int y = 0; y < jpegData->height; y++) {

        jpeg_write_scanlines(&cinfo, &row, 1);

        row += stride;

    }



    // 6.

    jpeg_finish_compress(&cinfo);

    jpeg_destroy_compress(&cinfo);

    fclose(fp);



    return true;

}

void plot_calculate(uint32_t x,uint32_t y,JpegData *plot_img){
	int j;	
	y=(plot_img->height)-y;
	for(j=0;j<plot_img->ch;j++){
		//printf("\nx=%d,y=%d\n",x,y);
		plot_img->data[(int)((y*(plot_img->width)+x)*(plot_img->ch)+j)]=100;	
	}
}
void draw_rectangle(uint32_t x,uint32_t y,uint32_t width,uint32_t height,JpegData *plot_img){
	uint32_t i,out_x,out_y;
	for(i=0;i<(width*height);i++){
		out_x=(i)%width+x;
		out_y=(i)/width+y;
		plot_calculate(out_x,out_y,plot_img);
	}
}
//right :0 left 1
//up 1 down -1
void draw_triangle(uint32_t x,uint32_t y,uint32_t width,uint32_t height,JpegData *plot_img,uint32_t right_left,uint32_t up_down){
		uint32_t i,j;
		uint32_t line_width;
		for(i=0;i<height;i++){
//			for(j=0;j<width;j++){
				printf("\nok%d,%d\n",i,j);
				line_width=width;
				line_width=line_width/2*(up_down+1)-(uint32_t)((uint32_t)(((double)width/(double)height)*i)*(up_down));
				printf("\nline%d,%d\n",line_width,up_down);
				draw_rectangle(x+((right_left*-line_width)),(y+i),line_width,1,plot_img);
	//		}
		}
}
//right :0 left 1
//up 1 down -1
void draw_two_triangle(uint32_t top_x,uint32_t top_y,uint32_t width,uint32_t height,uint32_t left,uint32_t right,JpegData *plot_img,uint32_t up_down){
		draw_triangle(top_x,(top_y-height),(width-(top_x-left)),height,plot_img,0,up_down);
		printf("\ngo\n");
		draw_triangle(top_x,(top_y-height),(width-(right-top_x)),height,plot_img,1,up_down);
}
void draw_four_triangle(uint32_t top_x,uint32_t top_y,uint32_t middle_y,uint32_t bottom_x,uint32_t bottom_y,uint32_t width,uint32_t height,uint32_t left,uint32_t right,JpegData *plot_img){
		draw_two_triangle(top_x,top_y,width,(uint32_t)(top_y-middle_y),left,right,plot_img,1);
		draw_two_triangle(bottom_x,middle_y,width,(uint32_t)(middle_y-bottom_y),left,right,plot_img,-1);
	
}
/*
void draw_slope_up(uint32_t begin_x,uint32_t begin_y,uint32_t end_x,uint32_t end_y,uint32_t width,JpegData *plot_img){
		uint32_t i,j,k,draw_slope_plot;	
		j=begin_y;
		for(i=begin_x;i<end_x;i++){
//				for(j=slope_begin_y;j<slope_end_y;j++){
						for(k=0;k<width;k++){
//							plot_calculate(uint32_t x,uint32_t y,JpegData *plot_img){
							draw_slope_plot=i+k;
							if(draw_slope_plot<720){
								plot_calculate(draw_slope_plot,j,plot_img);
							}
						}
//				}
			j=i+begin_y;
		}
}*/
void draw_slope(uint32_t begin_x,uint32_t begin_y,uint32_t end_x,uint32_t end_y,uint32_t width,JpegData *plot_img){
		uint32_t i,j,k,draw_slope_plot,up_down,change_x,change_y;	
		double slope_angle;
		if(begin_x>end_x){
			change_x=begin_x;
			begin_x=end_x;
			end_x=change_x;
			change_y=begin_y;
			begin_y=end_y;
			end_y=change_y;
		}
		j=begin_y;
		if(begin_y<end_y){
			slope_angle=(double)(((double)((double)(end_y)-(double)(begin_y)))/((double)(end_x)-(double)(begin_x)));
			//up_down=1;
		}else{
			slope_angle=(double)(((double)((double)(end_y)-(double)(begin_y)))/((double)(end_x)-(double)(begin_x)));
			//up_down=-1;
		}
		printf("\nslope=%lf\n",slope_angle);
		if((int)(end_x)-(int)(begin_x)){
			for(i=begin_x;i<end_x;i++){
				j=(uint32_t)((double)(i-begin_x)*(slope_angle)+(double)(begin_y));
				if((j>0)&&(j<plot_img->height)){
//				for(j=slope_begin_y;j<slope_end_y;j++){
						for(k=0;k<width;k++){
//							plot_calculate(uint32_t x,uint32_t y,JpegData *plot_img){
							draw_slope_plot=i+k;
							printf("\ndraw_slope_plot=%d\n",(int)(draw_slope_plot));
							if((int)(draw_slope_plot)<(int)(plot_img->width)){
							printf("\nx=%dy=%d\n",draw_slope_plot,j);
								plot_calculate(draw_slope_plot,j,plot_img);
								printf("\n2:x=%dy=%d\n",draw_slope_plot,j);
							}
						}
					}
//				}
			}
		}else{
			if(begin_y>end_y){
				change_y=begin_y;
				begin_y=end_y;
				end_y=change_y;
			}
			for(j=begin_y;j<end_y;j++){
				i=begin_x;
				for(k=0;k<width;k++){
							draw_slope_plot=i+k;
							printf("\ndraw_slope_plot=%d\n",(int)(draw_slope_plot));
							if((int)(draw_slope_plot)<(int)(plot_img->width)){
								plot_calculate(draw_slope_plot,j,plot_img);
							}

				}
			}
		}
			printf("\nok\n");
}

void draw_quadratic(JpegData *plot_img){
	uint32_t i,j,k,draw_plot_curve;
	for(i=0;i<(plot_img->height*plot_img->height*20);i++){
		i=i;
		j=(uint32_t)((sqrt((double)(i)))*20);
		if((i>0)&&(i<(plot_img->height))){
			for(k=0;k<100;k++){
				draw_plot_curve=j+k;
				if((draw_plot_curve<(plot_img->height))&&(draw_plot_curve>0)){
					plot_calculate(draw_plot_curve,i,plot_img);
				}
			}
		}
	}
}
void set_point_angle(uint32_t point_num,uint32_t *x,uint32_t *y,JpegData *plot_img){
	uint32_t address,encode,i,j,buffer_point=0,origin=0,chenge=0,LR_point[2],k;
	uint32_t *point;
/*	for(i=0;i<point_num;i++){
		printf("\n%d\n",(int)((((plot_img->height)-y[i])*(plot_img->width))+x[i]));
	}
//		point=malloc(sizeof(uint32_t)*point_num);
		point=(uint32_t*) malloc(sizeof(uint32_t)*point_num);
	for(i=0;i<point_num;i++){
		point[i]=(((plot_img->height)-y[i])*(plot_img->width))+x[i];	
	}
		printf("\nok\n");
	for(i=0;(int)(i)<(int)(point_num);i++){
		buffer_max=point[(int)(i)];
		for(j=i;(int)(j)<(int)(point_num);j++){
			if(((int)(point[(int)(j)]))>((int)(buffer_max))){
				buffer_max=point[(int)(j)];
				origin=j;
			}	
		}
		printf("\nok%d,%d\n",(int)(i),(int)(buffer_max));
		chenge=point[(int)(i)];
		point[(int)(i)]=buffer_max;
		point[(int)(origin)]=chenge;
	}*/
	for(i=0;(int)(i)<(int)(point_num);i++){	
		printf("\ni=%d\n",(int)(point_num));
		buffer_point=(uint32_t)(((int)(i+1))%(int)(point_num));
		printf("\nbuffur=%d\n",y[(int)(buffer_point)]);
	
 		draw_slope(x[i],y[i],x[(int)(buffer_point)],y[(int)(buffer_point)],(uint32_t)(1),plot_img);
	}
	printf("\nok2\n");
	for(i=1;(((int)(i))<=((int)(plot_img->height)));i++){
		//printf("\n%d\n",encode);
	  encode=(plot_img->height)-i;
		k=0;
		for(j=0;j<(plot_img->width);j++){
       address=(uint32_t)(plot_img->data[(int)((encode*(plot_img->width)+j)*(plot_img->ch))]);
		  if((int)(address)==(int)(100)){
				if(k==0){
					LR_point[0]=j;
				}else if(k>=1){
					LR_point[1]=j;
				}
				k++;
			}
		}
//		printf("\nok3=%d\n",k);
		if((int)(k)>1){
			printf("\nLR=%d,%d,i=%d\n",LR_point[0],LR_point[1],i);
			draw_rectangle(LR_point[0],i,(LR_point[1]-LR_point[0]),1,plot_img);
		}
	//	printf("\nok4\n");
	}
}
void curve(uint32_t center_x,uint32_t center_y,uint32_t center_r,JpegData *plot_img){
	uint32_t y,i,j[2],k,draw_plot_curve[2],buffer_point;
	double curve;
	for(i=0;i<(plot_img->height);i++){
		curve=((double)(center_r*center_r)-(double)((i-center_y)*(i-center_y)));
		if(curve<0){
			curve=-curve;
		}
		j[0]=(uint32_t)((sqrt(curve)))+(uint32_t)(center_x);
		j[1]=(uint32_t)(-(sqrt(curve)))+(uint32_t)(center_x);
		y=i;
		if(((int)(y)>0)&&(y<(int)(center_y+center_r))&&((int)(y)>(int)(center_y-center_r))){
			//for(k=0;k<10;k++){
				draw_plot_curve[0]=j[0];
				draw_plot_curve[1]=j[1];
				if((int)(draw_plot_curve[0])<(int)(draw_plot_curve[1])){
					buffer_point=draw_plot_curve[0];
					draw_plot_curve[0]=draw_plot_curve[1];
					draw_plot_curve[1]=buffer_point;
				}
				printf("\n%d>%d\n",draw_plot_curve[0],draw_plot_curve[1]);

			if((draw_plot_curve[0]<(plot_img->width))&&(draw_plot_curve[0]>0)){
				if((draw_plot_curve[1]<(plot_img->width))&&(draw_plot_curve[1]>0)){
					printf("\nb1\n");
					draw_rectangle(draw_plot_curve[1],y,(draw_plot_curve[0]-draw_plot_curve[1]),1,plot_img);
				}else{
					printf("\nb2\n");
					draw_rectangle(0,y,(draw_plot_curve[0]),1,plot_img);
				}
			}else{
				if((draw_plot_curve[1]<(plot_img->width))&&(draw_plot_curve[1]>0)){
					printf("\nb3\n");
					draw_rectangle(draw_plot_curve[1],y,((plot_img->width-1)-draw_plot_curve[1]),1,plot_img);
				}else{
					printf("\nb4\n");
					draw_rectangle(0,y,(plot_img->width-1),1,plot_img);
				}
			}
			//	if((draw_plot_curve[0]<720)&&(draw_plot_curve[0]>0)){
			//		plot_calculate(draw_plot_curve[0],y,plot_img);
			//	}
			//	if((draw_plot_curve[1]<720)&&(draw_plot_curve[1]>0)){
			//		plot_calculate(draw_plot_curve[1],y,plot_img);
			//	}
			//}
		}
	}
}
int main(void)
{

    JpegData jpeg_out_data;

		struct jpeg_compress_struct outcinfo;

    struct jpeg_error_mgr jerr,outjerr;



    // src/dst file

    char *src = "./lena.jpg";

    char *dst = "./out.jpg";
		uint32_t x,y,four_x,four_y;
		uint32_t i,j,k;
		uint32_t point_set_x[3],point_set_y[3];
		uint32_t four_width,four_height;
		uint32_t triangle_width,triangle_height;
		uint32_t triangle_x,triangle_y,line_width;
		uint32_t two_triangle_width,two_triangle_height;
		uint32_t two_triangle_top_x,two_triangle_top_y,two_triangle_right,two_triangle_left;
		uint32_t four_triangle_width,four_triangle_height;
		uint32_t four_triangle_top_x,four_triangle_top_y,four_triangle_middle_y,four_triangle_bottom_x,four_triangle_bottom_y,four_triangle_right,four_triangle_left;
		uint32_t slope_up_begin_x,slope_up_begin_y,slope_up_end_x,slope_up_end_y,slope_up_width,draw_slope_up_plot;
		uint32_t slope_down_begin_x,slope_down_begin_y,slope_down_end_x,slope_down_end_y,slope_down_width,draw_slope_down_plot;
		uint32_t angle_curve_x,angle_curve_y,angle_curve_r;
/*    if (!read_jpeg(&jpegData, src, &jerr)){

        free_jpeg(&jpegData);

        return -1;

    }*/

    //printf("Read:  %s\n", src);




    // reverse all bits

   // int size = jpegData.width * jpegData.height * jpegData.ch;
//    int diff[2]={0,0};
 //   int sub;
		

		jpeg_out_data.height=1800;
		jpeg_out_data.width=3200;
		jpeg_out_data.ch=3;

		alloc_jpeg(&jpeg_out_data);

//		printf("h=%d,w=%d",jpegData.height,jpegData.width);
		for(i=0;i<jpeg_out_data.height;i++){
			for(j=0;j<jpeg_out_data.width;j++){
///			printf("\n%d\n",j);
				for(k=0;k<jpeg_out_data.ch;k++){
					//jpeg_out_data.data[(i*jpegData.width+j)*jpegData.ch+k]=jpegData.data[(i*jpegData.width+j)*jpegData.ch+k];
				//	printf("\n%d\n",jpegData.data[i*jpegData.width+j*jpegData.ch+k]);
//				jpeg_out_data.data[((jpeg_out_data.height-j)*jpeg_out_data.width+i)*jpegData.ch+k]=jpegData.data[(i*jpegData.width+j)*jpegData.ch+k];
//				jpeg_out_data.data[((jpeg_out_data.height-j)*jpeg_out_data.width+i)*jpegData.ch+k]=jpegData.data[(i*jpegData.width+j)*jpegData.ch+k];
									
					jpeg_out_data.data[(uint32_t)((i*jpeg_out_data.width+j)*jpeg_out_data.ch+k)]=255;
				}
			}
    }
		four_width=300;
		four_height=200;
		four_x=200;
		four_y=100;
//		draw_rectangle(four_x,four_y,four_width,four_height,&jpeg_out_data);
		triangle_x=300;
		triangle_y=700;
		triangle_width=100;
		triangle_height=100;
	//	draw_triangle(triangle_x,triangle_y,triangle_width,triangle_height,&jpeg_out_data,0,-1);
		printf("\nok\n");
		two_triangle_top_x=30;
		two_triangle_top_y=200;
		two_triangle_right=100;
		two_triangle_left=0;
		two_triangle_width=100;
		two_triangle_height=100;
//		draw_two_triangle(two_triangle_top_x,two_triangle_top_y,two_triangle_width,two_triangle_height,two_triangle_left, two_triangle_right,&jpeg_out_data,-1);
		four_triangle_top_x=30;
		four_triangle_top_y=800;
		four_triangle_bottom_x=60;
		four_triangle_bottom_y=700;
		four_triangle_middle_y=750;
		four_triangle_right=100;
		four_triangle_left=0;
		four_triangle_width=100;
		four_triangle_height=100;
	//	draw_four_triangle(four_triangle_top_x,four_triangle_top_y,four_triangle_middle_y,four_triangle_bottom_x,four_triangle_bottom_y,four_triangle_width,four_triangle_height,four_triangle_left, four_triangle_right,&jpeg_out_data);	
		slope_up_begin_x=15000;
		slope_up_begin_y=0;
		slope_up_end_x=0;
		slope_up_end_y=8000;
		slope_up_width=100;
//		draw_slope(slope_up_begin_x,slope_up_begin_y,slope_up_end_x,slope_up_end_y,slope_up_width,&jpeg_out_data);
		slope_down_begin_x=300;
		slope_down_begin_y=720;
		slope_down_end_x=720;
		slope_down_end_y=0;
		slope_down_width=100;
		angle_curve_x=100;
		angle_curve_y=300;
		angle_curve_r=100;
		//curve(angle_curve_x,angle_curve_y,angle_curve_r,&jpeg_out_data);
		point_set_x[0]=1250;
		point_set_y[0]=1300;
		point_set_x[1]=1300;
		point_set_y[1]=1200;
		point_set_x[2]=0;
		point_set_y[2]=0;
	set_point_angle(3,&point_set_x[0],&point_set_y[0],&jpeg_out_data);
	//	draw_slope(slope_down_begin_x,slope_down_begin_y,slope_down_end_x,slope_down_end_y,slope_down_width,&jpeg_out_data);
		//draw_quadratic(&jpeg_out_data);
		/*
		for(i=slope_begin_x;i<slope_end_x;i++){
//				for(j=slope_begin_y;j<slope_end_y;j++){
						for(k=0;k<slope_width;k++){
//							plot_calculate(uint32_t x,uint32_t y,JpegData *plot_img){
							draw_slope_plot=i+k;
							if(draw_slope_plot<720){
								plot_calculate(draw_slope_plot,i,&jpeg_out_data);
							}
						}
//				}
		}*/
//		draw_two_triangle(four_triangle_top_x,four_triangle_top_y,four_triangle_width,(four_triangle_top_y-four_triangle_middle_y),four_triangle_left, four_triangle_right,&jpeg_out_data,1);
//		draw_two_triangle(four_triangle_bottom_x,four_triangle_middle_y,four_triangle_width,(four_triangle_middle_y-four_triangle_bottom_y),four_triangle_left, four_triangle_right,&jpeg_out_data,-1);
		
		//draw_triangle(two_triangle_top_x,(two_triangle_top_y-two_triangle_height),(two_triangle_width-(two_triangle_right-two_triangle_top_x)),two_triangle_height,&jpeg_out_data,1,1);
//		draw_triangle(two_triangle_top_x,(two_triangle_top_y-two_triangle_height),(two_triangle_width-(two_triangle_top_x-two_triangle_left)),two_triangle_height,&jpeg_out_data,0,1);
//		draw_triangle(two_triangle_top_x,(two_triangle_y-two_triangle_height),(two_triangle_width-),triangle_height,&jpeg_out_data);
	//	draw_triangle(triangle_x,triangle_y,triangle_width,triangle_height,&jpeg_out_data);
/*		for(i=0;i<triangle_height;i++){
			for(j=0;j<triangle_width;j++){
				line_width=triangle_width;
				line_width=line_width-(triangle_width/triangle_height)*i;
				draw_rectangle(triangle_x,(triangle_y+i),line_width,1,&jpeg_out_data);
			}
		}*/
	//	for(i=0;i<four_width*four_height;i++){
/*					x[i]=i%jpeg_out_data.width;
			y[i]=jpeg_out_data.height-(i/jpeg_out_data.width);
					for(j=0;j<jpegData.ch;j++){
						jpeg_out_data.data[(y[i]*jpeg_out_data.width+x[i])*jpegData.ch+j]=0;	
					}
					*/
		//	x=(i)%four_width+four_x;
	//		y=(i)/four_width+four_y;
//			plot_calculate(x,y,&jpeg_out_data);
		//}
		//		printf("\nok%d,%d\n",i,j);
    if (!write_jpeg(&jpeg_out_data, dst, &outjerr)){
  //      free_jpeg(&jpegData);
        free_jpeg(&jpeg_out_data);
        return -1;
    }
    printf("Write: %s\n", dst);
//    free_jpeg(&jpegData);
    free_jpeg(&jpeg_out_data);
    return 0;
}
