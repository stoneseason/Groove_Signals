void chargesignal()
{
	// open the current signal ASCII file and draw a TH1F
	TString fFilename = "signal.out";
	ifstream f_sig(fFilename);

	// build a TH1F for save the charge signal
	TH1F* sig = new TH1F("charge","charge signal;Time(ns);Signal(fC)",4000,0,4000);
	Double_t chargesig = 0;
	for (int i=1;i<=4000;i++)
	{
		Double_t currentsig = 0;
		f_sig>>currentsig;
		chargesig = chargesig + currentsig;
		sig->SetBinContent(i,-chargesig);	
	}
	sig->Draw();
}

