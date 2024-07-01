#include <iostream>
#include <string>
#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TCanvas.h"


void drawVariableFromTree(const std::string& filename, const std::string& variable, const std::string& outputFilename) {
	TFile *file = new TFile(filename.c_str(), "READ");
	TDirectory *crtana_dir = (TDirectory*)file->Get("crtana");
	TTree *tree = (TTree*)crtana_dir->Get("tree");
	if (!tree) {
		std::cerr << "Tree 'tree' not found in file '" << filename << "'." << std::endl;
        	file->Close();
		delete file;
		return;
	}
	TCanvas *canvas1 = new TCanvas("canvas1", "Interactive Cut", 800, 600);
	std::string drawCommand = variable + " > -500";
	tree->Draw(variable.c_str());
	canvas1->Draw();
	canvas1->SaveAs(outputFilename.c_str());
	file->Close();
	delete file;
	delete canvas1;
}

int Practicecode() {
	std::string filename = "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13688_crtana.root";
	std::string variable = "cl_sp_x";
	std::string outputFilename = "TrialPlot.png";
	drawVariableFromTree(filename, variable, outputFilename);
	return 0;
}
