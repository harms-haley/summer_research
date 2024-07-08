#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include <iostream>

void AllDataTimeCut() {
	TChain chain("crtana/tree"); // Ensure "myTree" is the correct name of your TTree
	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13*_crtana.root"); // Replace with your file path pattern
	TH1F *histogram1_f_t = new TH1F("histogram1_f_t", "Front Face X;X;Counts", 100, -400, 400);
	TH1F *histogram2_f_t = new TH1F("histogram2_f_t", "Front Face Y;Y;Counts", 100, -400, 400);
	TH1F *histogram4_f_t = new TH1F("histogram4_f_t", "Time", 100, 1520e3 , 1540e3);
	TH2F *histogram3_f_t = new TH2F("histogram3_f_t", "Front Face XY;X;Y;Counts", 100, -400, 400, 100, -400, 400);
    	TCanvas *c1_f_t = new TCanvas("c1_f_t", "Front Face X");
	TCanvas *c2_f_t = new TCanvas("c2_f_t", "Front Face Y");
	TCanvas *c3_f_t = new TCanvas("c3_f_t", "Front Face XY");
	TCanvas *c4_f_t = new TCanvas("c4_f_t", "Time");

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

	for (Long64_t i = 0; i < nEntries; ++i) {
        	chain.GetEntry(i);
		if (cl_has_sp == nullptr || cl_has_sp->size() == 0 || !cl_has_sp->at(0)) {
            		continue;
        	}
		
		for (size_t j = 0; j < cl_sp_x->size(); ++j) {
            		double t1 = cl_sp_ts1->at(j);
            		double x = cl_sp_x->at(j);
            		double y = cl_sp_y->at(j);
            		double z = cl_sp_z->at(j);
			if (i % 10000 == 0) {
				std::cout << "Entry " << i << ", Hit " << j << ": t1=" << t1 << ", x=" << x << ", y=" << y << ", z=" << z << std::endl;
			}
			
			histogram4_f_t->Fill(t1);
			
			if (1529e3 < t1 && t1 < 1534e3) {
				if (y > -350 && -370 < x && x < 370) {  // cut off feet of detector
                    			if (-200 < z && z < -100) {
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
	c4_f_t->cd();
	histogram4_f_t->Draw();

	c1_f_t->SaveAs("Front_face_x_t_all.png");
    	c2_f_t->SaveAs("Front_face_y_t_all.png");
    	c3_f_t->SaveAs("Front_face_t_all.png");
	c4_f_t->SaveAs("Time.png");

}

