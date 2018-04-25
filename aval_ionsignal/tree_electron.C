void tree_electron()
{

	// build a TTree and connect the variables to the branch
	TTree *e_tree = new TTree("e_tree","information of electrons");
	double x0,y0,z0,t0,e0;
	e_tree->Branch("x0",&x0,"x0/D");
	e_tree->Branch("y0",&y0,"y0/D");
	e_tree->Branch("z0",&z0,"z0/D");
	e_tree->Branch("t0",&t0,"t0/D");
	e_tree->Branch("e0",&e0,"e0/D");
	double x,y,z,t,e;
	int status;
	e_tree->Branch("x",&x,"x/D");
	e_tree->Branch("y",&y,"y/D");
	e_tree->Branch("z",&z,"z/D");
	e_tree->Branch("t",&t,"t/D");
	e_tree->Branch("e",&e,"e/D");
	e_tree->Branch("status",&status,"status/I");

	// open the electron_start ASCII file and fill the tree
	TString fFilename1 = "avalanche_start.out";
	ifstream f_start(fFilename1);
	TString fFilename2 = "avalanche_end.out";
	ifstream f_end(fFilename2);
	while (!f_start.eof())
	{ 
		f_start>>x0; f_start>>y0; f_start>>z0; f_start>>t0; f_start>>e0;
		f_end>>x; f_end>>y; f_end>>z; f_end>>t; f_end>>e; f_end>>status;
		e_tree->Fill();
	}
	f_start.close();
	f_end.close();
	
	// check what the trees look like and save them
	TFile *f = new TFile("e_tree.root","RECREATE");
	e_tree->Print();
	e_tree->Write();
	f->Write();	
	
}	
	
