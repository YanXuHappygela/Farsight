/*=========================================================================
Copyright 2009 Rensselaer Polytechnic Institute
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. 
=========================================================================*/
#ifndef IMAGEACTORS_H_
#define IMAGEACTORS_H_

#include "vtkActor.h"
#include "vtkContourFilter.h"
#include "vtkColorTransferFunction.h"
#include "vtkImageData.h"
#include "vtkImageToStructuredPoints.h"
#include "vtkLODActor.h"
#include "vtkOpenGLVolumeTextureMapper3D.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkSmartPointer.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"

#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"

#include <stdio.h>
#include <string>
#include <vector>
//#include <set>
//#include <QtGui>



const unsigned int Dimension = 3;
typedef unsigned char  ImageActorPixelType;
typedef itk::Image< ImageActorPixelType, Dimension >   ImageType;
typedef itk::ImageFileReader< ImageType >    ReaderType;
typedef itk::ImageToVTKImageFilter<ImageType> ConnectorType;
struct imageFileHandle
{
	std::string tag;
	std::string filename;
	vtkSmartPointer<vtkImageData> ImageData;
	ReaderType::Pointer reader;
	ConnectorType::Pointer connector;
	std::vector<double> ImageSize;
	double x,y,z;
//Contour Filter pointers
	vtkSmartPointer<vtkContourFilter> ContourFilter;
	vtkSmartPointer<vtkPolyDataMapper> ContourMapper;
	vtkSmartPointer<vtkActor> ContourActor;
//Raycast pointers
	vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction;
	vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction;
	vtkSmartPointer<vtkVolumeProperty> volumeProperty;
	vtkSmartPointer<vtkOpenGLVolumeTextureMapper3D> volumeMapper;
	vtkSmartPointer<vtkVolume> volume;
};
class  ImageRenderActors
{
public:
	ImageRenderActors();
	int loadImage(std::string ImageSource, std::string tag);
	int loadImage(std::string ImageSource, std::string tag, double x, double y, double z);
//render actors
	vtkSmartPointer<vtkActor> ContourActor(int i);
	vtkSmartPointer<vtkVolume> RayCastVolume(int i);
//file information
	std::vector<std::string> GetImageList();
	std::string FileNameOf(int i){ return this->LoadedImages[i]->filename;};
	unsigned int NumberOfImages() {return this->LoadedImages.size();};
	bool isRayCast(int i);
	void ShiftImage(int i, double x, double y, double z);
	double pointData(int i, int x, int y, int z);
	std::vector<double> GetImageSize(int i);
	vtkSmartPointer<vtkImageData> GetImageData(int i);
	std::vector<double> getColorValues();
	void setColorValues(double r, double g, double b);
	void setColorValues(int i, double value);

private:
	vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction;
	vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction;
	std::vector<imageFileHandle*> LoadedImages;
	std::vector<std::string> ImageList;
	double r,g,b, opacity1, opacity2;
};
#endif
