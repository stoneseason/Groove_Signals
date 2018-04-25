void compare()
{
	// open the current signal ASCII file and draw a TH1F
	TString fFilename = "signal_e.out";
	ifstream f_sig(fFilename);
	TString fFilename2 = "signal_ion.out";
	ifstream f_sig2(fFilename2);
	TString fFilename3 = "signal_ion_aval.out";
	ifstream f_sig3(fFilename3);

	// build a TH1F for save the charge signal
	TH1F* sig0 = new TH1F("charge_aval","charge signal;Time(ns);Signal(fC)",4000,0,4000);
	TH1F* sig = new TH1F("charge_button","charge signal;Time(ns);Signal(fC)",4000,0,4000);
	Double_t chargesig0 = 0;
	Double_t chargesig = 0;
	for (int i=1;i<=4000;i++)
	{
		Double_t currentsig = 0;
		f_sig>>currentsig;
		chargesig = chargesig + currentsig;
		chargesig0 =chargesig0+ currentsig;
		
		f_sig2>>currentsig;
		chargesig = chargesig + currentsig;
		sig->SetBinContent(i,-chargesig);	
		
		f_sig3>>currentsig;
		chargesig0 = chargesig0 +currentsig;
		sig0->SetBinContent(i,-chargesig0);
	}
	sig0->SetLineColor(6);
	sig->SetMarkerColor(4);
	sig0->Draw();
	sig->Draw("same");
}

