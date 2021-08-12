#include <vector>
#include <iostream>

// vtk classes
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
//#include <vtkXMLPolyDataWriter.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkFloatArray.h>
#include <vtkCamera.h>
#include <vtkPointData.h>
#include <vtkImplicitFunction.h>
#include <vtkSphere.h>

#include "ImplicitFuncPolygonizer.h"

#include <array>


//static std::vector<std::array<int, 3>> currTriCells;
static VERTICES s_currVertices;  /* global needed by application */
static vtkCellArray* s_currPolys = nullptr;
static vtkImplicitFunction* s_currFunc = nullptr;


/* triangle: called by polygonize() for each triangle; write to stdout */

int triangle (int i1, int i2, int i3, VERTICES vertices)
{
    s_currVertices = vertices;
		if (s_currPolys)
		{
			vtkIdType ids[3];
			ids[0] = i1;
			ids[1] = i3; // reverse
			ids[2] = i2;
			s_currPolys->InsertNextCell(3, ids);
		}

  	//	std::cout << i1 << " " << i2 << " " << i3 << std::endl;
    return 1;
}


double torus (double x, double y, double z)
{
    double x2 = x*x, y2 = y*y, z2 = z*z;
    double a = x2+y2+z2+(0.5*0.5)-(0.1*0.1);
    return a*a-4.0*(0.5*0.5)*(y2+z2);
}

double sphere (double x, double y, double z)
{
    double rsq = x*x+y*y+z*z;
    return 1.0/(rsq < 0.00001? 0.00001 : rsq);
}


/* blob: a three-pole blend function, try size = .1 */

double blob (double x, double y, double z)
{
    return 4.0-sphere(x+1.0,y,z)-sphere(x,y+1.0,z)-sphere(x,y,z+1.0);
}

double implictVTKFunc(double x, double y, double z)
{
	assert(s_currFunc);

	return s_currFunc->FunctionValue(x, y, z);
}


/* main: call polygonize() with torus function
 * write points-polygon formatted data to stdout */

int main ()
{
    int i;
    char *err;
    std::cout<<"triangles\n\n";
  	vtkNew<vtkCellArray> polys;
		s_currPolys = polys.GetPointer();
		vtkNew<vtkSphere> sphere;
		sphere->SetCenter(0, 0, 0);
		sphere->SetRadius(1.0);
		s_currFunc = sphere.GetPointer();
   // if ((err = polygonize(blob, .05, 40, 0.,0.,0., triangle, NOTET)) != NULL) 
    if ((err = polygonize(implictVTKFunc, .05, 40, 0.,0.,0., triangle, NOTET)) != NULL) 
		{
	   std::cerr<<err<<std::endl;
		 return 1;
		}

		vtkNew<vtkPolyData> surface;
		vtkNew<vtkPoints>       pts;
		surface->SetPoints(pts);
		surface->SetPolys(s_currPolys);

		pts->SetNumberOfPoints(s_currVertices.count);
		vtkNew<vtkFloatArray> normals;
		normals->SetNumberOfComponents(3);
		normals->SetNumberOfTuples(s_currVertices.count);
		surface->GetPointData()->SetNormals(normals);

		for (i = 0; i < s_currVertices.count; i++)
		{
	    VERTEX v;
      v = s_currVertices.ptr[i];
			pts->SetPoint(i, v.position.x, v.position.y, v.position.z);
			normals->SetTuple3(i, v.normal.x, v.normal.y, v.normal.z);
	    //std::cout<<v.position.x<<" "<< v.position.y<<" "<< v.position.z
	    //         <<v.normal.x<<" "<<v.normal.y<<" "<<	v.normal.z<<std::endl;
    }
		free(s_currVertices.ptr);
		s_currPolys = nullptr;
		s_currFunc = nullptr;
	  //std::cout << gTriCells.size() / 3 << " triangles, " << gvertices.count <<" vertices"<<std::endl;    

   vtkNew<vtkPolyDataMapper> mapper;
	 mapper->SetInputData(surface);
  // The actor is a grouping mechanism: besides the geometry (mapper), it
  // also has a property, transformation matrix, and/or texture map.
  // Here we set its color and rotate it around the X and Y axes.
  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
   // The renderer generates the image
  // which is then displayed on the render window.
  // It can be thought of as a scene to which the actor is added
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);
  // Zoom in a little by accessing the camera and invoking its "Zoom" method.
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Zoom(1.5);

  // The render window is the actual GUI window
  // that appears on the computer screen
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(300, 300);
  renderWindow->AddRenderer(renderer);
  renderWindow->SetWindowName("Polygonizer");

  // The render window interactor captures mouse events
  // and will perform appropriate camera or actor manipulation
  // depending on the nature of the events.
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  // This starts the event loop and as a side effect causes an initial render.
  renderWindow->Render();
  renderWindowInteractor->Start();

		return 0;
}

