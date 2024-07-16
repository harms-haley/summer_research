#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include <iostream>
#include "TF1.h"
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include "TPaveText.h"
#include "TF2.h"
#include <TStyle.h>
#include <TMath.h>
#include <TLine.h>
double gaussian(double *x, double *par) {
	double arg = (x[0] - par[1]) / par[2];
    	return par[0] * exp(-0.5 * arg * arg);
}

double gaussian2D(double *x, double *par) {
	double argX = (x[0] - par[1]) / par[2];
  	double argY = (x[1] - par[3]) / par[4];
    	return par[0] * exp(-0.5 * (argX * argX + argY * argY));
	
}


double calculate_mean(const std::vector<double>& data) {
	return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}
double calculate_stddev(const std::vector<double>& data, double mean) {
	double sum = 0.0;
    	for (double value : data) {
        	sum += (value - mean) * (value - mean);
    	}
    	return sqrt(sum / data.size());
}



void fit_gaussian1D(TH1F* histogram) {
	int n_bins = histogram->GetNbinsX();
    	std::vector<double> data;
    	for (int i = 1; i <= n_bins; ++i) {
        	int bin_count = histogram->GetBinContent(i);
        	double bin_center = histogram->GetBinCenter(i);
        	for (int j = 0; j < bin_count; ++j) {
            		data.push_back(bin_center);
        	}
    	}	
	double mean = calculate_mean(data);
    	double stddev = calculate_stddev(data, mean);

    	TF1 *fit_func = new TF1("fit_func", gaussian, histogram->GetXaxis()->GetXmin(), histogram->GetXaxis()->GetXmax(), 3);
    	fit_func->SetParameters(histogram->GetMaximum(), mean, stddev);
    	histogram->Fit(fit_func, "R");
	
	double chi2 = fit_func->GetChisquare();
    	int ndf = fit_func->GetNDF();

	gStyle->SetOptStat(0);
	double entries = histogram->GetEntries();
	std::cout << "Entries: " << entries << std::endl;

}


