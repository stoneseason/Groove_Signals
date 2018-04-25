#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#include <TCanvas.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TBuffer3D.h>
#include <TBuffer3DTypes.h>
#include <TVirtualViewer3D.h>
#include <TFile.h>

#include "MediumMagboltz.hh"
#include "ComponentAnsys123.hh"
#include "Sensor.hh"
#include "AvalancheMicroscopic.hh"
#include "AvalancheMC.hh"
#include "ViewGeometry.hh"
#include "Plotting.hh"
#include "ViewField.hh"
#include "ViewFEMesh.hh"
#include "ViewSignal.hh"

#include <time.h>

using namespace Garfield;
using namespace std;

const int tWindow=4000;

int main(int argc, char * argv[]) {

  TApplication app("app", &argc, argv);

  

  // Loading the Comsol files
  ComponentAnsys123* groove = new ComponentAnsys123();
  groove->Initialise("ELIST.lis","NLIST.lis","MPLIST.lis","PRNSOL.lis","mm");
    groove->EnablePeriodicityX();
    groove->EnablePeriodicityY();
  //groove->SetMagneticField(0.,0.,1.5);
  groove->PrintRange();
  MediumMagboltz* gas = new MediumMagboltz();
  gas->SetComposition("ar",95.,"ch4",5.);
  gas->SetTemperature(293.15);
  gas->SetPressure(760.0);
  gas->SetMaxElectronEnergy(200.);

  const double rPenning = 0.18;
  const double lambdaPenning = 0.;
  gas->EnablePenningTransfer(rPenning,lambdaPenning,"ar");
  gas->LoadIonMobility("IonMobility_Ar+_Ar.txt");


  gas->Initialise(true);

  //Set the medium
  int numofmat=groove->GetNumberOfMaterials();
  cout<< "Number of material: "<< numofmat<<endl;
  for(int i=0;i<numofmat;i++)
  {
    double perman = groove->GetPermittivity(i);
    cout<<"The permittivity of material "<<i<<" is "<<perman<<endl;
    if(fabs(perman-1)<1.e-13){groove->SetMedium(i,gas);}
  }

  ViewField* fieldView = new ViewField();
  fieldView->SetComponent(groove);
  TCanvas* c1 = new TCanvas();
  fieldView->SetCanvas(c1);
  //fieldView->SetPlane(0., 0., 1, 0., 0., 0.);
  //fieldView->SetArea(-0.2,-0.2,0.2,0.2);
  fieldView->SetPlane(0., -1., 0, 0., 0., 0.);
  fieldView->SetArea(-0.2,-0.05,0.2,0.1);

  //fieldView->PlotProfile(0.,0.,0.02,0,0,-0.05,"e");
  fieldView->PlotContour("v");
  
  //ViewField* fieldView2 = new ViewField();
  //fieldView2->SetComponent(groove);
  //TCanvas* c3 = new TCanvas();
  //fieldView2->SetCanvas(c3);
  //fieldView->PlotContour("e");

  //Electrode label
  const std::string label = "readout";
  //Setup the weighting field.
  //In this example we use a FEM field map.
  groove->SetWeightingField("WPOT.lis",label);
  
  Sensor* sensor = new Sensor();
  sensor->AddComponent(groove);
  sensor->AddElectrode(groove, label);

  //Setup the binning (0 to 2500 ns in 2500 steps).
  const double tStart = 0;
  const double tStop = tWindow;
  const int nSteps = tWindow;
  const double tStep = (tStop - tStart) / nSteps;

  sensor->SetTimeWindow(tStart,tStep,nSteps);

  AvalancheMicroscopic* aval = new AvalancheMicroscopic();
  //AvalancheMC* aval = new AvalancheMC();
 
    AvalancheMC* drift = new AvalancheMC();
    const bool plotDrift = true;
    ViewDrift* driftView = new ViewDrift();
    if (plotDrift) {
      driftView->SetArea(-2 * 0.1, -2 * 0.1, -0.05,
  			  2 * 0.1,  2 * 0.1,  0.14);
      // Plot every 10  collisions (in microscopic tracking).
      //aval->SetCollisionSteps(10);
      drift->EnablePlotting(driftView);
    }
  aval->SetSensor(sensor);
   
  aval->EnableSignalCalculation();

  //aval->EnableMagneticField();
  aval->EnableAvalancheSizeLimit(1000);


  //Set the signal pool
  double totsignal[tWindow];
  for(int i=0;i<tWindow;i++) {totsignal[i]=0;}
  double totesignal[tWindow];
  for(int i=0;i<tWindow;i++) {totesignal[i]=0;}
  double totionsignal[tWindow];
  for(int i=0;i<tWindow;i++) {totionsignal[i]=0;}

  ofstream ofelectrons("electrons.out");
  ofstream ofallsignal("allsignals.out");
  ofstream ofallesignal("all_e_signals.out");
  ofstream ofallionsignal("all_ion_signals.out");



  for(int i=0;i<1;i++)
  {
    float timeused = (float)clock()/CLOCKS_PER_SEC;
    if (timeused >25000.0)
    {
      cout<<"Time limit reached, "<<timeused<<" sec"<<endl;
      break;
    }
    
    /*
    //Calculate one electron.
    //Set the initial position [cm] and starting time[ns].
    double x0=0., y0=0., z0=0.04, t0=0.;
    //Set the initial energy[eV].
    double e0=0.1;
    //Set the initial direction (x,y,z).
    //In case of a null vector, the direction si randomized.
    double dx0=0., dy0=0., dz0=0.;
    //Calculate an electron avalanche.
    aval->AvalancheElectron(x0,y0,z0,t0,e0,dx0,dy0,dz0);
    

    
    int ne,ni;
    aval->GetAvalancheSize(ne,ni);
    using namespace std;
    cout<<"Avalanche - electrons = "<<ne<<", ions = "<<ni<<endl;
    ofelectrons<<ne<<" "<<ni<<endl;

    double ex0, ey0, ez0, et0, ee0;
    double ex, ey, ez, et, ee;
    int status;
    ofstream electron_start_point("avalanche_start.out");
    ofstream electron_end_point("avalanche_end.out");
    const int np = aval->GetNumberOfElectronEndpoints();
    for (int j=np; j--;) {
    	aval->GetElectronEndpoint(j, ex0, ey0, ez0, et0, ee0, ex, ey, ez, et, ee, status);
        electron_start_point<<ex0<<" "<<ey0<<" "<<ez0<<" "<<et0<<" "<<ee0<<endl;
        electron_end_point<<ex<<" "<<ey<<" "<<ez<<" "<<et<<" "<<ee<<" "<<status<<endl;
    }

*/
  
    //calculate ions
    drift->SetSensor(sensor);
    drift->EnableSignalCalculation();
    drift->SetDistanceSteps(2.e-4); //important  
  
    //Set the initial position [cm] and starting time[ns].
    TString fFilename1 = "avalanche_start.out";
    ifstream f_start(fFilename1);
    double ix0=0., iy0=0., iz0=0., it0=0.;
    while (!f_start.eof())
    {
    	double temp;
	f_start>>ix0; f_start>>iy0; f_start>>iz0; f_start>>it0; f_start>>temp;
	drift->DriftIon(ix0,iy0,iz0,it0);
    }

    for (int j=0;j<tWindow;j++)
    {
      double s=(sensor->GetSignal(label,j));
      double s_e=(sensor->GetElectronSignal(label,j));
      double s_ion=(sensor->GetIonSignal(label,j));
      totsignal[j]=totsignal[j]+s;
      ofallsignal<<s<<" ";
      totesignal[j]=totesignal[j]+s_e;
      ofallesignal<<s_e<<" ";
      totionsignal[j]=totionsignal[j]+s_ion;
      ofallionsignal<<s_ion<<" ";

    }
    ofallsignal<<endl;
    ofallesignal<<endl;
    ofallionsignal<<endl;
  }
  
  //Polt the induced current.
  ViewSignal* signalView = new ViewSignal();
  signalView->SetSensor(sensor);
  TCanvas* c2 = new TCanvas();
  signalView->SetCanvas(c2);
  signalView->PlotSignal(label);
  
  TCanvas* cD = new TCanvas();
  if (plotDrift) {
	driftView->SetCanvas(cD);
	driftView->Plot();
	ViewFEMesh* meshView = new ViewFEMesh();
	meshView->SetComponent(groove);
	meshView->SetPlane(0., -1., 0, 0., 0., 0.);
	meshView->SetArea(-0.2,-0.1,-0.05,0.2,0.1,0.14);
	meshView->SetFillColor(2,3); //matid=2 is FR4, paint as green.
	meshView->SetFillMesh(true);
	meshView->SetViewDrift(driftView);
	TCanvas* cM = new TCanvas();
	meshView->SetCanvas(cM);
	meshView->Plot();

  }

  TH1F* signalView2 = new TH1F("totsignal","totsignal",tWindow,0,tWindow);
  
  ofstream ofsig("signal.out");
  ofstream ofesig("signal_e.out");
  ofstream ofionsig("signal_ion.out");
  for (int i=0;i<tWindow;i++) 
  {
    //cout<<"Time="<<i<<"ns Charge="<<totsignal[i]<<"fC."<<endl;
    ofsig<<totsignal[i]<<endl;
    ofesig<<totesignal[i]<<endl;
    ofionsig<<totionsignal[i]<<endl; 

  }
  
  
  signalView2->Draw();
  cin.get();
}
