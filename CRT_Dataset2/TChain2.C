#include <iostream>
#include "TFile.h"
#include "TChain.h"
#include "TH1D.h"
#include "TCanvas.h"

void createAndFillHistogram(const std::string& inputDir, const std::string& outputFilename) {
	TChain *chain = new TChain("crtana/tree");
	TString filePattern = inputDir + "/*crtana.root";
	std::cout << "Adding files matching pattern: " << filePattern.Data() << std::endl;
	Int_t nFilesAdded = chain->Add(filePattern);
	TObjArray *fileList = chain->GetListOfFiles();
	if (fileList) {
		for (Int_t i = 0; i < fileList->GetEntries(); ++i) {
			TChainElement *element = (TChainElement*)fileList->At(i);
			if (element) {
				std::cout << "Added file: " << element->GetTitle() << std::endl;
			}
		}
	}
	







	TH1D *histogram = new TH1D("histogram1D", "1D Histogram", 20, -400, 400);
	std::vector<double> *cl_sp_x = nullptr;
	std::vector<double> *cl_sp_z = nullptr;
	std::vector<bool> *cl_has_sp = nullptr;

	chain->SetBranchAddress("cl_has_sp", &cl_has_sp);
	chain->SetBranchAddress("cl_sp_x", &cl_sp_x);
	chain->SetBranchAddress("cl_sp_z", &cl_sp_z);

	Long64_t nEntries = chain->GetEntries();
	Long64_t maxEntriesPerFile = 1000;
	Long64_t entriesToProcess = std::min(nEntries, maxEntriesPerFile);
	for (Long64_t i = 0; i < entriesToProcess; ++i) {
		chain->GetEntry(i);
		for (size_t k = 0; k < cl_has_sp->size(); ++k) {
			if ((*cl_has_sp)[k]) {
				for (size_t j = 0; j < cl_sp_x->size(); ++j) {
					double x = (*cl_sp_x)[j];
					double z = (*cl_sp_z)[j];
					if (-250 < z && z < -150) {
						histogram->Fill(x);
						std::cout << "Filled histogram with x = " << x << std::endl; // Print debug message
					}	
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
	std::string inputDir = "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024";
	std::string outputFilename = "1DHistogram2.png";
	createAndFillHistogram(inputDir, outputFilename);
	return 0;
}
