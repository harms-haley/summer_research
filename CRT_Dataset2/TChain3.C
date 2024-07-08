#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <TChain.h>

namespace fs = std::filesystem;

int TChain3() {
    std::string directory = "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/";

        std::vector<std::string> filenames = {                                                 	
		"/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13178_crtana.root",	
                "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13466_crtana.root",
                "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13476_crtana.root",
                "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13666_crtana.root",
                "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13680_crtana.root",
                "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13758_crtana.root",
                "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13690_crtana.root", 
                "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13828_crtana.root",
                "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13268_crtana.root",
                "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13830_crtana.root",
		"/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13689_crtana.root",
                "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13470_crtana.root",
                "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13688_crtana.root",
		"/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13281_crtana.root",
		"/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13320_crtana.root",
                "/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13693_crtana.root"
		};
	TChain chain("crtana/tree");
	for (const auto& filename : filenames) {
	chain.Add(filename.c_str());
	}

	Long64_t n_entries = chain.GetEntries();
	Long64_t nMaxEntries = 1000;
	
	std::cout << "Number of entries in the TChain: " << n_entries << std::endl;

	if (n_entries == 0) {
	std::cerr << "No entries found in the TChain. Please check the filenames and tree names." << std::endl;
	return 1;
	}

	std::vector<bool>* cl_has_sp = nullptr;
	std::vector<double>* cl_sp_x = nullptr;
	std::vector<double>* cl_sp_y = nullptr;
	std::vector<double>* cl_sp_z = nullptr;
	std::vector<double>* cl_sp_ts1 = nullptr;

	chain.SetBranchAddress("cl_has_sp", &cl_has_sp);
	chain.SetBranchAddress("cl_sp_x", &cl_sp_x);
	chain.SetBranchAddress("cl_sp_y", &cl_sp_y);
	chain.SetBranchAddress("cl_sp_z", &cl_sp_z);
	chain.SetBranchAddress("cl_sp_ts1", &cl_sp_ts1);

	TH1D* histogram1_f_t = new TH1D("histogram1Dft_x", "Front face (x)", 10, -400, 400);
	TH1D* histogram2_f_t = new TH1D("histogram1Dft_y", "Front face (y)", 10, -400, 400);
	TH2D* histogram3_f_t = new TH2D("histogram2Dft", "Front face", 10, -400, 400, 10, -400, 400);
	TH1D* histogram1_b_t = new TH1D("histogram1Dbt_x", "Back face (x)", 10, -400, 400);
	TH1D* histogram2_b_t = new TH1D("histogram1Dbt_y", "Back face (y)", 10, -400, 400);
	TH2D* histogram3_b_t = new TH2D("histogram2Dbt", "Back face", 10, -400, 400, 10, -400, 400);

	TCanvas* c1_f_t = new TCanvas("c1_f_t", "Front Face (x)", 800, 600);
	TCanvas* c2_f_t = new TCanvas("c2_f_t", "Front Face (y)", 800, 600);
	TCanvas* c3_f_t = new TCanvas("c3_f_t", "Front Face", 800, 600);
	TCanvas* c1_b_t = new TCanvas("c1_b_t", "Back Face (x)", 800, 600);
	TCanvas* c2_b_t = new TCanvas("c2_b_t", "Back Face (y)", 800, 600);
	TCanvas* c3_b_t = new TCanvas("c3_b_t", "Back Face", 800, 600);

	for (Long64_t i = 0; i < n_entries && i < nMaxEntries; ++i) {
        	chain.GetEntry(i);

        	if ((i % 10000) == 0) {
            		std::cout << i << "k" << std::endl;
       		}

        	if (cl_has_sp == nullptr || cl_has_sp->size() == 0 || !cl_has_sp->at(0)) {
            		continue;
        	}

        	for (size_t j = 0; j < cl_sp_x->size(); ++j) {
            		double t1 = cl_sp_ts1->at(j);
            		double x = cl_sp_x->at(j);
            		double y = cl_sp_y->at(j);
            		double z = cl_sp_z->at(j);
			if (1529.8e3 < t1 && t1 < 1532.8e3) {		
            			if (y > -350) {  //cut off feet of detector
	    				if (-250 < z && z < -150) {
                				histogram1_f_t->Fill(x);
                				histogram2_f_t->Fill(y);
                				histogram3_f_t->Fill(x, y);
            					}	 
					else if (750 < z && z < 850) {
   			             	histogram1_b_t->Fill(x);
                			histogram2_b_t->Fill(y);
                			histogram3_b_t->Fill(x, y);
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

    	c1_b_t->cd();
    	histogram1_b_t->Draw();
    	c2_b_t->cd();
    	histogram2_b_t->Draw();
    	c3_b_t->cd();
    	c3_b_t->SetLogz();
	histogram3_b_t->Draw("COLZ");

	c1_f_t->SaveAs("Front_face_x_t_all.png");
	c2_f_t->SaveAs("Front_face_y_t_all.png");
	c3_f_t->SaveAs("Front_face_t_all.png");

	c1_b_t->SaveAs("Back_face_x_t_all.png");
	c2_b_t->SaveAs("Back_face_y_t_all.png");
	c3_b_t->SaveAs("Back_face_t_all.png");

	return 0;
	}
