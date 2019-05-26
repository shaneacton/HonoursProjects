#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "volimage.h"

using namespace std;
 
ACTSHA001::VolImage::VolImage(){
  height=0;
  width=0;
  noImages=0;
}
 
ACTSHA001::VolImage::~VolImage(){
   //DELETE ALL POINTERS
  //cout<<"volimage destructor called"<<endl;
   for(int slice=0;slice<noImages;slice++){
     for(int row=0;row<height;row++){
       delete[] slices[slice][row];
     }
     delete [] slices[slice];
   }
 }

void ACTSHA001::VolImage::diffmap(int sliceI, int sliceJ, std::string output_prefix){
  unsigned char** pic = new unsigned char*[height];
  for(int row=0;row<height;row++){
    unsigned char* rowPeice = new unsigned char[width];
    for(int col=0;col<width;col++){
      unsigned char* c = new unsigned char;
      *c = (slices[sliceI][row][col]-slices[sliceJ][row][col])/2;
      rowPeice[col] = *c;
    }
    pic[row] = rowPeice;
  }
  writeImage(pic,output_prefix);
  
  //clean memory after using temp pic
    for(int row=0;row<height;row++){
      delete[] pic[row];  
    }
    cout<<"deleting pic"<<endl;
    delete[] pic;
}

void ACTSHA001::VolImage::extract(int sliceId, std::string output_prefix){
  writeImage(slices[sliceId],output_prefix);
}


void ACTSHA001::VolImage::writeImage(unsigned char** pic ,std::string output_prefix){
  //write file
  ofstream out(output_prefix+"/output.raw",std::ios::binary);
  for(int row=0;row<height;row++){
      out.write((char*) pic[row],width);
  }
  out.close();
  //write header
  ofstream outH(output_prefix+"/output.dat");
  std::string outS = std::to_string(width) +" " + std::to_string(height) + " 1";
  outH<<outS;
  outH.close();
}


bool ACTSHA001::VolImage::readImages(std::string imageBase){

  int* arr = readHeader(imageBase);
  width = arr[0];
  height = arr[1];
  noImages = arr[2];
  cout << "Number of images: " << noImages<<endl;
  
  
  int size = 0;
  for(int slice=0;slice<noImages;slice++){
    
    //allocating space on heap for incoming slice file
    unsigned char** pic = new unsigned char*[height];
    for(int row=0;row<height;row++){
      unsigned char* rowPeice = new unsigned char[width];
      for(int col=0;col<width;col++){
        unsigned char* c = new unsigned char;
        rowPeice[col] = *c;
      }
        pic[row] = rowPeice;
    }
    slices.push_back(pic);
    
    ifstream inFile((imageBase+"_Raws/"+imageBase+""+std::to_string(slice)+".raw"),std::ios::binary);
    if(!inFile){
      cout<<"file not found"<<endl;
    }
    
    //read raw slice file into slices vec, accounts for size of data and all used pointers
    for(int row=0;row<height;row++){
      inFile>>*slices[slice][row];
      for(int col=0;col<width;col++){
        size+=sizeof(slices[slice][row][col]);
      }
      size+=sizeof(slices[slice][row]);
    }
    size+=sizeof(slices[slice]);
    inFile.close();
  }
  
  size+=sizeof(slices);
  cout << "Number of bytes required: " <<size<< endl;
  
  return true;
}

 
 int*  ACTSHA001::VolImage::readHeader(string imageBase){//returns width,height, no_images
   int* values  = new int[3];
   
   ifstream myReadFile;
   string fileName = imageBase+"_Raws/"+imageBase+".data";
   myReadFile.open(fileName);
   char output[100];
   if (myReadFile.is_open()) {
     int i = 0;
     while (!myReadFile.eof()) {
       myReadFile >> output;
       values[i] = stoi(output);
       i++;
     }
   }
   myReadFile.close();
   return values;
 }

 