void AllData3DHist() {
	TChain chain("crtana/tree"); // Ensure "myTree" is the correct name of your TTree
	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13*_crtana.root"); // Replace with your file path pattern
	TH1F *histogram1_f_nb = new TH1F("histogram1_f_nb", "Front Face X;No Beam", 10, -360, 360);
	histogram1_f_nb->GetXaxis()->SetTitle("X Location (cm)");
    	histogram1_f_nb->GetYaxis()->SetTitle("Number of Hits");

	TH1F *histogram2_f_nb = new TH1F("histogram2_f_nb", "Front Face Y;No Beam", 10, -360, 360);
	histogram2_f_nb->GetXaxis()->SetTitle("Y Location (cm)");
        histogram2_f_nb->GetYaxis()->SetTitle("Number of Hits");

	TH2F *histogram3_f_nb = new TH2F("histogram3_f_nb", "Front Face XY;No Beam", 10, -360, 360, 10, -360, 360);
	histogram3_f_nb->GetXaxis()->SetTitle("X Location (cm)");
        histogram3_f_nb->GetYaxis()->SetTitle("Y Location (cm)");

	TH1F *histogram4_f_tc = new TH1F("histogram4_f_tc", "Front Face X; Time Cut", 10, -360, 360);
        histogram4_f_tc->GetXaxis()->SetTitle("X Location (cm)");
        histogram4_f_tc->GetYaxis()->SetTitle("Number of Hits");

	TH1F *histogram5_f_tc = new TH1F("histogram5_f_tc", "Front Face Y; Time Cut", 10, -360, 360);
        histogram5_f_tc->GetXaxis()->SetTitle("Y Location (cm)");
        histogram5_f_tc->GetYaxis()->SetTitle("Number of Hits");

	TH2F *histogram6_f_tc = new TH2F("histogram6_f_tc", "Front Face XY; Time Cut", 10, -360, 360, 10, -360, 360);
	histogram6_f_tc->GetXaxis()->SetTitle("X Location (cm)");
        histogram6_f_tc->GetYaxis()->SetTitle("Y Location (cm)");
	//histogram6_f_tc->SetContour(contours.size(), contours.data());

    	TCanvas *c1_f_nb = new TCanvas("c1_f_nb", "Front Face X NB");
	TCanvas *c2_f_nb = new TCanvas("c2_f_nb", "Front Face Y NB");
	TCanvas *c3_f_nb = new TCanvas("c3_f_nb", "Front Face XY NB");
	TCanvas *c4_f_tc = new TCanvas("c4_f_tc", "Front Face X TC");
        TCanvas *c5_f_tc = new TCanvas("c5_f_tc", "Front Face Y TC");
        TCanvas *c6_f_tc = new TCanvas("c6_f_tc", "Front Face XY TC");
	TCanvas *c7_f_ob = new TCanvas("c7_f_ob", "Front Face X OB");
        TCanvas *c8_f_ob = new TCanvas("c8_f_ob", "Front Face Y OB");
        TCanvas *c9_f_ob = new TCanvas("c9_f_ob", "Front Face XY OB");
	TCanvas *c3D_f_nb = new TCanvas("c3D_f_nb", "3D Front Face XY NB", 800, 600);
   	TCanvas *c3D_f_tc = new TCanvas("c3D_f_tc", "3D Front Face XY TC", 800, 600);
    	TCanvas *c3D_f_ob = new TCanvas("c3D_f_ob", "3D Front Face XY OB", 800, 600);

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
	
	//gStyle->SetNumberContours(1);
	gStyle->SetOptStat(0);
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
			
			if (1529e3 < t1 && t1 < 1534e3) {
				//if (y > -350 && -370 < x && x < 370) {
					if (-200 < z && z < -100) {
						histogram4_f_tc->Fill(x);
						histogram5_f_tc->Fill(y);
						histogram6_f_tc->Fill(x,y);
					}
				//}
			}
			
			if (0 < t1 && t1 < 1529e3 || 1534e3 < t1 && t1 < 1000e6) {
				//if (y > -350 && -370 < x && x < 370) {  // cut off feet of detector
                    			if (-200 < z && z < -100) {
                        			histogram1_f_nb->Fill(x);
                        			histogram2_f_nb->Fill(y);
                        			histogram3_f_nb->Fill(x, y);
                    			}
                		//}
            		}
        	}
    	}


	TH1F *histogramx = (TH1F*)histogram4_f_tc->Clone("histogramx");
	histogramx->Add(histogram1_f_nb, -1.0 / 199999);
	histogramx->GetXaxis()->SetTitle("X Location (cm)");
        histogramx->GetYaxis()->SetTitle("Number of Hits)");

	TH1F *histogramy = (TH1F*)histogram5_f_tc->Clone("histogramy");
        histogramy->Add(histogram2_f_nb, -1.0 / 199999);
	histogramy->GetXaxis()->SetTitle("Y Location (cm)");
        histogramy->GetYaxis()->SetTitle("Number of Hits");

	TH2F *histogramxy = (TH2F*)histogram6_f_tc->Clone("histogramxy");
        histogramxy->Add(histogram3_f_nb, -1.0 / 199999);
	histogramxy->GetXaxis()->SetTitle("X Location (cm)");
        histogramxy->GetYaxis()->SetTitle("Y Location (cm)");


    	c3_f_nb->cd();
    	histogram3_f_nb->Draw("COLZ");
	
	c6_f_tc->cd();
        histogram6_f_tc->Draw("COLZ");


	c9_f_ob->cd();
        histogramxy->Draw("COLZ");


	c3D_f_nb->cd();
    	histogram3_f_nb->Draw("LEGO2");

   	c3D_f_tc->cd();
    	histogram6_f_tc->Draw("LEGO2");

    	c3D_f_ob->cd();
    	histogramxy->Draw("LEGO2");

	
	c3D_f_nb->SaveAs("Front_face_nb_3D.png");
    	c3D_f_tc->SaveAs("Front_face_tc_3D.png");
    	c3D_f_ob->SaveAs("Front_face_ob_3D.png");

}

