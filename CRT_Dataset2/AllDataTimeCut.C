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
#include <TH2.h>
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

void fit_gaussian2D(TH2F* histogram) {
	int n_binsX = histogram->GetNbinsX();
    	int n_binsY = histogram->GetNbinsY();
    	std::vector<double> dataX;
    	std::vector<double> dataY;
    	for (int i = 1; i <= n_binsX; ++i) {
        	for (int j = 1; j <= n_binsY; ++j) {
            		int bin_count = histogram->GetBinContent(i, j);
            		double bin_centerX = histogram->GetXaxis()->GetBinCenter(i);
            		double bin_centerY = histogram->GetYaxis()->GetBinCenter(j);
            		for (int k = 0; k < bin_count; ++k) {
                		dataX.push_back(bin_centerX);
                		dataY.push_back(bin_centerY);
            		}
        	}
    	}

	double meanX = calculate_mean(dataX);
    	double meanY = calculate_mean(dataY);
    	double stddevX = calculate_stddev(dataX, meanX);
    	double stddevY = calculate_stddev(dataY, meanY);

    	TF2 *fit_func = new TF2("fit_func", gaussian2D, histogram->GetXaxis()->GetXmin(), histogram->GetXaxis()->GetXmax(), histogram->GetYaxis()->GetXmin(), histogram->GetYaxis()->GetXmax(), 5);
    	fit_func->SetParameters(histogram->GetMaximum(), meanX, stddevX, meanY, stddevY);
    	histogram->Fit(fit_func, "N");

    	double chi2 = fit_func->GetChisquare();
    	int ndf = fit_func->GetNDF();

	std::cout << "#chi^{2} = " << chi2 << std::endl;
    	std::cout << "NDF = " << ndf << std::endl;

	gStyle->SetOptStat(0);
	double entries = histogram->GetEntries();
	std::cout << "Entries: " << entries << std::endl;	

}
void AllDataTimeCut() {
	TChain chain("crtana/tree"); // Ensure "myTree" is the correct name of your TTree
	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13688_crtana_22jul2024.root"); // Replace with your file path pattern
//	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13689_crtana_22jul2024.root");
//	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13693_crtana_22jul2024.root");
//	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run13758_crtana_22jul2024.root");


	//TH1F *histogram1_f_t = new TH1F("histogram1_f_t", "Front Face X;X;Counts", 10, -400, 400);
	//TH1F *histogram2_f_t = new TH1F("histogram2_f_t", "Front Face Y;Y;Counts", 10, -400, 400);
//	TH1F *histogram4_f_t = new TH1F("histogram4_f_t", "Time", 100, 1525e3 , 1540e3);
	TH2F *histogram3_f_t = new TH2F("histogram3_f_t", "Front Face XY", 10, -360, 360, 10, -360, 360);
    	//TCanvas *c1_f_t = new TCanvas("c1_f_t", "Front Face X");
	//TCanvas *c2_f_t = new TCanvas("c2_f_t", "Front Face Y");
	TCanvas *c3_f_t = new TCanvas("c3_f_t", "Front Face XY");
	//TCanvas *c4_f_t = new TCanvas("c4_f_t", "Time");

	std::vector<double> *cl_sp_x = nullptr;
    	std::vector<double> *cl_sp_y = nullptr;
    	std::vector<double> *cl_sp_z = nullptr;
    	std::vector<double> *cl_sp_ts1 = nullptr;
    	std::vector<bool> *cl_has_sp = nullptr;
	std::vector<double> *tdc_timestamp = nullptr;
	chain.SetBranchAddress("cl_has_sp", &cl_has_sp);
    	chain.SetBranchAddress("cl_sp_x", &cl_sp_x);
    	chain.SetBranchAddress("cl_sp_y", &cl_sp_y);
    	chain.SetBranchAddress("cl_sp_z", &cl_sp_z);
    	chain.SetBranchAddress("cl_sp_ts1", &cl_sp_ts1);
	chain.SetBranchAddress("tdc_timestamp", &tdc_timestamp);

	Long64_t nEntries = chain.GetEntries();
	const Long64_t maxEntries = 30000;
	nEntries = std::min(nEntries, maxEntries);	



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
			double t = tdc_timestamp->at(j);
			if (i % 10000 == 0) {
				std::cout << "Entry " << i << ": t=" << t << std::endl;
				if (1529e3 < t1 && t1 < 1533e3) {
					if (-360 < y && y < 360 && -360 < x && x < 360) {  // cut off feet of detector
                    				if (-200 < z && z < -100) {
                        				histogram3_f_t->Fill(x, y);
                    				}
                			}
            			}
    				c3_f_t->cd();
    				histogram3_f_t->Draw("COLZ");
				fit_gaussian2D(histogram3_f_t);	
			}
		}
	}
}
