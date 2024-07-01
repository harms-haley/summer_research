#include <iostream>
#include "TFile.h"
#include "TChain.h"
#include "TH1D.h"
#include "TCanvas.h"

void createAndFillHistogram(const std::string& inputDir, const std::string& outputFilename) {
	TChain *chain = new TChain("crtana/tree");
	chain->Add((inputDir + "/run*_crtana.root").c_str());
	TH1D *histogram = new TH1D("histogram1D", "1D Histogram", 20, -400, 400);
	std::vector<double> *cl_sp_x = nullptr;
	std::vector<double> *cl_sp_z = nullptr;
	bool cl_has_sp;

	chain->SetBranchAddress("cl_has_sp", &cl_has_sp);
	chain->SetBranchAddress("cl_sp_x", &cl_sp_x);
	chain->SetBranchAddress("cl_sp_z", &cl_sp_z);

	Long64_t nEntries = chain->GetEntries();
	for (Long64_t i = 0; i < nEntries; ++i) {
		chain->GetEntry(i);
		if (cl_has_sp) {
			for (size_t j = 0; j < cl_sp_x->size(); ++j) {
				double x = (*cl_sp_x)[j];
				double z = (*cl_sp_z)[j];
				if (-250 < z && z < -150) {
					histogram->Fill(x);
				}
			}
		}
	}
	
	histogram->GetXaxis()->SetTitle("X");
	TCanvas *c1 = new TCanvas("c", "1D Histogram", 800, 600);
	histogram->Draw();
	c1->Draw();
	c1->SaveAs(outputFilename.c_str());
	delete c1;
	delete histogram;
	delete chain;
}

int TChain2() {
	std::string inputDir = "/path/to/your/files";
	std::string outputFilename = "1DHistogram.png";
	createAndFillHistogram(inputDir, outputFilename);
	return 0;
}
