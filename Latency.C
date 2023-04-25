#include <iostream>
#include <fstream>
#include <cmath>

//Root headers
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TF1.h"
#include "TH2.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TRandom3.h"
#include "TVector3.h"
#include "TRotation.h"
#include "TLorentzVector.h"


void Latency(const char* inputFileName = "latency_scan_Apri19_2023.root"){

	// ******** Start measuring time ******** //
	clock_t start, end, cpu_time;
	start = clock();

	// Read the skimmed ntuple from miniAOD after reference frame transformation
	TFile *infile = TFile::Open(inputFileName, "READ");
	TTree *outputtree = (TTree*)gDirectory -> Get("outputtree");

	TH2D *CHvsLatencyHist = new TH2D("CHvsLatencyHist", "channel vs latency;Latency(BX);VFAT Channel", 210, -5, 205, 136, -5, 131);
	TH1D *CHHist = new TH1D("CHHist", "channel;VFAT Channel;Rate", 136, -5, 131);
	TH1D *LatencyHist = new TH1D("LatencyHist", "Latency;Latency(BX);Rate", 210, -5, 205);
	TH1D *LatencyHistFix = new TH1D("LatencyHistFix", "Latency;Latency(BX);Rate", 210, -5, 205);

	// // Read tree
	// TTreeReader reader("outputtree", infile);
	// TTreeReaderValue<Int_t> latency(reader, "runParameter");
	// TTreeReaderValue<Int_t> slots(reader, "slot"), ohs(reader, "OH");
	// TTreeReaderValue<Int_t> vfats(reader, "VFAT"), channels(reader, "CH");
	// TTreeReaderValue<Int_t> strips(reader, "digiStrip");

	double NEntries = outputtree -> GetEntries(); 
	cout << "Total Entries: " << NEntries << endl;
	// // cout << reader.Next() << endl;
	// for(int iEvent =0; iEvent < 749/*NEntries*/ ; iEvent++){
	// 	reader.Next();
	// 	cout << *latency << endl;

	// }
	TCanvas *c1 = new TCanvas("c1", "c1", 1000, 300);
	c1 -> Divide(3);
	int OHVFATArray[11][2]={{0, 8}, 
							{1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 6}, {1, 10}, 
							{2, 0}, {2, 2}, {2, 7}, {2, 11}};
	int NoiseCh[11] = {102, 
					   1, 0 ,126, 127, 15, 85, 
					   /*{32,34,82}*/32, 125, 126, 85};

	for(int i=0; i<11; i++){
		c1 -> cd(1);
		gPad -> SetLogy(); 
		gStyle -> SetOptStat(0);
	 	outputtree -> Draw("CH>>CHHist", Form("OH==%d && VFAT==%d",OHVFATArray[i][0], OHVFATArray[i][1]), "HIST");
	 	CHHist -> Scale(1/NEntries);

		CHHist -> GetXaxis() -> SetRangeUser(-5, 131);
		CHHist -> SetMinimum(0.5E-7);
		CHHist -> SetTitle(Form("Channel (OH=%d, VFAT=%d)", OHVFATArray[i][0], OHVFATArray[i][1]));

	 	c1 -> cd(2);
	 	gPad -> SetFrameFillColor(TColor::GetColorPalette(0));
	 	gPad -> SetRightMargin(0.12);
	 	gStyle -> SetOptStat(0);
		gStyle -> SetPalette(kViridis);
		outputtree -> Draw("CH:runParameter>>CHvsLatencyHist", Form("OH==%d && VFAT==%d",OHVFATArray[i][0], OHVFATArray[i][1]), "COLZ");
		CHvsLatencyHist -> GetXaxis() -> SetRangeUser(-5, 205);
		CHvsLatencyHist -> GetYaxis() -> SetRangeUser(-5, 131);
		CHvsLatencyHist -> SetTitle(Form("Channel vs Latency (OH=%d, VFAT=%d)", OHVFATArray[i][0], OHVFATArray[i][1]));

	 	c1 -> cd(3);
	 	gPad -> SetLeftMargin(0.12);
	 	gStyle -> SetOptStat(0);
	 	outputtree -> Draw("runParameter>>LatencyHist", Form("OH==%d && VFAT==%d",OHVFATArray[i][0], OHVFATArray[i][1]), "HIST");
	 	LatencyHist -> Scale(1./NEntries);
	 	LatencyHist -> GetXaxis() -> SetRangeUser(-5, 205);
	 	LatencyHist -> SetMinimum(0);	
		LatencyHist -> SetTitle(Form("Latency (OH=%d, VFAT=%d)", OHVFATArray[i][0], OHVFATArray[i][1]));

	 	c1 -> SaveAs(Form("%d%d.gif",OHVFATArray[i][0], OHVFATArray[i][1]));
	
	TCanvas *c2 = new TCanvas("c2", "c2", 300, 300);
	outputtree -> Draw("runParameter>>LatencyHistFix", Form("OH==%d && VFAT==%d && CH!=%d",OHVFATArray[i][0], OHVFATArray[i][1], NoiseCh[i]), "HIST");
	LatencyHistFix -> Scale(1./NEntries);
	LatencyHistFix -> GetXaxis() -> SetRangeUser(-5, 205);
	LatencyHistFix -> SetMinimum(0);	
	c2 -> SaveAs(Form("%d%d_exc%d.gif", OHVFATArray[i][0], OHVFATArray[i][1], NoiseCh[i]));

	}

	int OH = 2;
	int VFAT = 0;

	// c1 -> cd(1);
	// gPad -> SetLogy(); 
	// gStyle -> SetOptStat(0);
 	// outputtree -> Draw("CH>>CHHist", Form("OH==%d && VFAT==%d",OH, VFAT), "HIST");
 	// CHHist -> Scale(1/NEntries);

	// CHHist -> GetXaxis() -> SetRangeUser(-5, 131);
	// CHHist -> SetMinimum(0.5E-7);
	// CHHist -> SetTitle(Form("Channel (OH=%d, VFAT=%d)", OH, VFAT));



	TCanvas *c3 = new TCanvas("c3", "c3", 300, 300);
	outputtree -> Draw("runParameter>>LatencyHistFix", Form("OH==%d && VFAT==%d && CH!=32 && CH!=34 && CH!=82",OH, VFAT), "HIST");
	LatencyHistFix -> Scale(1./NEntries);
	LatencyHistFix -> GetXaxis() -> SetRangeUser(-5, 205);
	LatencyHistFix -> SetMinimum(0);	
	c3 -> SaveAs(Form("%d%d_exc%d.gif", OH, VFAT, 323482));


	return;

}