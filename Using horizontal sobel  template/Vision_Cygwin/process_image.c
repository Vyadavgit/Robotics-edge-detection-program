#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>

#define DIM 512

// declaring arrays
float f_image[DIM][DIM]; 
int i_image[DIM][DIM];
unsigned char c_image[DIM][DIM];
float h_image[DIM][DIM]; //convulation result - float data type

// 3x3 Sobel templates
// 2k+1=3 rows, 2l+1=3 cols //here both width and height are equal for this template
int k=1; // 2k+1=3 rows
int l=1; // 2l+1=3 cols

int htemplate[3][3]={ // horizontal edge template 2k+1=3 rows, 2l+1=3
    {-1,-2,-1},
    {0,0,0},
    {1,2,1}
}; 

float f_htemplate[3][3]; // float type horizontal edge template

/******************************************************************/
/* This structure contains the coordinates of a box drawn with    */
/* the left mouse button on the image window.                     */
/* roi.x , roi.y  - left upper corner's coordinates               */
/* roi.width , roi.height - width and height of the box           */
/******************************************************************/
extern XRectangle roi;


/******************************************************************/
/* Main processing routine. This is called upon pressing the      */
/* Process button of the interface.                               */
/* image  - the original greyscale image                          */
/* size   - the actual size of the image                          */
/* proc_image - the image representation resulting from the       */
/*              processing. This will be displayed upon return    */
/*              from this function.                               */
/******************************************************************/
void process_image(image, size, proc_img)
unsigned char image[DIM][DIM];
int size[2];
unsigned char proc_img[DIM][DIM];
{
    // ----------------------------------------type casting char to float------------------
        // float f_image[DIM][DIM];   
        for (int i = 0; i < size[0]; i++) {
        for (int j = 0; j < size[1]; j++) {
            f_image[i][j]=(float)image[i][j];
        }
    }

    // ----------------------------------------type casting int to float-----------------
    // type cast templates to float 
    for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
            f_htemplate[i][j]=((float)htemplate[i][j])/9;
        }
    }

    //----------------------------------calculate average of image------------------------
    // calculate average of image
    float avg_f_image;
    float total=0;
    for(int x=0; x<size[0]; x++){
        for(int y=0; y<size[1]; y++){
            total = total + f_image[x][y];
        }
    }
    avg_f_image = total/(size[0]*size[1]); // m*n size image

    // compute convolution and normalize: h(x,y) = Σ(i=-k to k)Σ(j=-l to l) ((image(x+i,y+j)-imgavg)*(template(i+k,j+l)-templateavg))
    // iterate through all locations of image where template can be placed and fill the remaining locations with neighbour's value
    // the template can be placed to (m-(k-1))*(n-(l-1)) locations for [m x n-image & k x l-template]
    // for this assignment, the template can be placed to (m-(l-1))*(n-(k-1)) locations for [n x m-image & k x l-template]
    for(int x=1; x<=(size[0]-(l-1)); x++){ // (m-(l-1)) i.e (m,n) = (x,y) coordinates apparantly
        for(int y=1; y<=(size[1]-(k-1)); y++){ // (n-(k-1)) i.e (m,n) = (x,y) coordinates apparantly
            h_image[x][y]=0;
            // iterate through all rows and cols of template region and sum them up
            // here convolution is k*l expensive
            for (int i=-l; i<=l; i++){
                for(int j=-k; j<=k; j++){
                    // h(x,y) = Σ(i=-k to k)Σ(j=-l to l) ((image(x+i,y+j)-iavg)*template(i+k,j+l)) // this function works when the image_array's (rows,cols) = (Y-components,X-components) i.e (m,n)

                    // the image received for this assignment has the form: the image_array's (rows,cols) = (X-components,Y-components) i.e (m,n)
                    h_image[x][y] = (f_image[x+i][y+j]-avg_f_image)*f_htemplate[j+k][i+l] + h_image[x][y]; // hence, in this case this function works
                }
            }
        }
    }

    // fill the edge locations with neighbour's value
    for(int x=0; x<=size[0]; x++){
        for(int y=0; y<=size[1]; y++){
            if(x==0 && y==0){
                h_image[x][y]=h_image[x+1][y+1];
            }
            else if(x==0){
                h_image[x][y]=h_image[x+1][y];
            }
            else if(y==0){
                h_image[x][y]=h_image[x][y+1];
            }
            else if(x>=(size[0]-(k-1))){ 
                h_image[x][y]=h_image[x-1][y];
            }
            else if(y>=(size[1]-(l-1))){ 
                h_image[x][y]=h_image[x][y-1];
            }
        }
    }

    // find max and min from the processed result 
    float max=-100000; // arbitrary max chosen
    float min=100000;  // arbitrary min chosen
    for(int i=0; i<size[0]; i++){
        for(int j=0; j<size[1]; j++){
            if(h_image[i][j]>max){
                max=h_image[i][j];
            }
            if(h_image[i][j]<min){
                min=h_image[i][j];
            }
        }
    }


    // scaling range [min, max] to range [0, 255]
    // for scaling range [min, max] to [a,b], h(x,y) = ((h(x,y)-min)*(b-a))/(max-min) + a
    for(int i=0; i<size[0]; i++){
        for(int j=0; j<size[1]; j++){
            h_image[i][j]= ((h_image[i][j]-min)*(255-0))/(max-min)+0; 
        }
    }


    // ----------------------------------------type casting float to int------------------
    // int i_image[DIM][DIM];
    for (int i = 0; i < size[0]; i++) {
        for (int j = 0; j < size[1]; j++) {
            i_image[i][j]=(int)h_image[i][j];
        }
    }

    // ----------------------------------------type casting int to char------------------
    // unsigned char proc_img[DIM][DIM];
    for (int i = 0; i < size[0]; i++) {
        for (int j = 0; j < size[1]; j++) {
            c_image[i][j]=(char)i_image[i][j];
        }
    }

    // passing values to processed image
    for (int i = 0; i < size[0]; i++) {
        for (int j = 0; j < size[1]; j++) {
            proc_img[i][j]=c_image[i][j];
        }
    }

}
