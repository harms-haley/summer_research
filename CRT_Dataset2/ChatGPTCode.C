#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include <iostream>

void ChatGPTCode() {
	TChain chain("crtana/tree"); // Ensure "myTree" is the correct name of your TTree
	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/*crtana.root"); // Replace with your file path pattern
	TH1F *histogram1_f_t = new TH1F("histogram1_f_t", "Front Face X;X;Counts", 100, -10, 10);
	TH1F *histogram2_f_t = new TH1F("histogram2_f_t", "Front Face Y;Y;Counts", 100, -10, 10);
	TH2F *histogram3_f_t = new TH2F("histogram3_f_t", "Front Face XY;X;Y;Counts", 100, -10, 10, 100, -10, 10);
    	TCanvas *c1_f_t = new TCanvas("c1_f_t", "Front Face X");
	TCanvas *c2_f_t = new TCanvas("c2_f_t", "Front Face Y");
	TCanvas *c3_f_t = new TCanvas("c3_f_t", "Front Face XY");

	std::vector<double> *cl_sp_x = nullptr;
    	std::vector<double> *cl_sp_y = nullptr;
    	std::vector<double> *cl_sp_z = nullptr;
    	std::vector<double> *cl_sp_ts1 = nullptr;
    	std::vector<bool> *cl_has_sp = nullptr;
	chain.SetBranchAddress("cl_has_sp", &cl_has_sp);
    	chain.SetBranchAddress("cl_sp_x", &cl_sp_x);
    	chain.SetBranchAddress("cl_sp_y", &cl_sp_y);
    	chain.SetBranchAddress("cl_sp_z", &cl_sp_z);
    	chain.SetBranchAddress("cl_sp_ts1", &cl_sp_ts1);

	Long64_t nEntries = chain.GetEntries();
    	Long64_t nMaxEntries = 1000;

	for (Long64_t i = 0; i < nEntries && i < nMaxEntries; ++i) {
        	chain.GetEntry(i);
		if (cl_has_sp == nullptr || cl_has_sp->size() == 0 || !cl_has_sp->at(0)) {
            		continue;
        	}
		
		for (size_t j = 0; j < cl_sp_x->size(); ++j) {
            		double t1 = cl_sp_ts1->at(j);
            		double x = cl_sp_x->at(j);
            		double y = cl_sp_y->at(j);
            		double z = cl_sp_z->at(j);

			std::cout << "Entry " << i << ", Hit " << j << ": t1=" << t1 << ", x=" << x << ", y=" << y << ", z=" << z << std::endl;
			if (1529.8e3 < t1 && t1 < 1532.8e3) {
				if (y > -350) {  // cut off feet of detector
                    			if (-250 < z && z < -150) {
                        			histogram1_f_t->Fill(x);
                        			histogram2_f_t->Fill(y);
                        			histogram3_f_t->Fill(x, y);
                    			}
                		}
            		}
        	}
    	}


	c1_f_t->cd();
    	histogram1_f_t->Draw();
    	c2_f_t->cd();
    	histogram2_f_t->Draw();
    	c3_f_t->cd();
    	c3_f_t->SetLogz();
    	histogram3_f_t->Draw("COLZ");	

	c1_f_t->SaveAs("Front_face_x_t_all.png");
    	c2_f_t->SaveAs("Front_face_y_t_all.png");
    	c3_f_t->SaveAs("Front_face_t_all.png");

}

